CC = mpicc


CFLAGS = -Wall -W -g

# For solaris
#LL = -lsocket -lnsl
LL = 

all: client server

client: client.c  client.h common.c common.h
		$(CC) client.c common.c $(LL) $(CFLAGS) -o client

server: server.c server.h common.c common.h
		$(CC) server.c common.c $(LL) $(CFLAGS) -o server

clean:
		rm -rf *.o *.core client server
