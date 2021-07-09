/*
 *
 * Programmed by Thomas Paul Whalan
 *
 * This program has been written to help search for strings within a file */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<ctype.h>

#include<range.h>
#include<nerror.h>

#define OPTION_IGNORE 	1	// 0b00000001
#define OPTION_ISOLATE 	2	// 0b00000010
#define OPTION_LINES	4	// 0b00000100
#define OPTION_RANGE	8	// 0b00001000
#define OPTION_SAVE	16	// 0b00010000

int main(int argc, char *argv[])
{
	/* If the arg count is less than 3, we look to see if -h or --help
	 * has been invoked. If yes, the help menu is displayed. If no,
	 * we get an error that is sent to standard error */
	if (argc < 3) {
		FAIL_IF_R(argc == 1, 1, stderr, "Usage: search [OPTION]... TERM FILE\nTry 'search --help' for more information\n");
		
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			puts("Search help:\n\tUSAGE: search [OPTION]... TERM FILE\n");
			puts("\t-h, --help\t\tShow this help dialog\n");
			puts("\t-i, --ignore-case\tSearch is not case sensitive\n");
			puts("\t-I, --isolate\t\tOnly return a word where it is\n\t\t\t\tan exact match, ie, not part of\n\t\t\t\ta compound word or hyphenated\n\t\t\t\tword.\n");
			puts("\t-l, --lines\t\tDisplay line numbers in results\n\t\t\t\tand the position of the start of\n\t\t\t\tthe word\n");
			puts("\t-r, --range\t\tDisplay results only from a\n\t\t\t\tgiven range of lines in the file.\n\t\t\t\tUsed in the format of num-num.\n\t\t\t\tEG: -r 50-75\n");
			puts("\t-s, --save\t\tSave results to a file\n");
			puts("\tSearch is able to be used with other programms.\n\tTry piping the results of search into\n\tgrep to narrow down search results\n");
			puts("\tEG: search Port /etc/ssh/sshd_config | grep 22");

			return 0;
		}
		else {
			fprintf(stderr, "USAGE: search [OPTION]... TERM FILE\nTry 'search --help' for more information\n");

			return 1;
		}
	}

	/* These are the variables with which we will keep track of what
	 * options have been specified */
	uint8_t option_field = 0;
	uint8_t saveposition = 0;
	uint8_t rangeposition = 0;
	
	/* We will loop through argv from position 1 to the 3rd to last position.
	 * That way we can keep track of what options have been used */
	for (int flagcheck = 1; flagcheck < argc-2; flagcheck++) {
		/* If -l or --lines is specified, lflag is given the value of 1 */
		if (strcmp(argv[flagcheck], "-i") == 0 || strcmp(argv[flagcheck], "--ignore-case") == 0) {
			FAIL_IF_R(option_field & OPTION_IGNORE, 1, stderr, "ERROR: You can only employ a flag once\n");
			option_field ^= OPTION_IGNORE;
		}

		else if (strcmp(argv[flagcheck], "-I") == 0 || strcmp(argv[flagcheck], "--isolate") == 0) {
			FAIL_IF_R(option_field & OPTION_ISOLATE, 1, stderr, "ERROR: You can only employ a flag once\n");
			option_field ^= OPTION_ISOLATE;
		}

		else if (strcmp(argv[flagcheck], "-l") == 0 || strcmp(argv[flagcheck], "--lines") == 0) {
			FAIL_IF_R(option_field & OPTION_LINES, 1, stderr, "ERROR: You can only employ a flag once\n");
			option_field ^= OPTION_LINES;
		}
		/* if -h or --help is specified, we let the user know that we would rather
		 * the -h and --help options be specified without any other flags */
		else if (strcmp(argv[flagcheck], "-h") == 0 || strcmp(argv[flagcheck], "--help") == 0) {
			fprintf(stderr, "\nERROR: Help can only be invoked on its own\n");
			fprintf(stderr, "Try:\n\tsearch -h\nor\n\tsearch --help\n\n");

			return 1;
		}
		else if (strcmp(argv[flagcheck], "-s") == 0 || strcmp(argv[flagcheck], "--save") == 0) {
			FAIL_IF_R(option_field & OPTION_SAVE, 1, stderr, "ERROR: You can only employ a flag once\n");
			FAIL_IF_R(flagcheck + 1 == argc - 2, 1, stderr, "ERROR: File not specified\n");
			flagcheck++; // Increment flagcheck so that the save position is not checked as an option
			saveposition = flagcheck;
			option_field ^= OPTION_SAVE;
		}
		else if (strcmp(argv[flagcheck], "-r") == 0 || strcmp(argv[flagcheck], "--range") == 0) {
			FAIL_IF_R(option_field & OPTION_RANGE, 1, stderr, "ERROR: You can only employ a flag once\n");
			FAIL_IF_R(flagcheck + 1 == argc - 2, 1, stderr, "ERROR: Range not specified\n");
			flagcheck++; // Increment flagcheck so that the range position is not checked as an option
			rangeposition = flagcheck;
			option_field ^= OPTION_RANGE;
		}
		else {
			fprintf(stderr, "\nERROR: \"%s\" is not a valid option\n\n", argv[flagcheck]);
			return 1;
		}
	}

	FILE *searchfile = searchfile = fopen(argv[argc-1], "r");
	FAIL_IF_R(searchfile == NULL, 1, stderr, "search: Could not open search file\n");

	FILE *file_stream;
	if (option_field & OPTION_SAVE) {
		file_stream = fopen(argv[saveposition], "w");
		FAIL_IF_R(file_stream == NULL, 1, stderr, "search: Could not open save file\n");
	}
	else {
		file_stream = stdout;
	}

	/* searchsize is set to the string length of the search term which is located
	 * as the second to last option on the command line */
	int searchsize = strlen(argv[argc-2]);
	/* We will use searchword for our comparisons */
	char searchword[128];
	strcpy(searchword, argv[argc-2]);
	char linebuff[1024];
	int linecount = 1;

	/* Get the left and right values from range and sort into the correct variables */
	int lowerrange = getleftvalue(argv[rangeposition]);
	int upperrange = getrightvalue(argv[rangeposition]);
	if (upperrange < lowerrange) {
		int intholder = upperrange;
		upperrange = lowerrange;
		lowerrange = intholder;
	}
	printf("Searching for \"%s\" in %s\n", argv[argc-2], argv[argc-1]);

	while (fgets(linebuff, 1024, searchfile)) {
		int counter;
		for (counter = 0; counter < strlen(linebuff); counter++) {
			/* If the first letter matches, start comparing the characters
			 * in searchword[]
			 * */
			if ((linebuff[counter] == searchword[0]) || ((option_field & OPTION_IGNORE) && (toupper(linebuff[counter]) == toupper(searchword[0])))) {
				for (int x = 0; x <= searchsize; x++) {
					if ((linebuff[counter+x] == searchword[x]) || ((option_field & OPTION_IGNORE) && (toupper(linebuff[counter+x]) == toupper(searchword[x])))) {
						if (x == searchsize-1) {
							/* Test to see which other options shall be employed
							 * */
							if (option_field & OPTION_ISOLATE) {
								if ((counter == 0 || linebuff[counter-1] == ' ') && (linebuff[counter+searchsize] == ' ' || linebuff[counter+searchsize] == '\n')) {
									if (option_field & OPTION_LINES) {
										if (option_field & OPTION_RANGE) {
											if (linecount >= lowerrange && linecount <= upperrange) {
												fprintf(file_stream, "LINE %d, POS %d: ", linecount, counter+1);
											}
										}
										else {
											fprintf(file_stream, "LINE %d, POS %d: ", linecount, counter+1);
										}
									}
									if (option_field & OPTION_RANGE) {
										if (linecount >= lowerrange && linecount <= upperrange) {
											fprintf(file_stream, "%s", linebuff);
										}
									}
									else {
										fprintf(file_stream, "%s", linebuff);
									}
								}
							}
							else {
								if (option_field & OPTION_LINES) {
									if (option_field & OPTION_RANGE) {
										if (linecount >= lowerrange && linecount <= upperrange) {
											fprintf(file_stream, "LINE %d, POS %d: ", linecount, counter+1);
										}
									}
									else {
										fprintf(file_stream, "LINE %d, POS %d: ", linecount, counter+1);
									}
								}
								if (option_field & OPTION_RANGE) {
									if (linecount >= lowerrange && linecount <= upperrange) {
										fprintf(file_stream, "%s", linebuff);
									}
								}
								else {
									fprintf(file_stream, "%s", linebuff);
								}
							}
						}
					}
					/* If the characters don't match, break
					 * */
					else {
						break;
					}
				}
			}
		}
		linecount++;
	}

	fclose(searchfile);
	if (option_field & OPTION_SAVE) {
		printf("Results written to %s\n", argv[saveposition]);
		fclose(file_stream);
	}

	return 0;
}
