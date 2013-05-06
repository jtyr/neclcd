/*
 * Name: string.c
 * Description: Function which returns the substring of the source string
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include <stdlib.h>
#include <stdio.h>


char *substr(char *src, unsigned int begin, unsigned int cnt) {
	char *ret;
	int i;

	ret = calloc(cnt+1, sizeof(char));

	for (i=begin; i<(begin+cnt); i++)
		ret[i-begin] = src[i];

	return ret;
}
