/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef OUT_H

#define OUT_H

#include <stdio.h>
#include "globals.h"
#include "cvector.h"

typedef struct
{
    char* file;
    char* str;
    short isFile;
    short type; // 0 - lexer, 1 - parser, 2 - interpretator
    unsigned short line;
    unsigned short pos;
} Environment;

extern void error(const char* msg);
extern void error2(const char* msg);
extern void initStack();
extern void clearStack();
extern void addEnvironment(Environment* environment);
extern void removeEnvironment();
extern Environment* getCurrent();
#endif

