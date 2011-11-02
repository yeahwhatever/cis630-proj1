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

void test_temperature();

int main(int argc, char *argv[]) {
	int status, socketfd, yes=1;
	struct addrinfo hints, *servinfo, *p;
	char *host, *port;

	test_temperature();

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

void test_temperature(){

  struct heatpoint heatpoints[5];
  num_sources = 5;

  x=1002;
  y=1002;

  t_x = 401;
  t_y = 401;

  heatpoints[0].x = 101;
  heatpoints[0].y = 101;
  heatpoints[0].t = 33;

  heatpoints[1].x = 101;
  heatpoints[1].y = 701;
  heatpoints[1].t = 13;

  heatpoints[2].x = 376;
  heatpoints[2].y = 376;
  heatpoints[2].t = 20;

  heatpoints[3].x = 751;
  heatpoints[3].y = 701;
  heatpoints[3].t = 13;

  heatpoints[4].x = 351;
  heatpoints[4].y = 701;
  heatpoints[4].t = 23;

  /*
  struct heatpoint heatpoints[2];
  num_sources = 2;

  x=10;
  y=10;

  t_x = 5;
  t_y = 5;

  heatpoints[0].x = 3;
  heatpoints[0].y = 3;
  heatpoints[0].t = 33;

  heatpoints[1].x = 7;
  heatpoints[1].y = 7;
  heatpoints[1].t = 13;
  */
  init_sheet(x,y,heatpoints,num_sources,t_x,t_y);

  while((iteration < 5000)
    && (terminate_sheet_check() != 1)){
      step_sheet();
  }

  printf("Finished after %d iterations\nFinal state is:\n",iteration);
  printf("Target (%d,%d) terminated with value %f\n",t_x,t_y,query_sheet(t_x,t_y));

}

int listen_loop(int socketfd) {
	socklen_t sin_size;
	struct sockaddr_storage client_addr;
	int client_fd, num_bytes;

	char buf[100];

	while (1) {
		sin_size = sizeof client_addr;
		client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &sin_size);

		if (client_fd == -1) {
			perror("accept");
			continue;
		}

		if ((num_bytes = recv(client_fd, buf, 99, 0)) == -1)
			perror("recv");
		buf[num_bytes] = '\0';

		if (strcmp(buf, "hey")) {
			fprintf(stderr, "Didn't get a 'hey'");
			close(client_fd);
			continue;
		}

		if (send(client_fd, "whatsup", 7, 0) == -1) 
			perror("send");

		close(client_fd);
	}

	return 0;
}


/* STEP_SHEET(X,Y)
   Moves the sheet one step forward in time
*/
void step_sheet(){

  int i,j;
  /* Move curr sheet to prev sheet */
  for(i=0; i<x; i++){
    for(j=0; j<y; j++){
      prev_sheet[i][j] = sheet[i][j];
    }
  }

  for(i=1; i<(x-1); i++){
    for(j=1; j<(y-1); j++){
      sheet[i][j] = (prev_sheet[i][j]
		     + prev_sheet[i-1][j]
		     + prev_sheet[i][j-1]
		     + prev_sheet[i+1][j]
		     + prev_sheet[i][j+1]) / 5;
    }
  }

  reset_sheet();
  iteration++;

}

/* TERMINATE_SHEET_CHECK(X,Y)
   Checks if it is time to terminate by checking the specified
   point to see if it's delta is less than .5.

   If it's delta is zero then it means that the point has not been
   touched yet and therefore we should not exit

   returns true if it is time to go, false otherwise
*/
int terminate_sheet_check(){

  //For the example given, the result of 1.744C is not consistent with the described algorithm.
  //The change in temperature for target cell (400,400) is so small that it is always
  //under the cutoff temperature change point of .5 C.

  //Therefore we have removed our terminate sheet check for now and will always...
  return 0;

  float delta;
  delta = sheet[t_x][t_y]-prev_sheet[t_x][t_y];


  printf("Terminate check %f %f %f cutoff %f\n",sheet[t_x][t_y],
	 prev_sheet[t_x][t_y],delta,delta_terminate);

  /* If the curr value is equal to the present value return false */
  if(delta == 0){
    return 0;
  }    

  /* If the delta value is less than the terminate value, return true */
  else if(delta < delta_terminate){
      printf("LEAVING!");
      return 1;      
  }
  else{
    /*Else return false*/
    return 0;
  }
}

/* STEP_SHEET(X,Y)
   Checks the current heat value of the sheet
   at the specified point
*/
float query_sheet(int x_val, int y_val){
  if(x_val < x && y_val < y){
    return sheet[x_val][y_val];
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
void reset_sheet(){

  int i,j;
  /* Zero out the edges */
  j = y - 1;
  for(i=0;i<x;i++){
    sheet[i][0] = 0;
    sheet[i][j] = 0;
  }
  i = x - 1;
  for(j=0;j<y;j++){
    sheet[0][j] = 0;
    sheet[i][j] = 0;
  }

  /* Reset constant heat sources */
  for(i=0; i<num_sources; i++){
    sheet[heat_sources[i].x][heat_sources[i].y] = heat_sources[i].t;
  }

}

/* INIT_SHEET(X,Y)
   Initializes the sheet pointer
   Creates a matrix of size (x+2,y+2)
*/
void init_sheet(int x_val, int y_val, 
		 struct heatpoint * heatpoints, int num_heatpoints,
		 int tx_val, int ty_val){
  int i, j;

  /* Initialize our target point */
  t_x = tx_val;
  t_y = ty_val;

  /* Initialize constant heat sources */
  num_sources = num_heatpoints;
  heat_sources = xmalloc(num_sources * sizeof(struct heatpoint));
  for(i=0; i<num_sources; i++){
    heat_sources[i] = heatpoints[i];
  }
  
  /* Initialize past and present sheets */
  x = x_val + 2;
  y = y_val + 2;
  iteration = 0;

  sheet = xmalloc(x*sizeof(float*));
  prev_sheet = xmalloc(x*sizeof(float*));

  for(i=0; i< x; i++){
    sheet[i] = xmalloc(y*sizeof(float*));
    prev_sheet[i] = xmalloc(y*sizeof(float*));
  }

  /*Initialize Values in past and present sheets*/
  for(i=0; i<x; i++){
    for(j=0; j<y; j++){
      sheet[i][j] = 0;
      prev_sheet[i][j] = 0;
    }
  }

  /* Insert constant heat values */
  reset_sheet();

}

/* PRINT_SHEET()
   prints the matrix for debugging purposes
*/
void print_sheet(){

  int i,j;

  for(i=0; i<x; i++){
    for(j=0; j<y; j++){
      printf("%f ",sheet[i][j]);
    }
    printf("\n");
  }

  printf("\n");

}

