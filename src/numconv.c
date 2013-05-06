/*
 * Name: numconv.c
 * Description: Functions for special number and string conversion
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int strhex2int(char *hex) {
	int num = 0;

        sscanf(hex, "%x", &num);

	return num;
}


char *int2strhex(int num, int n) {
	char *buffer, *tmp;
	int i, len, max = 20;

	buffer = calloc(max, sizeof(char));
	snprintf(buffer, max*sizeof(char), "%X", num);

	len = strlen(buffer);

	if (len < n) {
		tmp = calloc(n+1, sizeof(char));

		for (i=0; i<n-len; i++)
			tmp[i] = '0';

		buffer = strcat(tmp, buffer);
	}

	return buffer;
}
