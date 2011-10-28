#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "client.h"
#include "common.c"

int main(int argc, char *argv[]) {
	char *host, *port;
	char buf[100];
	int socketfd, status, numbytes;
	struct addrinfo hints, *servinfo, *p;


	if (argc != 3) {
		printf("Usage: %s <server> <port>\n", argv[0]);
		return 1;
	} else {
		host = argv[1];
		port = argv[2];
	}


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socketfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("client: call to socket failed");
			continue;
		}

		if (connect(socketfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socketfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to make socket\n");
		return 3;
	}

    freeaddrinfo(servinfo); 


    if ((numbytes = recv(socketfd, buf, 99, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    close(socketfd);

	return 0;
}
