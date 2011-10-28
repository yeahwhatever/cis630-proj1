#include "temps.h"

int main(int argc, char *argv[]) {

  struct heatpoint heatpoints[2];

  /*What do I need these for? Goddamn nothing*/
  if(argc!=0){
    argv[argc-1] = "";
  }

  heatpoints[0].x = 3;
  heatpoints[0].y = 3;
  heatpoints[0].t = 100;
  heatpoints[1].x = 7;
  heatpoints[1].y = 7;
  heatpoints[1].t = 100;

  init_sheet(10,10,heatpoints,2,5,5);
  printf("%d %d %d %d\n", t_x, x, t_y, y);

  while((iteration < 5000)
	&& (terminate_sheet_check() != 1)){
      step_sheet();
  }

  printf("Finished after %d iterations\nFinal state is:\n",iteration);
  print_sheet();
  printf("Target (%d,%d) terminated with value %f\n",t_x,t_y,query_sheet(t_x,t_y));
  return 0;
}

/* STEP_SHEET(X,Y)
   Moves the sheet one step forward in time
*/
void* step_sheet(){

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

  return 0;
}

/* TERMINATE_SHEET_CHECK(X,Y)
   Checks if it is time to terminate by checking the specified
   point to see if it's delta is less than .5.

   If it's delta is zero then it means that the point has not been
   touched yet and therefore we should not exit

   returns true if it is time to go, false otherwise
*/
int terminate_sheet_check(){
  float delta;
  delta = sheet[t_x][t_y]-prev_sheet[t_x][t_y];
  /*
  printf("Terminate check %f %f %f \n",sheet[t_x][t_y],
	 prev_sheet[t_x][t_y],delta);
  */
  /* If the curr value is equal to the present value return false */
  if(delta == 0){
    return 0;
  }    
  /* If the delta value is less than the terminate value, return true */
  else if(delta < delta_terminate){
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
void* reset_sheet(){

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

  return 0;
}

/* INIT_SHEET(X,Y)
   Initializes the sheet pointer
   Creates a matrix of size (x+2,y+2)
*/
void* init_sheet(int x_val, int y_val, 
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

  return 0;
}

/* PRINT_SHEET()
   prints the matrix for debugging purposes
*/
void* print_sheet(){

  int i,j;

  for(i=0; i<x; i++){
    for(j=0; j<y; j++){
      printf("%f ",sheet[i][j]);
    }
    printf("\n");
  }

  printf("\n");

  return 0;
}
