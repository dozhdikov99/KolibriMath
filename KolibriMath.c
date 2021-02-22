/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/

#include <stdio.h>
#include "globals.h"
#include "lexer.h"
#include "out.h"
#include "parser.h"
#include "interpretator.h"
#include <kos32sys.h>
#include <stdlib.h>
#include <console_obj.h>
#include "file.h"

char string[MAX_EXPRESSION];

short IsWork() {
	printf(">>>");
	Array* array;
	Expression* expression;
	gets(string);
	if (string == NULL) {
		error2("Строка не может быть слишком длинной.");
		return 0;
	}
	int i = 0;
	for (; string[i] != '\0'; i++) {
	}
	i++;
	String = (char*)malloc(sizeof(char) * i);
	if (String == NULL) {
		error2("Ошибка выделения памяти.");
		return 0;
	}
	for (int j = 0; j < i; j++) {
		String[j] = string[j];
	}
	String[i] = '\0';
	array = tokenize(String);
	Token* token;
	if (array != NULL) {
		expression = parse(array);
		if (expression != NULL) {
			run(expression);
		}
	}
	return 1;
}


void main(int argc, char** argv) {
	load_console();
	con_set_title("KolibriMath");
	printf("KolibriMath %s.\n", ver);
	printf("Для просмотра справки введите help().\n");
	if(initTables()){
		if (argc == 2) {
		printf(">>>file(\'%s\')\n", argv[1]);
		runFromFile(argv[1]);
		}
	while(IsWork()){}
	}
	con_exit(0);
}