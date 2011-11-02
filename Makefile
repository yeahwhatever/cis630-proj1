CC = gcc

CFLAGS = -Wall -W -g 

# For solaris
#LL = -lsocket -lnsl
LL = 

all: client server

client: client.c 
		$(CC) client.c common.c $(LL) $(CFLAGS) -o client

server: client.c
		$(CC) server.c common.c $(LL) $(CFLAGS) -o server

temps: temps.c temps.h
		$(CC) temps.c $(LL) $(CFLAGS) -o temps

clean:
		rm -rf *.o client server
