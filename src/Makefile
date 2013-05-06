#####
#
# Name: Makefile
# Description: Makefile for creation of the executable neclcd file
# Programmer: Jiri Tyr
# Last update: 6.5.2009
#
#####


CC = gcc
CFLAGS = -Wall -O3
RM_F = rm -f

.PHONY: build clean


build: neclcd

neclcd: debug.o message.o port.o numconv.o string.o neclcd.o
	$(CC) $(CFLAGS) debug.o message.o port.o numconv.o string.o neclcd.o -o neclcd
	rm -f *.o

debug.o: debug.c
	$(CC) $(CFLAGS) debug.c -c

message.o: message.c
	$(CC) $(CFLAGS) message.c -c

port.o: port.c
	$(CC) $(CFLAGS) port.c -c

numconv.o: numconv.c
	$(CC) $(CFLAGS) numconv.c -c

string.o: string.c
	$(CC) $(CFLAGS) string.c -c

neclcd.o: neclcd.c
	$(CC) $(CFLAGS) neclcd.c -c


clean:
	$(RM_F) neclcd *.o
