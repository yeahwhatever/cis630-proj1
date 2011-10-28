/**
 * CIS 610 Project 1
 * Joe Pletcher, Adam Bates, Nick Tiller
 *
 * Some code from Beej's Networking guide was used
 */

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

#include "server.h"
#include "common.h"

#define BACKLOG 10
#define DEBUG 0

int main(int argc, char *argv[]) {
	int status, socketfd, yes=1;
	struct addrinfo hints, *servinfo, *p;
	char *host, *port;

	if (argc != 3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		return 1;
	} else {
		host = argv[1];
		port = argv[2];
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

    /* loop through all the results and bind to the first we can */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((socketfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(socketfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socketfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 3;
	}


	freeaddrinfo(servinfo);

	if (listen(socketfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	status = listen_loop(socketfd);

	return status;
}

int listen_loop(int socketfd) {
	socklen_t sin_size;
	struct sockaddr_storage client_addr;
	int client_fd;

	while (1) {
		sin_size = sizeof client_addr;
		client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &sin_size);

		printf("Connection.\n");

		if (client_fd == -1) {
			perror("accept");
			continue;
		}

		if (send(client_fd, "Hello, world!", 13, 0) == -1) {
			perror("send");
		}
		close(client_fd);
	}

	return 0;
}
