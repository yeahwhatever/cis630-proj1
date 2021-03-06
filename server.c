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
#include <mpi.h>

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
#define DEBUG 1
#define BUFSIZE 512

int main(int argc, char *argv[]) {
	int status, socketfd, yes=1;
	struct addrinfo hints, *servinfo, *p;
	char *host, *port;
	int numprocs, rank;

	if (argc != 3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		return 1;
	} else {
		host = argv[1];
		port = argv[2];
	}


	MPI_Init(&argc, &argv);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Rank: %d, Numprocs: %d\n",rank, numprocs);

	if(rank == 0) {
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

	}
	status = listen_loop(socketfd);

	MPI_Finalize();
	return status;
}

int listen_loop(int socketfd) {
	socklen_t sin_size;
	struct sockaddr_storage client_addr;
	struct sheet *s;
	struct heatpoint *hps;
	int client_fd, num_bytes, x_v, y_v, width, height, num_heat;
	int i, rank, numprocs;
	float ff;
	char buf[BUFSIZE], final_val[100];

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	if(rank == 0) {
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

/*		for(i = 1; i < numprocs; i++) {
			MPI_Send(&num_bytes, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&buf, num_bytes, MPI_CHAR, i, 1, MPI_COMM_WORLD);

		}
	} else {
		MPI_Recv(&num_bytes, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
		printf("Rank: %d received num_bytes: %d\n", rank, num_bytes);
		MPI_Recv(&buf, num_bytes, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &Stat); 
		printf("Rank: %d received buf: \n%s\n", rank, buf);
*/

		/** 
		 * Parse the json query into height, width, and number of heat sources
		 */
		hps = parseJson(buf, &width, &height, &num_heat);

#if DEBUG > 0
		for(i = 0; i < num_heat; i++) {
			printf("x: %d, y: %d, t: %f\n",hps[i].x, hps[i].y, hps[i].t);

		}
#endif 
		/**
		 * Init our sheet from the values we parse, and free those, as we don't
		 * need them anymore
		 */

		s = init_sheet(width, height, hps, num_heat);
		free(hps);


		/**
		 *
		 * recv the first query and parse it
		 */
		if((num_bytes = recv(client_fd, buf, BUFSIZE-1, 0)) == -1)
			perror("recv");
		buf[num_bytes] = '\0';

		parseJsonQuery(buf, &x_v, &y_v);
		printf("Received query (%d, %d)\n", x_v, y_v);
	}
	if(rank == 0) {



		/**
		 * Run the sheet with the specified values, and return the results
		 *
		 */
		ff = run_sheet(s, x_v, y_v);
	} else {

		slave_compute();

	}
	if(rank == 0) {
		sprintf(final_val, "%.6f", ff);

		if(send(client_fd, final_val, strlen(final_val), 0) == -1)
			perror("send");	

		/**
		 * Loop forever returning points that the client queries
		 */
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

	}
	/**
	 * Free the memory, close the client_fd, and start the loop over again
	 * waiting for another connection
	 */
	free_sheet(s);
	close(client_fd);

	return listen_loop(socketfd);
}

/**
 * Run the sheet sheet, from the initial state and return the final temp
 * for the x,y point specified. If we hit the error condition, we finish
 * by setting the sheet to it's final state, and return the float
 */
float run_sheet(struct sheet *s, int x, int y) {
	int i;
	float final_val;

	if(!s->checked) {
		for (i = 0; i < 5000 && !(s->checked); i++) {
			step_sheet(s);
		}

		s->checked = 1;
		printf("Finished after %d iterations\n",i); 
	}

	final_val = query_sheet(s,x,y);
	printf("Target (%d,%d) terminated with value %f\n",x,y,final_val);
	return final_val;
}



void slave_compute() {
	int len;
	MPI_Status Stat;


	MPI_Recv(&len, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);



}
/* STEP_SHEET()
   Moves the sheet one step forward in time
*/
void step_sheet(struct sheet *s){
  int i, j, k, rank, numprocs, row_spot, col_spot, which_proc;
  float delta = 0, big_delta = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);




	for(i = 1; i < numprocs; i++ ) {
		MPI_Send(&s->x, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
	}
	/* Move curr sheet to prev sheet */
	for(i=0; i < s->x; i++){
		for(j=0; j < s->y; j++){
			s->prev_sheet[i][j] = s->sheet[i][j];
		}
	}

	if(numprocs > 0) {
		float *full_row = xmalloc(sizeof(int) * s->x * 3);
		which_proc = 0;
		//Going through every row in the table
		for(i = 0; i < s->y-2; i++) {
			//For iterating through the 3 rows in a row. So, it should first be
			//row 0, 1, 2, concatenated together.
			for(j = 0; j < 3; j++) {
				for(k = 0; k < s->x; k++) {
					row_spot = j * s->x + k;
					col_spot = j + i;
					full_row[row_spot] = s->prev_sheet[k][col_spot];
				}

			}
			which_proc++;
			if(which_proc >= numprocs) {
				which_proc = 1;
			}
			
			MPI_Send(&full_row, 3*s->x, MPI_FLOAT, which_proc, 1, MPI_COMM_WORLD);


		}
	} else {
		for(i=1; i < (s->x-1); i++){
			for(j=1; j < (s->y-1); j++){
				s->sheet[i][j] = (s->prev_sheet[i][j]
						+ s->prev_sheet[i-1][j]
						+ s->prev_sheet[i][j-1]
						+ s->prev_sheet[i+1][j]
						+ s->prev_sheet[i][j+1]) / 5.0;

			}
		}
	}
	/**
	 * We need to reset before we look for the largest change...
	 */
	reset_sheet(s);

	/**
	 * Get the largest change in the sheet and test if its smaller than our
	 * terminate case, if it is we set our finished flag.
	 */
	for(i=1; i < (s->x-1); i++){
		for(j=1; j < (s->y-1); j++){
			delta = s->sheet[i][j] - s->prev_sheet[i][j];
			if (delta > big_delta)
				big_delta = delta;
		}
	}

	if (big_delta < DELTA_TERMINATE)
		s->checked = 1;

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

/* INIT_SHEET
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

/**
 * Clean up all the memory we allocate in init_sheet
 */
void free_sheet(struct sheet *s) {
	int i;

	free(s->hps);
	for (i = 0; i< s->x; i++) {
		free(s->sheet[i]);
		free(s->prev_sheet[i]);
	}

	free(s);
}
