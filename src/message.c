/*
 * Name: message.c
 * Description: Functions for creating and parsing messages from LCD
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "debug.h"
#include "message.h"
#include "numconv.h"
#include "port.h"
#include "string.h"


void convert(char *str, int c) {
	int i;

	if (getDebug() > 2) printf("I: HEX to string: %X\n", c);
	sprintf(str, "%X", c);

	if (strlen(str) == 1) {
		str[1] = str[0];
		str[0] = '0';
		str[2] = '\0';
	}

	if (getDebug() > 2)
		for (i=0; i<strlen(str); i++)
			printf("   * char %d = %d\n", i, str[i]);
}


void print(int n, char *str) {
	int i;

	for (i=0; i<n; i++) {
		printf("%#x", str[i]);
		if (i+1 != n )
			printf(" ");
	}
}


void send_message(char *str, char **replay_msg) {
	int fd, w;

	/* open port */
	fd = open_port();

	/* setup port */
	setup_port(&fd);

	/* write port */
	w = write_port(&fd, str);

	/* read replay */
	if (w > 0) read_port(&fd, replay_msg);

	/* close port */
	close(fd);
}


void initMessage(MESSAGE *m, char message_type, int message_length) {
	if (getDebug() > 0) printf("I: Initialisation message: ");

	/* head */
	m->soh = SOH;
	m->reserve = RES;
	m->message_type = message_type;
	m->message_length = message_length;

	/* message */
	m->stx = STX;
	m->etx = ETX;

	/* delimiter */
	m->cr = CR;

	if (getDebug() > 0) puts("OK");
}


int sendMessage(MESSAGE *m) {
	int i, bcc = 0, error = 0;
	char ocp[3], oc[3], cmd[3], head[7], message[m->message_length];

	/* convert HEX to string */
	if (m->message_type == 'A') {
		convert(cmd,  m->command);
	} else {
		convert(ocp,  m->op_code_page);
		convert(oc,   m->op_code);
	}

	if (getDebug() > 0) {
		puts(">>>> >>>  >>   >");
		puts("I: Preparing message:");
	}

	/* create head string */
	sprintf(head, "%c%c%c%c%c%s",
		m->soh,
		m->reserve,
		m->destination,
		m->source,
		m->message_type,
		int2strhex(m->message_length, 2)
	);
	if (getDebug() > 0) {
		printf("   * Head       = [");
		print(7, head);
		printf("]\n");
	}

	/* create message string */
	if (m->message_type == 'A')
		/* command */
    		sprintf(message, "%c%s%c",
			m->stx,
			cmd,
			m->etx
		);
	else if (m->message_type == 'C')
		/* get current parameter */
    		sprintf(message, "%c%s%s%c",
			m->stx,
			ocp,
			oc,
			m->etx
		);
	else if (m->message_type == 'E')
		/* set parameter message */
    		sprintf(message, "%c%s%s%s%c",
			m->stx,
			ocp,
			oc,
			m->value,
			m->etx
		);

	if (getDebug() > 0) {
		printf("   * Message    = [");
		print(m->message_length, message);
		printf("]\n");
	}

	/* count check code */
	for (i=1; i<7; i++)
		bcc ^= head[i];
	for (i=0; i<m->message_length; i++)
		bcc ^= message[i];

	if (getDebug() > 0) {
		printf("   * Check code = [%#x]\n", bcc);
		printf("   * Delimiter  = [%#x]\n", m->cr);
	}

	m->msg = calloc(19, sizeof(char));
	sprintf(m->msg, "%s%s%c%c", head, message, bcc, m->cr);

	m->replay = NULL;
	send_message(m->msg, &m->replay);

	if (getDebug() > 0 && m->replay == NULL)
		perror("E: No replay!");
	else if (getDebug() > 1 && m->replay != NULL) {
		REPLAY r;

		r.initReplay = initReplay;
		r.initReplay(&r, &m->replay);

		r.printReplay = printReplay;
		r.printReplay(&r);
	}

	if (m->replay == NULL || strcmp(substr(m->replay, 8, 2), "00") != 0)
		error = -1;

	if (getDebug() > 0 ) puts("<   <<  <<< <<<<");
	return error;
}


void initReplay(REPLAY *r, char **replay_msg) {
	if (getDebug() > 0) printf("I: Initialisation replay: ");

	if (strlen(*replay_msg) != 27)
		perror("\nE: Bad message length!");

	/* head */
	r->soh			= (*replay_msg)[0];
	r->reserve		= (*replay_msg)[1];
	r->source		= (*replay_msg)[2];
	r->destination		= (*replay_msg)[3];
	r->message_type		= (*replay_msg)[4];
	r->message_length	= calloc(2, sizeof(char));
	r->message_length	= substr(*replay_msg, 5, 2);

	/* message */
	r->stx			= (*replay_msg)[7];
	r->result_code		= calloc(2, sizeof(char));
	r->result_code		= substr(*replay_msg, 8, 2);
	r->op_code_page		= calloc(2, sizeof(char));
	r->op_code_page		= substr(*replay_msg, 10, 2);
	r->op_code		= calloc(2, sizeof(char));
	r->op_code		= substr(*replay_msg, 12, 2);
	r->op_type		= calloc(2, sizeof(char));
	r->op_type		= substr(*replay_msg, 14, 2);
	r->max_value		= calloc(4, sizeof(char));
	r->max_value		= substr(*replay_msg, 16, 4);
	r->value		= calloc(4, sizeof(char));
	r->value		= substr(*replay_msg, 20, 4);
	r->etx			= (*replay_msg)[24];

	/* check_code */
	r->bcc			= (*replay_msg)[25];

	/* delimiter */
	r->cr			= (*replay_msg)[26];

	/* full message */
	r->msg			= *replay_msg;

	if (getDebug() > 0) puts("OK");
}


void printReplay(REPLAY *r) {
	int i;
	char *error = "no error";

	puts("I: The replay:");

	printf("   * Head       = [");
	for (i=0; i<6; i++)
		printf("%#x ", r->msg[i]);
	printf("%#x]\n", r->msg[6]);
	printf("       # source      = [%#x]\n", r->source);
	printf("       # destination = [%#x]\n", r->destination);
	printf("       # msg type    = [%#x]\n", r->message_type);
	printf("       # msg length  = [%#x %#x] = %d\n", r->message_length[0], r->message_length[1], strhex2int(r->message_length));

	printf("   * Message    = [");
	for (i=7; i<24; i++)
		printf("%#x ", r->msg[i]);
	printf("%#x]\n", r->msg[24]);
	if (strcmp(r->result_code, "00") != 0) {
	    error = "error!!!";
	}
	printf("       # result code = [%#x %#x] = %s\n", r->result_code[0], r->result_code[1], error);
	printf("       # op code pg  = [%#x %#x]\n", r->op_code_page[0], r->op_code_page[1]);
	printf("       # op code     = [%#x %#x]\n", r->op_code[0], r->op_code[1]);
	printf("       # op type     = [%#x %#x]\n", r->op_type[0], r->op_type[1]);
	printf("       # max value   = [");
	for (i=0; i<3; i++)
		printf("%#x ", r->max_value[i]);
	printf("%#x] = %d\n", r->max_value[3], strhex2int(r->max_value));
	printf("       # value       = [");
	for (i=0; i<3; i++)
		printf("%#x ", r->value[i]);
	printf("%#x] = %d\n", r->value[3], strhex2int(r->value));


	printf("   * Check code = [%#x]\n", r->bcc);
	printf("   * Delimiter  = [%#x]\n", r->cr);
}
