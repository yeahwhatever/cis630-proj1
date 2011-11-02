CC = gcc

CFLAGS = -Wall -W -g 

# For solaris
#LL = -lsocket -lnsl
LL = 

all: client server temps test

client: client.c 
		$(CC) client.c common.c $(LL) $(CFLAGS) -o client

server: server.c
		$(CC) server.c common.c $(LL) $(CFLAGS) -o server

temps: temps.c 
		$(CC) temps.c $(LL) $(CFLAGS) -o temps

test: test.c
		$(CC) test.c common.c $(LL) $(CFLAGS) -o test


clean:
		rm -rf *.o *.core client server temps test
