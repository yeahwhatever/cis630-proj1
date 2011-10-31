/**
 * Common functions shared between client and server
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

/**
 * malloc wrapper to die quickly if we run out of memory
 */
void* xmalloc(unsigned int size) {
	void *ptr;

	ptr = malloc(size);

	if (ptr)
		return ptr;

	exit(255);
}

char* mkJson(int x, int y, int num_heat, struct heatpoint *hps ) {
	int i;
	char *tstr = xmalloc(512 * sizeof(char));
	char* str = xmalloc((num_heat+1) * 512 * sizeof(char));

	sprintf(tstr, "{ x: %d, y: %d, num_heat: %d, ", x, y, num_heat);
	strcat(str, tstr);
	sprintf(tstr, "{points: [ ");
	strcat(str, tstr);
	for(i = 0; i < num_heat; i++) {
		sprintf(tstr, "{ x: %d, y: %d, t: %f }", hps[i].x, hps[i].y, hps[i].t);
		strcat(str, tstr); 
	}
	strcat(str, " ] } } ");
	printf("%s\n", str);
	return str;
}	

char* mkJsonQuery(int x, int y) {
	char* str = xmalloc(512 * sizeof(char));
	sprintf(str, "{ x: %d, y: %d}", x, y);
	printf("%s\n", str);
	return str;
}
