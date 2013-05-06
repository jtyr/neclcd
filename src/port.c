/*
 * Name: port.c
 * Description: Functions for RS-232 comunication
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard library definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "debug.h"
#include "port.h"


void read_port(int *fd, char **replay_msg) {
	char buffer[8];  /* Input buffer */
	int  nbytes;     /* Number of bytes read */
	int  i, n = 0;

	if (getDebug() > 1) printf("I: Read port:\n");

	/* read characters into our string buffer */
	while ((nbytes = read(*fd, buffer, buffer + sizeof(buffer) - buffer - 1)) > 0) {
		if (getDebug() > 1) 
			for (i=0; i<nbytes; i++)
				printf("\t%02d: %#x\n", i, buffer[i]);

		*replay_msg = (char *) realloc(*replay_msg, (n+nbytes)*sizeof(char));
		strncat(*replay_msg, buffer, nbytes);
		n += nbytes;
	}

	if (getDebug() > 1) puts("OK");
}


int write_port(int *fd, char *s) {
	int n, ret = 1;

	if (getDebug() > 1) printf("I: Write port: ");

	n = write(*fd, s, strlen(s));
	if (n < 0) {
		if (getDebug() > 1 ) perror("\nE: write() failed!");
		ret = -1;
	} else
		if (getDebug() > 1) puts("OK");

	return ret;
}


int open_port(void) {
	int fd; /* File descriptor for the port */

	if (getDebug() > 1) printf("I: Open port: ");

	fd = open(PORT, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1)
		perror("E: open_port: Unable to open /dev/ttyS0 - ");
	else
		fcntl(fd, F_SETFL, 0);


	if (getDebug() > 1) puts("OK");

	return (fd);
}


void setup_port(int *fd) {
	struct termios options;


	/* default stty settings:
	   stty -F /dev/ttyS0 1100:5:cbd:8a31:3:1c:7f:15:4:1e:0:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0
	 */


	if (getDebug() > 1) printf("I: Port setting: ");


	/* Get the current options for the port */
	tcgetattr(*fd, &options);


	/*
	 * CONTROL OPTIONS
	 */

	/* Set the baud rates to 9600 bps */
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);

	/* Setting no Parity Checking, One stop bit, Bit mask for data bits */
	options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);

	/* Seting the Character Size - Select 8 data bits */
	options.c_cflag |= CS8;


	/*
	 * LOCAL OPTIONS
	 */

	/* Disable Canonical Input */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


	/*
	 * INPUT OPTIONS
	 */

	/* Flags for raw input */
	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);


	/*
	 * OUTPUT OPTIONS
	 */

	/* Disable Processed Output = raw */
	options.c_oflag &= ~OPOST;


	/*
	 * CONTROL CHARACTERS
	 */

	/* Minimum number of characters to read */
	options.c_cc[VMIN] = 0;

	/* Time to wait for data = 1 second timeout */
	options.c_cc[VTIME] = TIMEOUT;


	/* set raw input completly - `man cfmakeraw` */
	//cfmakeraw(&options);


	/*
	 * SET THE NEW OPTIONS FOR THE PORT
	 */
	tcsetattr(*fd, TCSANOW, &options);


	if (getDebug() > 1) puts("OK");
}
