/*
 * Name: port.h
 * Description: Header file for port.c
 * Programmer: Jiri Tyr
 * Last update: 17.02.2006
 */


#define PORT "/dev/ttyS0"
#define TIMEOUT 1


void read_port(int *fd, char **replay_msg);
int write_port(int *fd, char *s);
int open_port(void);
void setup_port(int *fd);
