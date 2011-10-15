CC = gcc

CFLAGS = -Wall -W -g -ansi -pedantic -Werror

# For solaris
#LL = -lsocket -lnsl
LL = 

all: client server

client: client.c 
		$(CC) client.c $(LL) $(CFLAGS) -o client

server: client.c
		$(CC) server.c $(LL) $(CFLAGS) -o server

clean:
		rm -rf *.o client server
