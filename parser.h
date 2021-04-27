/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef SYNTAX_H

#define SYNTAX_H

#include "data_structures.h"
#include "utils.h"
#include "out.h"

void freeExpression(Expression* expression);

Expression* parse(Array* array);

#endif 