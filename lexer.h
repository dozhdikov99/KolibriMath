/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef  LEXER_H
#define LEXER_H

#include "data_structures.h"
#include <string.h>
#include <stdlib.h>

extern Array* tokenize(char* string);

void freeToken(Token* token);

void freeArray(Array* array);

#endif
