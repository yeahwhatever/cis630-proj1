#include "common.h"

#ifndef SERVER_H
#define SERVER_H

int listen_loop(int);

#define DELTA_TERMINATE 0.5
#define WORK 1
#define DIE 0
#define SIZE 1
#define RETURN 2


void test_temperature();
void step_sheet(struct sheet *s);
int terminate_sheet_check(struct sheet *s, int x, int y);
float run_sheet(struct sheet *s, int, int);
float query_sheet(struct sheet *s, int,int);
struct sheet* init_sheet(int,int,struct heatpoint*,int);
void reset_sheet( struct sheet *s);
void print_sheet(struct sheet *s);
void free_sheet(struct sheet *s);
void row_worker(); 
#endif
