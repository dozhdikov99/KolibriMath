/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "out.h"
#include <string.h>
#include "globals.h"

cvector* stack;
const char* errors4[] = {"ошибка выделения памяти [код: 4.0]."};

void freeEnvironment(Environment* environment){
	free(environment);
}

void initStack(){
	stack = (cvector*)malloc(sizeof(cvector));
	if(stack == NULL){
		error2(errors4[1]);
		return;
	}
	cvector_init(stack);
}

void clearStack(){
	for(int i = 0; i < cvector_size(stack); i++){
		freeEnvironment(cvector_get(stack, i));
	}
	cvector_free(stack);
	free(stack);
}

void addEnvironment(Environment* environment){
	cvector_push_back(stack, environment);
}

void removeEnvironment(){
	Environment* environment = cvector_get(stack, cvector_size(stack)-1);
	vector_delete(stack, cvector_size(stack)-1);
	freeEnvironment(environment);
}

Environment* getCurrent(){
	if(cvector_size(stack) > 0){
		return (Environment*)cvector_get(stack, cvector_size(stack)-1);
	}else{
		return NULL;
	}
}

void error(const char* msg) {
	Environment* environment;
	for(int i = 0; i < cvector_size(stack); i++){
		environment = cvector_get(stack, i);
		if((*environment).isFile){
			if((*environment).type == 0 || (*environment).type == 1){
				printf("в файле \'%s\' (строка %hu, символ %hu)\n", environment->file, (*environment).line, (*environment).pos);
			    printf("%s\n", environment->str);
				printf("^\n");
			}else if((*environment).type == 3){
				printf("в файле \'%s\' (строка %hu, символ %hu)\n", environment->file, (*environment).line, (*environment).pos+1);
			    printf("%s\n", environment->str);
				printf("^\n");
			}else{
				printf("в файле \'%s\'\n", environment->file);
			}
		}else{
			printf("%s\n", environment->str);
			for (int p = 0; p < (*environment).pos; p++)
		    {
				printf(" ");
			}
		    printf("^\n");
		}
	}
	printf("\nпроизошла ошибка: %s\n", msg);
}

void error2(const char* msg) {
	printf("\nпроизошла ошибка: %s\n", msg);
}