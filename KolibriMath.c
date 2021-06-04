/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/

#include <string.h>
#include <kos32sys.h>
#include <console_obj.h>
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "lexer.h"
#include "out.h"
#include "parser.h"
#include "interpretator.h"

char string[MAX_EXPRESSION];
char* new_string;
Environment* environment;
const char* errors0[] = {"Строка не может быть слишком длинной [код: 0.0].", "Ошибка выделения памяти [код: 0.1]."};

short IsWork() {
	printf("\n>>> ");
	Array* array;
	Expression* expression;
	gets(string);
	if (string == NULL) {
		error2(errors0[0]);
		return 0;
	}
	int i = 0;
	for (; string[i] != '\0'; i++) {
	}
	i++;
	new_string = (char*)malloc(sizeof(char) * i);
	if (new_string == NULL) {
		error2(errors0[1]);
		return 0;
	}
	for (int j = 0; j < i; j++) {
		new_string[j] = string[j];
	}
	new_string[i] = '\0';
	environment = (Environment*)malloc(sizeof(Environment));
	if (environment == NULL) {
		error2(errors0[1]);
		return 0;
	}
	(*environment).isFile = 0;
	(*environment).pos = 1;
	(*environment).line = 1;
	addEnvironment(environment);
	array = tokenize(new_string);
	if (array != NULL) {
		expression = parse(array);
		if (expression != NULL) {
			run(expression);
		}
	}
	removeEnvironment();
	return 1;
}


void main(int argc, char** argv) {
    load_console();
	con_set_title("KolibriMath");
	printf("KolibriMath %s.\n", ver);
	printf("Для просмотра справки введите help().\n");
	if (initTablesAndVariables()) {
		initStack();
		if (argc == 2) {
			printf(">>> file(\'%s\')\n", argv[1]);
			runFromFile(argv[1]);
		}
		while (IsWork()) {}
	}
}