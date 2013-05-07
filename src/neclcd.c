/*
 * Name: lcd.c
 * Version: 0.1
 * Description: Main function for external control of the NEC MultiSync LCD4610
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "debug.h"
#include "message.h"
#include "numconv.h"


void help(void);


int main(int argc, char **argv) {
	int c, value = -1, ret = 0;
	char message_type = '\xFF', destination = 'A', source = '0', code_page = '\xFF', code = '\xFF', command = '\xFF';


	/* parse command line options */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"type",
				required_argument,
				NULL,
				't'
			},
			{"dest",
				required_argument,
				NULL,
				'd'
			},
			{"src",
				required_argument,
				NULL,
				's'
			},
			{"codepage",
				required_argument,
				NULL,
				'p'
			},
			{"code",
				required_argument,
				NULL,
				'c'
			},
			{"value",
				required_argument,
				NULL,
				'v'
			},
			{"command",
				required_argument,
				NULL,
				'm'
			},
			{"debug",
				optional_argument,
				NULL,
				'e'
			},
			{"help",
				no_argument,
				NULL,
				'h'
			}
		};

		c = getopt_long(argc, argv, "t:d:s:p:c:v:e::h", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 't':
				message_type = optarg[0];
				break;

			case 'd':
				destination = optarg[0];
				break;

			case 's':
				source = optarg[0];
				break;

			case 'p':
				code_page = strhex2int(optarg);
				break;

			case 'c':
				code = strhex2int(optarg);
				break;

			case 'v':
				value = strhex2int(optarg);
				break;

			case 'm':
				command = strhex2int(optarg);
				break;

			case 'e':
				if (optarg == NULL)
					setDebug(1);
				else
					setDebug(strhex2int(optarg));
				break;

			case 'h':
				help();
				break;

			default:
				fprintf(stderr, "E: getopt returned character code 0%o\n", c);
				exit(-1);
				break;
		}
	}


	if (optind == 1) {
		help();
	}


	if (optind < argc) {
		fprintf(stderr, "W: Non-option ARGV-elements: ");
		while (optind < argc)
			fprintf(stderr, "'%s' ", argv[optind++]);
		fprintf(stderr, "\n");
	}


	/* check message type */
	if (message_type != 'A' && message_type != 'C' && message_type != 'E') {
		fprintf(stderr, "E: Wrong message type!\n");
		help();
	}

	/* check parameters */
	if ((message_type == 'A' && (command == '\xFF')) ||
	    (message_type == 'C' && (code_page == '\xFF' || code == '\xFF')) ||
	    (message_type == 'E' && (code_page == '\xFF' || code == '\xFF' || value == -1))
	) {
		fprintf(stderr, "E: Too few parameters!\n");
		help();
	}


	MESSAGE m;
	REPLAY r;

	switch (message_type) {
		case 'A':
			/*
			* COMMAND
			*/
			/* inicialisation */
			m.initMessage	= initMessage;
			m.initMessage(&m, message_type, 4);

			/* set header parameters */
			m.destination	= destination;
			m.source	= source;

			/* set message parameters */
			m.command	= command;

			/* set LCD parameters */
			m.sendMessage = sendMessage;
			if ((ret = m.sendMessage(&m)) != 0)
				perror("E: Send message failed!");

			break;
		case 'C':
			/*
			* GET CURRENT PARAMETER
			*/
			/* inicialisation */
			m.initMessage	= initMessage;
			m.initMessage(&m, message_type, 6);

			/* set header parameters */
			m.destination	= destination;
			m.source	= source;

			/* set message parameters */
			m.op_code_page	= code_page;
			m.op_code	= code;

			/* set LCD parameters */
			m.sendMessage = sendMessage;
			if ((ret = m.sendMessage(&m)) != 0)
				perror("E: Send message failed!");
			else {
				r.initReplay = initReplay;
				r.initReplay(&r, &m.replay);

				printf("max_value = %d\n", strhex2int(r.max_value));
				printf("current_value = %d\n", strhex2int(r.value));
			}

			break;

		case 'E':
			/*
			* SET PARAMETER
			*/
			/* inicialisation */
			m.initMessage	= initMessage;
			m.initMessage(&m, message_type, 10);

			/* set header parameters */
			m.destination	= destination;
			m.source	= source;

			/* set message parameters */
			m.op_code_page	= code_page;
			m.op_code	= code;
			m.value	= int2strhex(value, 4);

			/* set LCD parameters */
			m.sendMessage = sendMessage;
			if ((ret = m.sendMessage(&m)) != 0)
				perror("E: Send message failed!");

			break;
	}


	return ret;
}


void help(void) {
	puts("Usage: neclcd [OPTIONS]\n");
	puts("OPTIONS:");
	puts("  -t, --type=VAL         message type");
	puts("                         ('A' = command [not implemented properly]");
	puts("                          'C' = get current param");
	puts("                          'E' = set param)");
	puts("  -d, --dest=VAL         destination address (default is 'A')");
	puts("  -s, --src=VAL          source equipment ID (default is '0')");
	puts("  -p, --codepage=VAL     OP code page");
	puts("  -c, --code=VAL         OP code");
	puts("  -v, --value=VAL        value");
	puts("  -m, --command=VAL      command");
	puts("  -e, --debug            debug mode\n");
	puts("  -h, --help             display this help and exit\n");
	puts("Examples:");
	puts("neclcd --dest=A --type=A --command=0C");
	puts("neclcd --dest=A --type=C --codepage=00 --code=60");
	puts("neclcd --dest=A --type=E --codepage=00 --code=60 --value=12\n");
	puts("Report bugs to <jiri(dot)tyr(at)gmail(dot)com>.");

	exit(-1);
}
