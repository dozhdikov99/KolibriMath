/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#ifndef  MATRIX_H

#define MATRIX_H

#include "data_structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Matrix* matrix_init_undefinedElementsType(int rowsCount, int columnsCount);

Matrix* matrix_init(int rowsCount, int columnsCount, enum VarType elementsType);

Matrix* matrix_copy(Matrix* oldMatrix);

Matrix* matrix_setElement(Matrix* matrix, int rowIndex, int columnIndex, double value);

Matrix* matrix_resize(Matrix* oldMatrix, int newRowCount, int newColumnCount);

void matrix_free(Matrix* matrix);

void matrix_print(Matrix* matrix);

Matrix* matrix_multiplication_withMatrix(Matrix* matrix1, Matrix* matrix2);

Matrix* matrix_multiplication_withNumber(Matrix* matrix, double number);

Matrix* matrix_addition(Matrix* matrix1, Matrix* matrix2);

Matrix* matrix_substraction(Matrix* matrix1, Matrix* matrix2);

short matrix_equals(Matrix* matrix1, Matrix* matrix2);

double matrix_det(Matrix* matrix);

Matrix* matrix_inverse(Matrix* matrix);

Matrix* matrix_T(Matrix* matrix);

#endif