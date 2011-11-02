#ifndef COMMON_H
#define COMMON_H

struct heatpoint {
	int x;
	int y;
	float t;
};

struct sheet {
	int x;
	int y;

	int num_heat;
	struct heatpoint *hps;

	float **sheet;
	float **prev_sheet;

	int checked;
};

void* xmalloc(unsigned int size);

char* mkJson(int x, int y, int num_heat, struct heatpoint *hps );
char* mkJsonQuery(int x, int y);
struct heatpoint *parseJson(char* json, int *width, int *height, int *num_heat);
void parseJsonQuery(char* json, int *x, int *y);
#endif
