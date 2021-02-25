/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef  INTERPRETATOR_H

#define INTERPRETATOR_H

#include "globals.h"
#include "out.h"
#include "cvector.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "file.h"
#include "parser.h"
#include "utils.h"
#include <ctype.h>

enum ResultType {
	ResultType_Int,
	ResultType_Float,
	ResultType_Undefined,
	ResultType_Str,
	ResultType_Spec
};

typedef struct {
	double value;
	char* str;
	enum ResultType type;
} Result;

typedef struct {
	char* name;
	double value;
	enum VarType type;
} Variable_;

typedef struct {
	void* function;
	int args;
	enum VarType* argTypes;
} Function_;

void print(Result result);

void run(Expression* expression);

void freeTables();

short initTables();

#endif

