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
#define BUFSIZE 512

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

float run_sheet(struct sheet *s, int x, int y) {

	if(!s->checked) {
		int i;
		for (i = 0; i < 5000 && terminate_sheet_check(s, x, y); i++) {
			step_sheet(s);
		}

		s->checked = 1;
		printf("Finished after %d iterations\n",i); 
	}

	float final_val = query_sheet(s,x,y);
	printf("Target (%d,%d) terminated with value %f\n",x,y,final_val);
	return final_val;

}
char* create_test() {

  struct heatpoint heatpoints[5];

  heatpoints[0].x = 100;
  heatpoints[0].y = 100;
  heatpoints[0].t = 33.0;
  heatpoints[1].x = 100;
  heatpoints[1].y = 700;
  heatpoints[1].t = 13.0;
  heatpoints[2].x = 375;
  heatpoints[2].y = 375;
  heatpoints[2].t = 20.0;
  heatpoints[3].x = 750;
  heatpoints[3].y = 700;
  heatpoints[3].t = 13.0;
  heatpoints[4].x = 350;
  heatpoints[4].y = 700;
  heatpoints[4].t = 23.0;

  return mkJson(1000,1000, 5, heatpoints);
}
void test_temperature(){
  struct heatpoint heatpoints[5];
  int i;
  struct sheet *s;

  heatpoints[0].x = 100;
  heatpoints[0].y = 100;
  heatpoints[0].t = 33.0;
  heatpoints[1].x = 100;
  heatpoints[1].y = 700;
  heatpoints[1].t = 13.0;
  heatpoints[2].x = 375;
  heatpoints[2].y = 375;
  heatpoints[2].t = 20.0;
  heatpoints[3].x = 750;
  heatpoints[3].y = 700;
  heatpoints[3].t = 13.0;
  heatpoints[4].x = 350;
  heatpoints[4].y = 700;
  heatpoints[4].t = 23.0;

  s = init_sheet(1000,1000,heatpoints,5);

  printf("%d %d %d %d\n", 400, s->x, 400, s->y);

  for (i = 0; i < 5000 && terminate_sheet_check(s, 400, 400); i++) {
      step_sheet(s);
  }

  s->checked = 1;
  printf("Finished after %d iterations\nFinal state is:\n",i);
  printf("Target (%d,%d) terminated with value %f\n",400,400,query_sheet(s,400,400));

  free_sheet(s);
}

int listen_loop(int socketfd) {
	socklen_t sin_size;
	struct sockaddr_storage client_addr;
	int client_fd, num_bytes;
	int x_v, y_v;
	char buf[BUFSIZE];

	sin_size = sizeof client_addr;
	client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &sin_size);

	if (client_fd == -1)
		perror("accept");

	if ((num_bytes = recv(client_fd, buf, BUFSIZE-1, 0)) == -1)
		perror("recv");
	buf[num_bytes] = '\0';

	if (strcmp(buf, "hey")) {
		fprintf(stderr, "Didn't get a 'hey'");
		close(client_fd);
		return 5;
	}

	if (send(client_fd, "whatsup", 7, 0) == -1) 
		perror("send");

	if((num_bytes = recv(client_fd, buf, BUFSIZE-1, 0)) == -1)
		perror("recv");
	buf[num_bytes] = '\0';
	printf("Received:\n%s\n", buf);

	char final_val[100];
	int width, height, num_heat;
	struct heatpoint *hps = parseJson(buf, &width, &height, &num_heat);
	int i;
	for(i = 0; i < num_heat; i++) {
		printf("x: %d, y: %d, t: %f\n",hps[i].x, hps[i].y, hps[i].t);

	}
	struct sheet *s = init_sheet(width, height, hps, num_heat);
	free(hps);

	if((num_bytes = recv(client_fd, buf, BUFSIZE-1, 0)) == -1)
		perror("recv");
	buf[num_bytes] = '\0';

	printf("Received query.\n");
	parseJsonQuery(buf, &x_v, &y_v);
	float ff = run_sheet(s, x_v, y_v);
	sprintf(final_val, "%.6f", ff);
	if(send(client_fd, final_val, strlen(final_val), 0) == -1)
		perror("send");	

	while (1) {
		if((num_bytes = recv(client_fd, buf, BUFSIZE-1, 0)) == -1)
			perror("recv");
		buf[num_bytes] = '\0';

		parseJsonQuery(buf, &x_v, &y_v);
		
		if(x_v == 0 && y_v == 0) break;

		sprintf(final_val, "%.6f", query_sheet(s, x_v, y_v));
		if(send(client_fd, final_val, strlen(final_val), 0) == -1)
			perror("send");	

	}

	free_sheet(s);
	close(client_fd);
	return 0;
}


/* STEP_SHEET(X,Y)
   Moves the sheet one step forward in time
*/
void step_sheet(struct sheet *s){
  int i,j;
  /* Move curr sheet to prev sheet */
  for(i=0; i < s->x; i++){
    for(j=0; j < s->y; j++){
      s->prev_sheet[i][j] = s->sheet[i][j];
    }
  }

  for(i=1; i < (s->x-1); i++){
    for(j=1; j < (s->y-1); j++){
      s->sheet[i][j] = (s->prev_sheet[i][j]
		     + s->prev_sheet[i-1][j]
		     + s->prev_sheet[i][j-1]
		     + s->prev_sheet[i+1][j]
		     + s->prev_sheet[i][j+1]) / 5;
    }
  }

  reset_sheet(s);
}

/* TERMINATE_SHEET_CHECK(X,Y)
   Checks if it is time to terminate by checking the specified
   point to see if it's delta is less than .5.

   If it's delta is zero then it means that the point has not been
   touched yet and therefore we should not exit

   returns true if it is time to go, false otherwise
*/
int terminate_sheet_check(struct sheet *s, int t_x, int t_y){
  float delta;

  //For the example given, the result of 1.744C is not consistent with the described algorithm.
  //The change in temperature for target cell (400,400) is so small that it is always
  //under the cutoff temperature change point of .5 C.

  //Therefore we have removed our terminate sheet check for now and will always...
  return 1;

  delta = s->sheet[t_x+1][t_y+1] - s->prev_sheet[t_x+1][t_y+1];


  printf("Terminate check %f %f %f cutoff %f\n",s->sheet[t_x][t_y],
	 s->prev_sheet[t_x][t_y],delta,DELTA_TERMINATE);

  if(delta == 0)
    return 1;

  /* If the delta value is less than the terminate value, return true */
  else if(delta < DELTA_TERMINATE)
    return 0;
  else
    return 1;
}

/* STEP_SHEET(X,Y)
   Checks the current heat value of the sheet
   at the specified point
*/
float query_sheet(struct sheet *s, int x_val, int y_val){
  if(x_val+1 < s->x && y_val+1 < s->y){
    return s->sheet[x_val + 1][y_val + 1];
  }
  else{
    return -1;
  }
}

/* RESET_SHEET()
   Prepares sheet for the next iteration
   by zeroing out the edges
   and reseting the constant heat sources
*/
void reset_sheet(struct sheet *s){

  int i,j;
  /* Zero out the edges */
  j = s->y - 1;
  for(i = 0;i < s->x; i++){
    s->sheet[i][0] = 0;
    s->sheet[i][j] = 0;
  }
  i = s->x - 1;
  for(j = 0; j< s->y; j++){
    s->sheet[0][j] = 0;
    s->sheet[i][j] = 0;
  }

  /* Reset constant heat sources */
  for(i = 0; i < s->num_heat; i++){
  	s->sheet[s->hps[i].x+1][s->hps[i].y+1] = s->hps[i].t;
  }

}

/* INIT_SHEET(X,Y)
   Initializes the sheet pointer
   Creates a matrix of size (x+2,y+2)
*/
struct sheet* init_sheet(int x_val, int y_val, 
		 struct heatpoint * heatpoints, int num_heatpoints){
  int i, j;

  struct sheet *s = xmalloc(sizeof(struct sheet));

  s->num_heat = num_heatpoints;
  s->checked = 0;
  s->hps = xmalloc(num_heatpoints * sizeof(struct heatpoint));
  for(i = 0; i < num_heatpoints; i++){
    s->hps[i].x = heatpoints[i].x;
    s->hps[i].y = heatpoints[i].y;
    s->hps[i].t = heatpoints[i].t;
  }
  
  /* Initialize past and present sheets */
  s->x = x_val + 2;
  s->y = y_val + 2;

  s->sheet = xmalloc(x_val*sizeof(float *));
  s->prev_sheet = xmalloc(x_val*sizeof(float *));


  /*Initialize Values in past and present sheets*/
  for(i = 0; i< s->x; i++){
	s->sheet[i] = xmalloc(s->y*sizeof(float));
	s->prev_sheet[i] = xmalloc(s->y*sizeof(float));
    for(j = 0; j< s->y; j++){
      s->sheet[i][j] = 0;
      s->prev_sheet[i][j] = 0;
    }
  }

  /* Insert constant heat values */
  reset_sheet(s);

  return s;
}

/* PRINT_SHEET()
   prints the matrix for debugging purposes
*/
void print_sheet(struct sheet *s){

  int i,j;

  for(i=0; i < s->x; i++){
    for(j=0; j < s->y; j++){
      printf("%f ", s->sheet[i][j]);
    }
    printf("\n");
  }

  printf("\n");
}

void free_sheet(struct sheet *s) {
	int i;

	free(s->hps);
	for(i = 0; i< s->x; i++){
		free(s->sheet[i]);
		free(s->prev_sheet[i]);
	}

	free(s);
}
