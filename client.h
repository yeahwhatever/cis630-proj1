#ifndef CLIENT_H
#define CLIENT_H
#include "common.h"

int recv_loop(int socketfd);
void parse_two(char *s, int *x, int *y);
void parse_three(char *s, int *x, int *y, float *t);

#endif
