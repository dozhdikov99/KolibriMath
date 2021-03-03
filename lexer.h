/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef  LEXER_H
#define LEXER_H

#include "out.h"
#include <string.h>
#include <stdlib.h>

enum TokenType {
	TokenType_Float,
	TokenType_Int,
	TokenType_Name,
	TokenType_LeftBracket,
	TokenType_RightBracket,
	TokenType_OpAdd,
	TokenType_OpSub,
	TokenType_OpDiv,
	TokenType_OpMul,
	TokenType_OpEquals,
	TokenType_Del,
	TokenType_Sep,
	TokenType_NewLineSep,
	TokenType_Str,
	TokenType_If,
	TokenType_Repeat,
	TokenType_LeftFigureBracket,
	TokenType_RightFigureBracket,
	TokenType_And,
	TokenType_Or,
	TokenType_Equals,
	TokenType_NotEquals,
	TokenType_More,
	TokenType_EqualsOrMore,
	TokenType_Less,
	TokenType_EqualsOrLess,
	TokenType_Inverse,
	TokenType_False,
	TokenType_True,
	TokenType_Elif,
	TokenType_Else,
	TokenType_End
};

typedef struct {
	char* data;
	enum TokenType type;
	int coord;
} Token;

typedef struct {
	Token* data;
	int size;
} Array;

extern Array* tokenize(char* string);

void freeToken(Token* token);

void freeArray(Array* array);

#endif
