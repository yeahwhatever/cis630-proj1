#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.c"
#include "client.h"

#define LINE 100
#define false 0
#define true 1
#define bool char


struct heatpoint *parseJson(char* json, int *width, int *height, int *num_heat) {
	int len = strlen(json);
	int i = 1;
	int nstart;
	bool getX, getY, getNum, hpCoords, getT;
	getX = getY = getNum = hpCoords = false;
	printf("%s\n", json);
	while(json[i] != '[')
	{
		//printf("i: %d char: %c\n", i, json[i]);
		switch(json[i]) {
			case 'x':
				getX = true;
				i++;
				break;
			case 'y':
				getY = true;
				i++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nstart = i;
				while(json[i] != ' ') {
					i++;
				}
				char* numStr = xmalloc(i-nstart);
				strncpy(numStr, json+nstart, i-nstart-1);
				if(getX) {
					*width = atoi(numStr);
					getX = false;
				}
				if(getY) {
					*height = atoi(numStr);
					getY = false;
				}
				if(getNum) {
			       		*num_heat = atoi(numStr);	       
					getNum = false;
				}
				free(numStr);
				break;

			case 'n':
				if(strncmp(json+i, "num_heat", 8) == 0) {
					i += 8;
					getNum = true;
				}
				i++;
				break;
			default:
				i++;
				break;
		}
	}

	int heatCount = 0;
	struct heatpoint *hps = xmalloc(sizeof(struct heatpoint) * *num_heat);
	int j = -1;
	bool inHp, cont;
	inHp = getX = getY = getT = false;
	cont = true;
       	while(cont) {
		switch(json[i]) {
			case '{':
				j++;
				i++;
				inHp = true;
				break;
			case '}':
				if(inHp) {
					inHp = false;
				} else {
					cont = false;
				}
				i++;
				break;
			case 'x':
				getX = true;
				i++;
				break;
			case 'y':
				getY = true;
				i++;
				break;
			case 't':
				getT = true;
				i++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nstart = i;
				while(json[i] != ' ') {
					i++;
				}
				char* numStr = xmalloc(i-nstart);
				strncpy(numStr, json+nstart, i-nstart-1);
				if(getX) {
					hps[j].x = atoi(numStr);
					printf("j: %d, x: %d\n", j, hps[j].x);
					getX = false;
				}
				if(getY) {
					hps[j].y = atoi(numStr);
					printf("j: %d, y: %d\n", j, hps[j].y);
					getY = false;
				}
				if(getT) {
			       		hps[j].t = atof(numStr);	       
					printf("j: %d, t: %f\n", j, hps[j].t);
					getT = false;

				}
				free(numStr);
				break;
			default: 
				i++;
				break;		


		}
	}
	for(i = 0; i < *num_heat; i++) {
		printf("x: %d, y: %d, t: %f\n", hps[i].x, hps[i].y, hps[i].t);
	}
	return hps;
}
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
