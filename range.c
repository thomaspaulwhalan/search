/*
 *
 * Programmed by Thomas Paul Whalan
 *
 * This program has been written to help search for strings within a file */

#ifndef RANGE_H
#include<range.h>

int getleftvalue(char *args)
{
	int leftvalue = 0;
	char range[16] = "";

	for (int x = 0; x < strlen(args); x++) {
		if (args[x] == '-') {
			range[x] = '\0';
			break;
		}
		range[x] = args[x];
	}

	leftvalue = atoi(range);

	return leftvalue;
}

int getrightvalue(char *args)
{
	int rightvalue = 0;
	char range[16] = "";
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

	rightvalue = atoi(range);

	return rightvalue;
}

#endif // RANGE_H
