#ifndef FILE_H

#define FILE_H
#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "out.h"
#include "interpretator.h"
#include "cvector.h"
#include "globals.h"

void runFromFile(char* path);
#endif