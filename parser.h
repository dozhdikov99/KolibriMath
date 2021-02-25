/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef SYNTAX_H

#define SYNTAX_H

#include "out.h"
#include "cvector.h"
#include <string.h>
#include "lexer.h"
#include <stdlib.h>
#include "utils.h"

static const char* ERRORS[] = {
	"При парсинге произошла ошибка.",
	"Неожиданный конец выражения.",
	"Ожидалась операция удаления.",
	"Ожидалось название переменной.",
	"Ожидался символ \',\' или конец выражения.",
	"Ожидался символ \'=\'",
	"Ошибка при получении числа.",
	"Ожидалось название функции.",
	"Неожиданный символ.",
	"Ожидалось число.",
	"Ошибка выделения памяти.",
	"Строка не может быть частью арифметического выражения."
};

enum PartType {
	PartType_Set,
	PartType_Arithmetic,
	PartType_Del,
	PartType_NewLine,
	PartType_Str
};

enum VarType {
	VarType_Int,
	VarType_Float,
	VarType_Undefined,
	VarType_Str
};

enum OperandType {
	OperandType_Function,
	OperandType_Number,
	OperandType_Variable,
	OperandType_Arithmetic,
	OperandType_Unar,
	OperandType_Str
};

enum OpType {
	OpType_Add,
	OpType_Mul,
	OpType_Sub,
	OpType_Div
};

typedef struct {
	void* data;
	enum PartType type;
} Part;

typedef struct {
	void* data;
	enum VarType type;
} Const;

typedef struct {
	void* data;
	enum OperandType type;
} Operand;

typedef struct {
	char* name;
	Operand* operands;
	int count;
} Function;

typedef struct {
	Operand* operands;
	enum OpType* operations;
	int operandsCount;
	int operationsCount;
} Arithmetic;

typedef struct {
	Operand* variable;
	Operand* source;
} Setting;

typedef struct {
	Part* parts;
	int count;
} Expression;

void freeExpression(Expression* expression);

Expression* parse(Array* array);

#endif 