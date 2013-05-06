/*
 * Name: debug.c
 * Description: Functions for set and get DEBUG variable
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include "debug.h"


unsigned int DEBUG = 0;


void setDebug(unsigned int value) {
	DEBUG = value;
}


int getDebug(void) {
	return DEBUG;
}
