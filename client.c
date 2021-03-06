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
#include "common.h"

#define LINE 100
#define DEBUG 0

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

	status = recv_loop(socketfd);

	close(socketfd);

	return status;
}

int recv_loop(int socketfd) {
	char buf[LINE], *json;
	int i, numbytes;

	int x, y, query_x, query_y, num_heat;
	struct heatpoint *hps;

	if (send(socketfd, "hey", 3, 0) == -1)
		perror("send");

	/**
	 * Big listen loop. Two main things we need to listen for here
	 * First, a 'whatsup' will indicate we should send back an initial
	 * sheet state. On the other hand, the second case indicates that we
	 * got results from the server, and we should display them and ask for
	 * a new point.
	 */
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
			printf("Enter 'X Y' dimension of surface: \n");
			fgets(buf, LINE, stdin);

			parse_two(buf, &x, &y);

			if (x < 10 || x > 2000) {
				fprintf(stderr, "Width must be 10 <= w <= 20000\n");
				return 5;
			}
			if (y < 10 || y > 20000) {
				fprintf(stderr, "Height must be 10 <= h <= 20000\n");
				return 6;
			}

#if DEBUG > 0
			printf("X: %d Y: %d\n", x, y);
#endif

			printf("Enter number of heat sources: \n");
			fgets(buf, LINE, stdin);
			num_heat = atoi(buf);

			if (num_heat < 4 || num_heat > 20) {
				fprintf(stderr, "Number of heat sources must be 4 <= num heat <= 20\n");
				return 7;
			}

			hps = xmalloc(num_heat * sizeof (struct heatpoint));

			for (i = 0; i < num_heat; i++) {
				fgets(buf, LINE, stdin);

				parse_three(buf, &(hps[i].x), &(hps[i].y), &(hps[i].t));
#if DEBUG > 0
				printf("HP X: %d HP Y: %d HP T: %f\n", hps[i].x, hps[i].y, hps[i].t);
#endif

				if (hps[i].x < 0 || hps[i].x > x) {
					fprintf(stderr, "Heat point x coordinate is not on grid\n");
					return 8;
				}

				if (hps[i].y < 0 || hps[i].y > y) {
					fprintf(stderr, "Heat point y coordinate is not on grid\n");
					return 9;
				}

				if (hps[i].t < -100 || hps[i].t > 1500) {
					fprintf(stderr, "Heat point temperature must be -100 <= t <= 500\n");
					return 10;
				}
			}

			json = mkJson(x,y,num_heat, hps);
#if DEBUG > 0
			for (i = 0; i < num_heat; i++)
				printf("x: %d y:%d t:%f\n", hps[i].x, hps[i].y, hps[i].t);
			printf("Sending to server.\n");
#endif
			free(hps);
			if (send(socketfd, json, strlen(json), 0) == -1)
				perror("send");

			printf("Point ('X Y') to query?\n");
			fgets(buf, LINE, stdin);

			parse_two(buf, &query_x, &query_y);
#if DEBUG > 0
			printf("Query point: %d, %d\n", query_x, query_y);
#endif

			json = mkJsonQuery(query_x, query_y);

			if (send(socketfd, json, strlen(json), 0) == -1)
				perror("send");
#if DEBUG > 0
			printf("Awaiting server response.\n");
#endif
			free(json);

			if (query_x == 0 && query_y == 0) 
				break;

		} else {
			/**
			 * This is the branch where we've recieved a result from the
			 * server. Continue to query points until we give 0,0 in which
			 * we send the close to the server, and clean up
			 */
			printf("Result: %s\n", buf);
			printf("Point ('X Y') to query?\n");
			fgets(buf, LINE, stdin);

			parse_two(buf, &query_x, &query_y);
#if DEBUG > 0
			printf("Query point: %d, %d\n", query_x, query_y);
#endif
			json = mkJsonQuery(query_x, query_y);

			if (send(socketfd, json, strlen(json), 0) == -1)
				perror("send");
			
			free(json);

			if(query_x == 0 && query_y == 0) 
				break;
		}

	}
	return 0;

}

/**
 * Parse two integers, seperated by spaces from the buffer
 */
void parse_two(char *s, int *x, int *y) {
	int j, i = 0;
	char tmp[LINE];

	for (j = 0; i < LINE; i++) {
		if (s[i] != ' ')
			tmp[j] = s[i];
		else
			break;
		j++;
	}
	tmp[j] = '\0';
	i++;
	*x = atoi(tmp);

	for (j = 0; i < LINE; i++) {
		if (s[i] != '\n')
			tmp[j] = s[i];
		else
			break;
		j++;
	}
	tmp[j] = '\0';
	*y = atoi(tmp);
}

/**
 * Parse two integers and a float, spearated by spaces from the buffer
 */
void parse_three(char *s, int *x, int *y, float *t) {
	int i, j;
	char tmp[LINE];

	i = 0;
	for (j = 0; i < LINE; i++) {
		if (s[i] != ' ')
			tmp[j] = s[i];
		else
			break;
		j++;
	}
	tmp[j] = '\0';
	i++;
	*x = atoi(tmp);

	for (j = 0; i < LINE; i++) {

		if (s[i] != ' ')
			tmp[j] = s[i];
		else
			break;
		j++;
	}
	tmp[j] = '\0';
	i++;
	*y = atoi(tmp);

	for (j = 0; i < LINE; i++) {
		if (s[i] != '\n')
			tmp[j] = s[i];
		else
			break;
		j++;
	}
	tmp[j] = '\0';
	i++;
	*t = atof(tmp);
}

