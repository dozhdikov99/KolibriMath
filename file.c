#include "file.h"

short IS_FILE = 0;

void runFromFile(char* path) {
	IS_FILE = 1;
	int i = 0;
	char symbol;
	Array* array;
	Expression* expression;
	char str[MAX_EXPRESSION];
	char* string;
	if (path == NULL) {
		error2("Путь не найден.");
		return;
	}
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		error2("Невозможно открыть файл.");
		return;
	}
	symbol = getc(file);
	while (1) {
		str[i] = symbol;
		if (symbol == EOF) {
			str[i] = '\0';
			if (i > MAX_EXPRESSION) {
				error2("Файл не может быть слишком большой.");
				return;
			}
			if (i > 0) {
				string = (char*)malloc(sizeof(char) * (i + 1));
				if (string == NULL) {
					error2("Ошибка выделения памяти.");
					return;
				}
				for (int j = 0; j < i + 1; j++) {
					string[j] = str[j];
				}
				array = tokenize(string);
				if (array != NULL) {
					expression = parse(array);
					if (expression != NULL) {
						run(expression);
					}
				}
				free(string);
			}
			break;
		}
		symbol = getc(file);
		i++;
	};
	fclose(file);
	IS_FILE = 0;
}