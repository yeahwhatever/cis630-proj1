#ifndef TEMPS_H
#define TEMPS_H

#include "common.c"
#include "stdio.h"


/*******    CONFIGURATION VALUES **********/
/* Our termination delta value */
float delta_terminate = .5;
/*****************************************/


/* Our representation of the metal sheet */
float** sheet;
float** prev_sheet;

/*Matrix of constant heat sources
 [0] 
 [1] are ACTUAL x and y values of constant heat sources
     (i.e. +2 to x,y)
 [2] is the heat value
  */
struct heatpoint* heat_sources;
int num_sources = 0;

/* The ACTUAL size of the sheet matrix (i.e. +2 to x, y)*/
int x=0,y=0;

/* The termination points x and y values */
int t_x=0, t_y=0;

/* Our iteration of the sheet.  Will return at 5000 */
int iteration;

void* step_sheet();
int terminate_sheet_check();
float query_sheet(int,int);
void* init_sheet(int,int,struct heatpoint*,int,int,int);
void* reset_sheet();
void* print_sheet();

#endif
