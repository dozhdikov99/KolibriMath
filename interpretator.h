/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef  INTERPRETATOR_H

#define INTERPRETATOR_H

#include <string.h>
#include "data_structures.h"
#include "globals.h"
#include <math.h>
#include <ctype.h>
#include "parser.h"
#include "matrix.h"

void printResult(Result* result);

void run(Expression* expression);

void freeTables();

short initTables();

#endif

