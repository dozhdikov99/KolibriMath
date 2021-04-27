/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef OUT_H

#define OUT_H

#include "data_structures.h"
#include "cvector.h"

extern void error(const char* msg);
extern void error2(const char* msg);
extern void initStack();
extern void clearStack();
extern void addEnvironment(Environment* environment);
extern void removeEnvironment();
extern Environment* getCurrent();
#endif

