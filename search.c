/*
 *
 * Programmed by Thomas Paul Whalan
 *
 * This program has been written to help search for strings within a file */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int getupperrange(char *args);

int getlowerrange(char *args);

int main(int argc, char *argv[])
{
	/* If the arg count is less than 3, we look to see if -h or --help
	 * has been invoked. If yes, the help menu is displayed. If no,
	 * we get an error that is sent to standard error */
	if (argc < 3) {
		/* If not used... */
		if (argc == 1) {
			fprintf(stderr, "Uusage: search [OPTION]... TERM FILE\nTry 'search --help' for more information\n");
			return 1;
		}
		/* If used, we get our helpful hints */
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
			fprintf(stderr, "Uusage: search [OPTION]... TERM FILE\nTry 'search --help' for more information\n");

			return 1;
		}
	}

	/* These are the variables with which we will keep track of what
	 * options have been specified */
	int iflag = 0;
	int Iflag = 0;
	int lflag = 0;
	int rflag = 0;
	int sflag = 0;
	int saveposition = 0;
	int rangeposition = 0;
	
	/* We will loop through argv from position 1 to the 3rd to last position.
	 * That way we can keep track of what options have been used */
	for (int flagcheck = 1; flagcheck < argc-2; flagcheck++) {
		/* If -l or --lines is specified, lflag is given the value of 1 */
		if (strcmp(argv[flagcheck], "-i") == 0 || strcmp(argv[flagcheck], "--ignore-case") == 0) {
			puts("ignoring cases is not yet implemented");
			if (lflag == 1) {
				fprintf(stderr, "\nERROR: You can only employ a flag once\n\n");
				return 1;
			}
			else {
				iflag = 1;
			}
		}

		else if (strcmp(argv[flagcheck], "-I") == 0 || strcmp(argv[flagcheck], "--isolate") == 0) {
			if (Iflag == 1) {
				fprintf(stderr, "\nERROR: You can only employ a flag once\n\n");
				return 1;
			}
			else {
				Iflag = 1;
			}
		}

		else if (strcmp(argv[flagcheck], "-l") == 0 || strcmp(argv[flagcheck], "--lines") == 0) {
			if (lflag == 1) {
				/* If -l or --lines has already been specified, we print an error.
				 * Afterall, there is no use in writing out an option twice */
				fprintf(stderr, "\nERROR: You can only employ a flag once\n\n");
				return 1;
			}
			else {
				lflag = 1;
			}
		}
		/* if -h or --help is specified, we let the user know that we would rather
		 * the -h and --help options be specified without any other flags */
		else if (strcmp(argv[flagcheck], "-h") == 0 || strcmp(argv[flagcheck], "--help") == 0) {
			fprintf(stderr, "\nERROR: Help can only be invoked on its own\n");
			fprintf(stderr, "Try:\n\tsearch -h\nor\n\tsearch --help\n\n");

			return 1;
		}
		else if (strcmp(argv[flagcheck], "-s") == 0 || strcmp(argv[flagcheck], "--save") == 0) {
			if (sflag == 1) {
				fprintf(stderr, "\nERROR: You can only employ a flag once\n\n");
				return 1;
			}
			else if (flagcheck + 1 == argc - 2) {
				fprintf(stderr, "ERROR: Missing save file name\n");
				return 1;
			}
			else {
				saveposition = flagcheck + 1;
				sflag = 1;
				flagcheck++;
			}
		}
		else if (strcmp(argv[flagcheck], "-r") == 0 || strcmp(argv[flagcheck], "--range") == 0) {
			if (rflag == 1) {
				fprintf(stderr, "\nERROR: You can only employ a flag once\n\n");
				return 1;
			}
			else if (flagcheck + 1 == argc - 2) {
				fprintf(stderr, "ERROR: Range not specified\n");
				return 1;
			}
			else {
				rflag = 1;
				rangeposition = flagcheck + 1;
				flagcheck++;
			}
		}
		else {
			/* If a flag is specified that we do not recognise, we will print
			 * an error. */
			fprintf(stderr, "\nERROR: \"%s\" is not a valid option\n\n", argv[flagcheck]);
			return 1;
		}
	}

	/* Open file. If the file pointer is NULL, we return an error */
	FILE *myfile;
	myfile = fopen(argv[argc-1], "r");
	if (myfile == NULL) {
		fprintf(stderr, "Could not open file\n");
		return 1;
	}

	FILE *savefile;
	if (sflag == 1) {
		savefile = fopen(argv[saveposition], "w");
		if (savefile == NULL) {
			fprintf(stderr, "Error opening save file\n");
			return 1;
		}
	}

	if (Iflag == 1) {
		puts("Only using exact matches");
	}

	/* searchsize is set to the string length of the search term which is located
	 * as the second to last option on the command line */
	int searchsize = strlen(argv[argc-2]);
	/* We will use searchword for our comparisons */
	char searchword[128];
	strcpy(searchword, argv[argc-2]);
	/* We use linebuff to read the desired file line by line. 1024 bytes is probably overkill,
	 * but you just never know when you might come across a crazy long line in a code file
	 * or something */
	char linebuff[1024];
	/* linecount keeps track of what line we are up to so we can
	 * print it out with a search result */
	int linecount = 1;

	int upperrange = getupperrange(argv[rangeposition]);
	int lowerrange = getlowerrange(argv[rangeposition]);
	if (upperrange < lowerrange) {
		int intholder = upperrange;
		upperrange = lowerrange;
		lowerrange = intholder;
	}

	printf("Searching for \"%s\" in %s\n", argv[argc-2], argv[argc-1]);

	/* Time to go through the file */
	while (fgets(linebuff, 1024, myfile)) {
		/* After filling linebuff in the while loop, we use a for loop to go through linebuff
		 * for comparison with our search term */
		int counter;
		for(counter = 0; counter < strlen(linebuff); counter++) {
			/* If the first letter of our search term matches a letter in linebuff,
			 * We first check to see if our search term is a single letter or not.
			 * This is accomplished by checking whether or not the next position
			 * is a null terminator*/
			if (linebuff[counter] == searchword[0]) {
				if (searchword[1] == '\0') {
					if (Iflag == 1) {
						if ((counter == 0 || linebuff[counter-1] == ' ') && (linebuff[counter+1] == ' ' || linebuff[counter+1] == '\n')) {
							if (lflag == 1) {
								if (sflag == 1) {
									if (rflag == 1) {
										if (linecount >= lowerrange && linecount <= upperrange) {
											fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
										}
									}
									else {
										fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
									}
								}
								else {
									if (rflag == 1) {
										if (linecount >= lowerrange && linecount <= upperrange) {
											printf("LINE %d, POS %d: ", linecount, counter+1);
										}
									}
									else {
										printf("LINE %d, POS %d: ", linecount, counter+1);
									}
								}
							}
							if (sflag == 1) {
								if (rflag == 1) {
									if (linecount >= lowerrange && linecount <= upperrange) {
										fprintf(savefile, "%s", linebuff);
									}
								}
								else {
									fprintf(savefile, "%s", linebuff);
								}
							}
							else {
								if (rflag == 1) {
									if (linecount >= lowerrange && linecount <= upperrange) {
										printf("%s", linebuff);
									}
								}
								else {
									printf("%s", linebuff);
								}
							}
						}
					}
					else {
						if (lflag == 1) {
							if (sflag == 1) {
								if (rflag == 1) {
									if (linecount >= lowerrange && linecount <= upperrange) {
										fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
									}
								}
								else {
									fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
								}
							}
							else {
								if (rflag == 1) {
									if (linecount >= lowerrange && linecount <= upperrange) {
										printf("LINE %d, POS %d: ", linecount, counter+1);
									}
								}
								else {
									printf("LINE %d, POS %d: ", linecount, counter+1);
								}
							}
						}
						if (sflag == 1) {
							if (rflag == 1) {
								if (linecount >= lowerrange && linecount <= upperrange) {
									fprintf(savefile, "%s", linebuff);
								}
							}
							else {
								fprintf(savefile, "%s", linebuff);
							}
						}
						else {
							if (rflag == 1) {
								if (linecount >= lowerrange && linecount <= upperrange) {
									printf("%s", linebuff);
								}
							}
							else {
								printf("%s", linebuff);
							}
						}
					}
					break;
				}
				/* If the search term is longer than one letter, we continue here */
				else {
					/* Using a for loop, we iterate through both the search word
					 * and linebuff and check them against each other. If we get
					 * to the end of our search word, we have found a match, so
					 * we will print out our results */
					for (int x = 1; x <= searchsize; x++) {
						if (linebuff[counter+x] == searchword[x]) {
							if (x == searchsize-1) {
								if (Iflag == 1) {
									if ((counter == 0 || linebuff[counter-1] == ' ') && (linebuff[counter+searchsize] == ' ' || linebuff[counter+searchsize] == '\n')) {
										if (lflag == 1) {
											if (sflag == 1) {
												if (rflag == 1) {
													if (linecount >= lowerrange && linecount <= upperrange) {
														fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
													}
												}
												else {
													fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
												}
											}
											else {
												if (rflag == 1) {
													if (linecount >= lowerrange && linecount <= upperrange) {
														printf("LINE %d, POS %d: ", linecount, counter+1);
													}
												}
												else {
													printf("LINE %d, POS %d: ", linecount, counter+1);
												}
											}
										}
										if (sflag == 1) {
											if (rflag == 1) {
												if (linecount >= lowerrange && linecount <= upperrange) {
													fprintf(savefile, "%s", linebuff);
												}
											}
											else {
												fprintf(savefile, "%s", linebuff);
											}
										}
										else {
											if (rflag == 1) {
												if (linecount >= lowerrange && linecount <= upperrange) {
													printf("%s", linebuff);
												}
											}
											else {
												printf("%s", linebuff);
											}
										}
									}
								}
								else {
									if (lflag == 1) {
										if (sflag == 1) {
											if (rflag == 1) {
												if (linecount >= lowerrange && linecount <= upperrange) {
													fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
												}
											}
											else {
												fprintf(savefile, "LINE %d, POS %d: ", linecount, counter+1);
											}
										}
										else {
											if (rflag == 1) {
												if (linecount >= lowerrange && linecount <= upperrange) {
													printf("LINE %d, POS %d: ", linecount, counter+1);
												}
											}
											else {
												printf("LINE %d, POS %d: ", linecount, counter+1);
											}
										}
									}
									if (sflag == 1) {
										if (rflag == 1) {
											if (linecount >= lowerrange && linecount <= upperrange) {
												fprintf(savefile, "%s", linebuff);
											}
										}
										else {
											fprintf(savefile, "%s", linebuff);
										}
									}
									else {
										if (rflag == 1) {
											if (linecount >= lowerrange && linecount <= upperrange) {
												printf("%s", linebuff);
											}
										}
										else {
											printf("%s", linebuff);
										}
									}
								}
							}
						}
						/* If the characters don't match, we will break out
						 * of the for loop and continue searching through
						 * linebuff with the first letter of our search term
						 * (see above) */
						else {
							break;
						}
					}
				}
			}
		}
		/* Don't forget to increment linecount to keep track of every line
		 * that we have gone through */
		linecount++;
	}

	/* Wrap things up */
	fclose(myfile);
	if (sflag == 1) {
		printf("Results written to %s\n", argv[saveposition]);
		fclose(savefile);
	}

	/* Goodbye */
	return 0;
}

int getlowerrange(char *args)
{
	int lowerrange;
	char range[16];

	for (int x = 0; x < strlen(args); x++) {
		if (args[x] == '-') {
			range[x] = '\0';
			break;
		}
		range[x] = args[x];
	}

	lowerrange = atoi(range);

	return lowerrange;
}

int getupperrange(char *args)
{
	int upperrange;
	char range[16];
	int flip = 0;

	int y = 0;
	for (int x = 0; x < strlen(args); x++) {
		if (flip == 1) {
			range[y] = args[x];
			y++;
		}
		if (args[x] == '-') {
			flip = 1;
		}
	}

	upperrange = atoi(range);

	return upperrange;
}
