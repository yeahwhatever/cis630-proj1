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
	free(tstr);
	strcat(str, " ] } } ");
	printf("%s\n", str);
	return str;
}	

char* mkJsonQuery(int x, int y) {
	char* str = xmalloc(512 * sizeof(char));
	sprintf(str, "{ x: %d, y: %d, }", x, y);
	return str;
}

struct heatpoint *parseJson(char* json, int *width, int *height, int *num_heat) {
	int i = 1, j, nstart;
	char getX, getY, getNum, hpCoords, getT, inHp, cont, *numStr;
	struct heatpoint *hps;

	getX = getY = getNum = hpCoords = 0;
	while(json[i] != '[')
	{
		switch(json[i]) {
			case 'x':
				getX = 1;
				i++;
				break;
			case 'y':
				getY = 1;
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
				while(json[i] != ' ' ) {
					i++;
				}
				numStr = xmalloc(i-nstart);
				strncpy(numStr, json+nstart, i-nstart-1);
				numStr[i-nstart-1] = '\0';
				if(getX) {
					*width = atoi(numStr);
					getX = 0;
				}
				if(getY) {
					*height = atoi(numStr);
					getY = 0;
				}
				if(getNum) {
			       		*num_heat = atoi(numStr);	       
					getNum = 0;
				}
				free(numStr);
				break;

			case 'n':
				if(strncmp(json+i, "num_heat", 8) == 0) {
					i += 8;
					getNum = 1;
				}
				i++;
				break;
			default:
				i++;
				break;
		}
	}

	hps = xmalloc(sizeof(struct heatpoint) * *num_heat);
	j = -1;
	inHp = getX = getY = getT = 0;
	cont = 1;
       	while(cont) {
		switch(json[i]) {
			case '{':
				j++;
				i++;
				inHp = 1;
				break;
			case '}':
				if(inHp) {
					inHp = 0;
				} else {
					cont = 0;
				}
				i++;
				break;
			case 'x':
				getX = 1;
				i++;
				break;
			case 'y':
				getY = 1;
				i++;
				break;
			case 't':
				getT = 1;
				i++;
				break;
			case '-':
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
				numStr = xmalloc(i-nstart);
				strncpy(numStr, json+nstart, i-nstart-1);
				numStr[i-nstart-1] = '\0';
				if(getX) {
					hps[j].x = atoi(numStr);
					getX = 0;
				}
				if(getY) {
					hps[j].y = atoi(numStr);
					getY = 0;
				}
				if(getT) {
			       		hps[j].t = atof(numStr);	       
					getT = 0;

				}
				free(numStr);
				break;
			default: 
				i++;
				break;		


		}
	}
	return hps;
}

void parseJsonQuery( char* json, int *x, int *y) {
	int i = 0;
	int nstart = 0;
	char getX, getY, *numStr;
	getX = getY = 0;
	while(json[i] != '}') {
		switch(json[i]) {
			case 'x':
				getX = 1;
				i++;
				break;
			case 'y':
				getY = 1;
				i++;
				break;
			case '-':
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
				while(json[i] != ' ' && json[i] != ',') {
					i++;
				}
				numStr = xmalloc(i-nstart);
				strncpy(numStr, json+nstart, i-nstart);
				numStr[i-nstart] = '\0';
				if(getX) {
					*x = atoi(numStr);
					getX = 0;
				}
				if(getY) {
					*y = atoi(numStr);
					getY = 0;
				}
				free(numStr);
				break;
			default: 
				i++;
				break;		
		}
	}
}
