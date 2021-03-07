#include "file.h"

Environment* environment;
const char* errors1[] = {"путь не найден [код: 1.0].", "невозможно открыть файл [код: 1.1].", "файл не может быть слишком большой [код: 1.2].", "ошибка выделения памяти [код: 1.3]."};

void runFromFile(char* path) {
	int i = 0;
	char symbol;
	Array* array;
	Expression* expression;
	char str[MAX_EXPRESSION];
	char* string;
	if (path == NULL) {
		error(errors1[0]);
		return;
	}
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		error(errors1[1]);
		return;
	}
	symbol = getc(file);
	while (1) {
		str[i] = symbol;
		if (symbol == EOF) {
			str[i] = '\0';
			if (i > MAX_EXPRESSION) {
				error2(errors1[2]);
				return;
			}
			if (i > 0) {
				string = (char*)malloc(sizeof(char) * (i + 1));
				if (string == NULL) {
					error(errors1[3]);
					return;
				}
				for (int j = 0; j < i + 1; j++) {
					string[j] = str[j];
				}
				environment = (Environment*)malloc(sizeof(Environment));
	            if(environment == NULL){
		            error2(errors1[3]);
		            return;
	            }
	            (*environment).isFile = 1;
	            (*environment).pos = 1;
	            (*environment).line = 1;
	            environment->file = path;
	            addEnvironment(environment);
				array = tokenize(string);
				if (array != NULL) {
					expression = parse(array);
					if (expression != NULL) {
						run(expression);
					}
				}
				removeEnvironment();
				free(string);
			}
			break;
		}
		symbol = getc(file);
		i++;
	};
	fclose(file);
}