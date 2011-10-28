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

#define LINE 100
#define DEBUG 2

int main(int argc, char *argv[]) {
	char *host, *port;
	int socketfd, status;
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

	if (send(socketfd, "hey", 3, 0) == -1)
		perror("send");

	status = recv_loop(socketfd);

    close(socketfd);

	return status;
}

int recv_loop(int socketfd) {
	char buf[LINE];
	char tmp[LINE];
	int i, j, numbytes;

	int x, y, num_heat;
	struct heatpoint *hps;

	while (1) {
		if ((numbytes = recv(socketfd, buf, 99, 0)) == -1) {
			perror("recv");
			return 4;
		}
		buf[numbytes] = '\0';
#if DEBUG > 0
		printf("server: '%s'\n",buf);
#endif

		if (!strcmp("whatsup", buf)) {
			printf("Enter 'X Y' dimension of surface: ");
			fgets(buf, LINE, stdin);
			
			for (i = 0; i < LINE; i++) {
				if (buf[i] != ' ')
					tmp[i] = buf[i];
				else
					break;
			}
			tmp[++i] = '\0';
			x = atoi(tmp);

			for (i = 0; i < LINE; i++) {
				if (buf[i] != ' ')
					tmp[i] = buf[i];
				else
					break;
			}
			tmp[++i] = '\0';
			y = atoi(tmp);

			if (x < 10 || x > 2000) {
				fprintf(stderr, "Width must be 10 <= w <= 20000");
				return 5;
			}
			if (y < 10 || y > 20000) {
				fprintf(stderr, "Height must be 10 <= h <= 20000");
				return 6;
			}

#if DEBUG > 0
			printf("X: %d Y: %d", x, y);
#endif

			printf("Enter number of heat sources: ");
			fgets(buf, LINE, stdin);
			num_heat = atoi(buf);

			if (num_heat < 4 || num_heat > 20) {
				fprintf(stderr, "Number of heat sources must be 4 <= num heat <= 20");
				return 7;
			}

			hps = xmalloc(num_heat * sizeof (struct heatpoint));

			for (i = 0; i < num_heat; i++) {
				fgets(buf, LINE, stdin);

				for (j = 0; j < LINE; j++) {
					if (buf[j] != ' ')
						tmp[j] = buf[j];
					else
						break;
				}
				tmp[++j] = '\0';
				hps[i].x = atoi(tmp);

				for (j = 0; j < LINE; j++) {
					if (buf[j] != ' ')
						tmp[j] = buf[j];
					else
						break;
				}
				tmp[++j] = '\0';
				hps[i].y = atoi(tmp);

				for (j = 0; j < LINE; j++) {
					if (buf[j] != ' ')
						tmp[j] = buf[j];
					else
						break;
				}
				tmp[++j] = '\0';
				hps[i].t = atof(tmp);
			}

#if DEBUG > 0
			for (i = 0; i < num_heat; i++)
				printf("x: %d y:%d t:%f", hps[i].x, hps[i].y, hps[i].t);
#endif
			return 22;

		} else {

		}

	}

}
