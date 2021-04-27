/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/

#include "matrix.h"

#define DEFAULT_ELEMENT_VALUE 0

#define MAX_ELEMENT_LENGHT 100

#define ALLOCATE_MEMORY_ERROR 0

const char* matrixErrorMsgs[] = {"ошибка выделения памяти [код: 6.0]."};

Matrix* matrix_init_undefinedElementsType(int rowsCount, int columnsCount){
    Matrix* newMatrix = (Matrix*)malloc(sizeof(Matrix));
    if(newMatrix == NULL){
        error2(matrixErrorMsgs[ALLOCATE_MEMORY_ERROR]);
        return NULL;
    }
    newMatrix->elements = (double**)malloc(sizeof(double)*rowsCount);
    if(newMatrix->elements == NULL){
        free(newMatrix);
        error2(matrixErrorMsgs[ALLOCATE_MEMORY_ERROR]);
        return NULL;
    }
    for (int i = 0; i < rowsCount; i++) {
        newMatrix->elements[i] = (double*)malloc(sizeof(double) * columnsCount);
    }
    for(int i = 0; i < rowsCount; i++){
        for (int j = 0; j < columnsCount; j++) {
            newMatrix->elements[i][j] = DEFAULT_ELEMENT_VALUE;
        }
    }
    newMatrix->elementsType = VarType_Undefined;
    newMatrix->rowsCount = rowsCount;
    newMatrix->columnsCount = columnsCount;
    return newMatrix;
}

Matrix* matrix_init(int rowsCount, int columnsCount, enum VarType elementsType){
    if(elementsType != VarType_Float && elementsType != VarType_Int && rowsCount < 0 && columnsCount < 0){
        return NULL;
    }
    Matrix* newMatrix = matrix_init_undefinedElementsType(rowsCount, columnsCount);
    if(newMatrix != NULL)
    newMatrix->elementsType = elementsType;
    return newMatrix;
}

Matrix* matrix_setElement(Matrix* matrix, int rowIndex, int columnIndex, double value){
    if(matrix == NULL || rowIndex < 0 || columnIndex < 0){
        return NULL;
    }
    if(rowIndex >= matrix->rowsCount || columnIndex >= matrix->columnsCount){
        Matrix* new_matrix = matrix_init(rowIndex + 1, columnIndex + 1, matrix->elementsType);
        for (int i = 0; i < rowIndex + 1; i++) {
            for (int j = 0; j < columnIndex + 1; j++) {
                new_matrix->elements[i][j] = DEFAULT_ELEMENT_VALUE;
                if (i < (*matrix).rowsCount && j < (*matrix).columnsCount) {
                    new_matrix->elements[i][j] = matrix->elements[i][j];
                }
            }
        }
        new_matrix->elements[rowIndex][columnIndex] = value;
        matrix_free(matrix);
        return new_matrix;
    }
    matrix->elements[rowIndex][columnIndex] = value;
    return matrix;
}

Matrix* matrix_resize(Matrix* oldMatrix, int newRowCount, int newColumnCount) {
    if (newRowCount <= 0 || newColumnCount <= 0)
        return NULL;
    Matrix* newMatrix = matrix_init(newRowCount, newColumnCount, oldMatrix->elementsType);
    for (int i = 0; i < min_value(oldMatrix->rowsCount, newRowCount); i++) {
        for (int j = 0; j < min_value(oldMatrix->columnsCount, newColumnCount); j++) {
            if (oldMatrix->elementsType == VarType_Int) {
                newMatrix->elements[i][j] = (int)oldMatrix->elements[i][j];
            }
            else {
                newMatrix->elements[i][j] = oldMatrix->elements[i][j];
            }
        }
    }
    matrix_free(oldMatrix);
    return newMatrix;
}

int min_value(int value1, int value2) {
    if (value1 < value2)
        return value1;
    else
        return value2;
}

void matrix_free(Matrix* matrix){
    for (int i = 0; i < matrix->rowsCount; i++) {
        free(matrix->elements[i]);
    }
    free(matrix->elements);
    free(matrix);
}

void matrix_print(Matrix* matrix){
    if(matrix == NULL){
        return;
    }
    for (int i = 0; i < matrix->rowsCount; i++) {
        for (int j = 0; j < matrix->columnsCount; j++) {
            if (matrix->elementsType == VarType_Int) {
                printf("%5d ", (int)matrix->elements[i][j]);
            }
            else {
                printf("%5f ", matrix->elements[i][j]);
            }
        }
        printf("\n");
    }
}

Matrix* matrix_copy(Matrix* oldMatrix) {
    Matrix* newMatrix = matrix_init(oldMatrix->rowsCount, oldMatrix->columnsCount, oldMatrix->elementsType);
    if (newMatrix == NULL) {
        return NULL;
    }
    for (int i = 0; i < oldMatrix->rowsCount; i++) {
        for (int j = 0; j < oldMatrix->columnsCount; j++) {
            newMatrix->elements[i][j] = oldMatrix->elements[i][j];
        }
    }
    return newMatrix;
}

Matrix* matrix_multiplication_withMatrix(Matrix* matrix1, Matrix* matrix2){
    if(matrix1 == NULL || matrix2 == NULL || matrix1->columnsCount != matrix2->rowsCount || matrix1->rowsCount != matrix2->columnsCount)
    {
        return NULL;
    }
    enum VarType newMatrixType;
    if (matrix1->elementsType == VarType_Float || matrix2->elementsType == VarType_Float)
        newMatrixType = VarType_Float;
    else
        newMatrixType = VarType_Int;
    Matrix* newMatrix = matrix_init(matrix1->rowsCount, matrix2->columnsCount, newMatrixType);
    if(newMatrix == NULL){
        return NULL;
    }
    for(int i = 0; i < matrix1->rowsCount; i++){
        for(int j = 0; j < matrix2->columnsCount; j++){
            for(int p = 0; p < matrix1->columnsCount; p++){
                if(newMatrixType == VarType_Float)
                    newMatrix->elements[i][j] += matrix1->elements[i][p] * matrix2->elements[p][j];
                else
                    newMatrix->elements[i][j] += (int)((int)matrix1->elements[i][p] * (int)matrix2->elements[p][j]);
            }
        }
    }
    return newMatrix;
}

Matrix* matrix_multiplication_withNumber(Matrix* matrix, double number){
    if(matrix == NULL){
        return NULL;
    }
    Matrix* newMatrix = matrix_init(matrix->rowsCount, matrix->columnsCount, matrix->elementsType);
    for(int i = 0; i < matrix->rowsCount; i++){
        for(int j = 0; j < matrix->columnsCount; j++){
            newMatrix->elements[i][j] = matrix->elements[i][j] * number;
        }
    }
    return newMatrix;
}

Matrix* matrix_addition(Matrix* matrix1, Matrix* matrix2){
    if(matrix1 == NULL || matrix2 == NULL || matrix1->columnsCount != matrix2->columnsCount || matrix1->rowsCount != matrix2->rowsCount){
        return NULL;
    }
    enum VarType newMatrixType = VarType_Float;
    if(matrix1->elementsType == VarType_Int && matrix2->elementsType == VarType_Int){
        newMatrixType = VarType_Int;
    }
    Matrix* newMatrix = matrix_init(matrix1->rowsCount, matrix1->columnsCount, newMatrixType);
    if(newMatrix == NULL){
        return NULL;
    }
    for(int i = 0; i < matrix1->rowsCount; i++){
        for(int j= 0; j < matrix1->columnsCount; j++){
            if (newMatrixType == VarType_Float)
                newMatrix->elements[i][j] = matrix1->elements[i][j] + matrix2->elements[i][j];
            else
                newMatrix->elements[i][j] = (int)matrix1->elements[i][j] + (int)matrix2->elements[i][j];
        }
    }
    return newMatrix;
}

short matrix_equals(Matrix* matrix1, Matrix* matrix2){
    if(matrix1 == NULL && matrix2 == NULL){
        return 1;
    }else if(matrix1 != NULL && matrix2 != NULL){
        if(matrix1->columnsCount != matrix2->columnsCount || matrix1->rowsCount != matrix2->rowsCount){
            return 0;
        }
        for(int i = 0; i < matrix1->rowsCount; i++){
            for(int j = 0; j < matrix1->columnsCount; j++){
                if(matrix1->elements[i][j] != matrix2->elements[i][j]){
                    return 0;
                }
            }
        }
        return 1;
    }else{
        return 0;
    }
}

Matrix* matrix_substraction(Matrix* matrix1, Matrix* matrix2) {
    if (matrix1 == NULL || matrix2 == NULL || matrix1->columnsCount != matrix2->columnsCount || matrix1->rowsCount != matrix2->rowsCount) {
        return NULL;
    }
    enum VarType newMatrixType = VarType_Float;
    if (matrix1->elementsType == VarType_Int && matrix2->elementsType == VarType_Int) {
        newMatrixType = VarType_Int;
    }
    Matrix* newMatrix = matrix_init(matrix1->rowsCount, matrix1->columnsCount, newMatrixType);
    if (newMatrix == NULL) {
        return NULL;
    }
    for (int i = 0; i < matrix1->rowsCount; i++) {
        for (int j = 0; j < matrix1->columnsCount; j++) {
            if (newMatrixType == VarType_Float)
                newMatrix->elements[i][j] = matrix1->elements[i][j] - matrix2->elements[i][j];
            else
                newMatrix->elements[i][j] = (int)matrix1->elements[i][j] - (int)matrix2->elements[i][j];
        }
    }
    return newMatrix;
}