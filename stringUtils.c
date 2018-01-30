#include <stdio.h>
#include <string.h>

#include "includes/stringUtils.h"
#include "includes/protocol.h"

int strToInt(char * buff) {
	int i = 0;
	while(*buff) {
		i = i * 10 + (*buff - '0');
		buff++;
	}
	return i;
}

char * intToStr(int val, char * buff) {
	int aux[MAX_NUM_SIZE] = {0};
	int i = 0, j = 0;
	memset(buff, 0, MAX_NUM_SIZE);

	sprintf(buff, "%d", val);

	return buff;
}

int addToStr(char * ptr, char * str) {
	int i = 0;
	while(str[i]) {
		ptr[i] = str[i];
		i++;
	}
	return i;
}