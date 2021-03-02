/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "out.h"
#include <string.h>
#include "globals.h"

void error(const char* msg, int pos) {
	if (IS_FILE == 0) {
		printf("%s\n", String);
		for (int i = 0; i < pos; i++)
			printf(" ");
		printf("^");
	}
	else {
		printf("(символ %d)\n", pos);
	}
	printf("\nПроизошла ошибка: %s\n", msg);
}

void error2(const char* msg) {
	printf("\nПроизошла ошибка: %s\n", msg);
}