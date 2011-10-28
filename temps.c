#include "temps.h"
#include "common.c"
#include "stdio.h"

int main(int argc, char *argv[]) {

  int i;

  /*What do I need these for? Goddamn nothing*/
  if(argc!=0){
    argv[argc-1] = "";
  }

  init_sheet(10,5);
  for(i=0; i<10; i++){
    step_sheet();
    print_sheet();
    step_sheet();
    print_sheet();
  }
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
int terminate_sheet_check(int x_val, int y_val){
  /* If x or y is out of bounds return false */
  if(x_val < x && y_val < y){
    return 0;
  }
  else{
    /* If the curr value is equal to the present value return false */
    if(sheet[x_val][y_val]==prev_sheet[x_val][y_val]){
      return 0;
    }    
    /* If the delta value is less than the terminate value, return true */
    else if((sheet[x_val][y_val]-prev_sheet[x_val][y_val]) < delta_terminate){
      return 1;
    }
    else{
      /*Else return false*/
      return 0;
    }
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
    sheet[heat_sources[i][0]][heat_sources[i][1]] = heat_sources[i][2];
  }

  return 0;
}

/* INIT_SHEET(X,Y)
   Initializes the sheet pointer
   Creates a matrix of size (x+2,y+2)
*/
void* init_sheet(int x_val, int y_val){
  int i, j;

  /* Initialize constant heat sources */
  num_sources = 1;
  heat_sources = xmalloc(num_sources * sizeof(int*));
  heat_sources[0] = xmalloc(3 * sizeof(int));
  heat_sources[0][0] = 2;
  heat_sources[0][1] = 2;
  heat_sources[0][2] = 5;
  
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
