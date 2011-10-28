#ifndef COMMON_H
#define COMMON_H

struct heatpoint {
	int x;
	int y;
	float t;
};

void* xmalloc(unsigned int size);

#endif
