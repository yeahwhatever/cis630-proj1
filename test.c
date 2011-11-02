#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "client.h"

#define LINE 100
#define false 0
#define true 1
#define bool char


int main() {
	struct heatpoint *hps;
	int i;
	hps = xmalloc(5 * sizeof(struct heatpoint));	
	for(i = 0; i < 5; i++ ) {
		hps[i].x = i;
		hps[i].y = i;
		hps[i].t = i;
	}	
	char* json = mkJson(1, 2, 5, hps);
	int width, height, num_heat;
	struct heatpoint *hhps = parseJson(json, &width, &height, &num_heat);
	printf("height: %d\nwidth: %d\nnum_heat: %d\n",height, width, num_heat);
	for(i = 0; i < num_heat; i++) {
		printf("x: %d, y: %d, t: %f\n", hhps[i].x, hhps[i].y, hhps[i].t);
	}
	return 0;
}
