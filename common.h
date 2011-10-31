#ifndef COMMON_H
#define COMMON_H

struct heatpoint {
	int x;
	int y;
	float t;
};

void* xmalloc(unsigned int size);

char* mkJson(int x, int y, int num_heat, struct heatpoint *hps );
char* mkJsonQuery(int x, int y);
#endif
