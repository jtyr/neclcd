/*
 * Name: message.h
 * Description: Header file for message.c
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#define SOH '\x01'
#define RES '0'
#define STX '\x02'
#define ETX '\x03'
#define CR  '\x0D'


typedef struct message {
	/* head */
	char soh;
	char reserve;
	char source;
	char destination;
	char message_type;
	int  message_length;

	/* message */
	char stx;
	char op_code_page;
	char op_code;
	char *value;
	char command;
	char etx;

	/* check code */
	char bcc;

	/* delimiter */
	char cr;

	/* message to send */
	char *msg;

	/* replay message */
	char *replay;

	void (*initMessage)(struct message *, char, int);
	int (*sendMessage)(struct message *);
} MESSAGE;

extern void initMessage(MESSAGE *m, char message_type, int message_length);
extern int sendMessage(MESSAGE *m);


typedef struct replay {
	/* head */
	char soh;
	char reserve;
	char source;
	char destination;
	char message_type;
	char *message_length;

	/* message */
	char stx;
	char *result_code;
	char *op_code_page;
	char *op_code;
	char *op_type;
	char *max_value;
	char *value;
	char etx;

	/* check code */
	char bcc;

	/* delimiter */
	char cr;

	/* full message */
	char *msg;

	void (*initReplay)(struct replay *, char **);
	void (*printReplay)(struct replay *);
} REPLAY;

extern void initReplay(REPLAY *r, char **replay_msg);
extern void printReplay(REPLAY *r);
