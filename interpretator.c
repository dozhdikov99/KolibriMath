/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "interpretator.h"

enum TrigonometryFuncType {
	TrigonomertyFunc_Sin,
	TrigonomertyFunc_Cos
};

enum InputType {
	InputType_WithoutText,
	InputType_WithText
};

cvector* variables;
cvector* functions;
Environment* environment;

char* stringInput;
short value = 0;
const char* errors5[] = { "ошибка выделения памяти [код: 5.0].", "строка не может быть слишком длинной [код 5.1].", "функция с таким числом аргументов не найдена [код 5.2].", "не удалось выполнить функцию [код: 5.3]", "функция \'sqrt\' принимает отрицательный аргумент [код 5.4].", "функция \'pow\' принимает нулевой показатель [код: 5.5].", "изменение невозможно, так как переменная принадлежит другому типу [код: 5.6].", "индекс имеет недопустимый формат [код: 5.7].", "переменная с таким именем не найдена [код: 5.8].", "деление на ноль запрещено [код: 5.9].", "результат операции над матрицами не получен [код: 5.10].", "данная операция неприменима к матрицам [код: 5.11].", "переменная с таким именем уже существует [код: 5.12].", "нельзя изменить системную переменную [код: 5.13]." };

Result* runArithmetic(Arithmetic* arithmetic);
Result* runOperand(Operand* operand);
Result* runUnar(Operand* operand);
Result* runFunction(Function* function);
Result* runInverse(Operand* operand);
Result* runLogicConst(Const* logicConst);
Result* runLogic(Logic* logic);
Result* runMatrixInitializator(MatrixInitializator* m);
Result* runMatrixElement(MatrixElement* element);
void runPart(Part* part);

short isNumber(Result* result) {
	return result != NULL && result->type != VarType_Undefined && result->type != VarType_Str && result->type != VarType_FloatMatrix && result->type != VarType_IntMatrix;
}

short isStr(Result* result) {
	return result->type == ResultType_Str;
}

short isMatrix(Result* result) {
	return result->type == ResultType_IntMatrix || result->type == ResultType_FloatMatrix;
}

void freeVariable(Variable_* variable) {
	switch (variable->type) {
	case VarType_FloatMatrix:
	case VarType_IntMatrix:
		matrix_free(variable->matrix);
		free(variable);
		break;
	default:
		free(variable);
	}
}

Result* getNumberFromStr(Result* result) {
	int symbolIndex = 0;
	int pointsCount = 0;
	enum ResultType type = ResultType_Int;
	while (stringInput[symbolIndex] != '\0') {
		if (stringInput[symbolIndex] == '-' && symbolIndex == 0) {
			symbolIndex++;
			continue;
		}
		else if (isalpha(stringInput[symbolIndex])) {
			break;
		}
		else if (isdigit(stringInput[symbolIndex])) {
			symbolIndex++;
			continue;
		}
		else if (stringInput[symbolIndex] == '.' && pointsCount == 0) {
			pointsCount++;
			type = ResultType_Float;
		}
		else {
			break;
		}
		symbolIndex++;
	}
	if (symbolIndex == 0) {
		type = ResultType_Spec;
	}
	result->type = type;
	if (type == ResultType_Int) {
		(*result).value = (double)atoi(stringInput);
	}
	else if (type == ResultType_Float) {
		(*result).value = (double)atof2(stringInput);
	}
	return result;
}

Variable_* findVariable(char* name) {
	char* variableName;
	for (int i = 0; i < cvector_size(variables); i++) {
		variableName = (char*)((Variable_*)cvector_get(variables, i))->name;
		if (!strcmp(variableName, name)) {
			return cvector_get(variables, i);
		}
	}
	return NULL;
}

void deleteVariable(Variable_* variable) {
	char* variableName;
	for (int i = 0; i < cvector_size(variables); i++) {
		variableName = (char*)((Variable_*)cvector_get(variables, i))->name;
		if (!strcmp(variableName, variable->name)) {
			freeVariable((Variable_*)cvector_get(variables, i));
			vector_delete(variables, i);
		}
	}
}

void addVariable(Variable_* variable) {
	cvector_push_back(variables, variable);
}

void printResult(Result* result) {
	switch ((*(Result*)result).type) {
	case ResultType_Int:
		printf("%d", (int)(*(Result*)result).value);
		break;
	case ResultType_Float:
		printf("%f", (*(Result*)result).value);
		break;
	case ResultType_Spec:
		break;
	case ResultType_Logic:
		if ((*(Result*)result).value == 0.0) {
			printf("false");
		}
		else {
			printf("true");
		}
		break;
	case ResultType_IntMatrix:
	case ResultType_FloatMatrix:
		matrix_print(result->matrix);
		break;
	}
	free(result);
}

void runHelpFunc(Result* emptyResult) {
	printf("Список доступных команд:\n");
	printf("help() - получить сведения о командах.\n");
	printf("about() - получить сводку о программе.\n");
	printf("lang() - узнать информацию о языке KolibriMath.\n");
}

void runAboutFunc(Result* emptyResult) {
	printf("Версия программы: %s.\n", ver);
	printf("Разработчик: Дождиков Игорь.\n");
	printf("(C) 2021 год.\n");
	printf("Хочется сказать спасибо команде Kolibri OS за помощь!\n");
	printf("Отдельное спасибо Рустему (rgimad) спасибо за библиотеку cvector!\n");
}

void runLangFunc(Result* emptyResult) {
	printf("=== Справка о языке KolibriMath ===\n");
	printf("Язык KolibriMath является интерпретируемым языком с динамической типизацией.\n");
	printf("Основное назначение языка на данный момент - проведение расчетов.\n");
	printf("Язык прозволяет работать с переменными двух типов: int, float. Тип задается при присваивании автоматически.\n");
	printf("При присваивании используется следующая структура: <переменная> = <выражение>.\n");
	printf("Допускаются операции с числами: +, -, /, *. Имеются сокращенные операции присваивания: +=, -=, /=, *=.");
	printf("Поддерживаются матрицы целочисленные и дробные. Для матриц доступны операции: +, -, *, ==, !=.\n");
	printf("Имеются блоки if(<условие>){<тело>}, elif(<условие>){<тело>}, else{<тело>}, repeat(<условие>){<тело>}.\n");
	printf("Логические операции: & (И), | (ИЛИ), ! (НЕ), ==, !=, <=, <, >=, >.\n");
	printf("Удаление переменной: del <переменная>.\n");
	printf("Доступны четыре базовые операции: *, /, +, -.\n");
	printf("Возможно написание выражений через запятые в одну строку.\n");
	printf("Доступны функции: int(<число>), float(<число>), sqrt(<число>), pow2(<число>),\n");
	printf("pow(<число>, <число>), sin(<число>), cos(<число>), abs(<число>), \nfile(<путь к файлу>),\n");
	printf("input(), input(<подсказка к вводу>), ceil(<число>), mod(<число>).\n");
	printf("intMatrix(<название переменной>, <число строк>, <число столбцов>).\n");
	printf("floatMatrix(<название переменной>, <число строк>, <число столбцов>).\n");
	printf("resizeMatrix(<название переменной>, <новое число строк>, <новое число столбцов>).\n");
	printf("Имеются константы: PI, E.");
}

void runInputFunc(Result* emptyResult, Function* function, enum InputType inputType) {
	if (inputType == InputType_WithoutText) {
		printf("<");
		gets(stringInput);
		if (stringInput == NULL) {
			error(errors5[1]);
		}
		else {
			emptyResult = getNumberFromStr(emptyResult);
		}
	}
	else {
		Result* operand = runOperand(function->operands);
		if (operand != NULL && operand->type == VarType_Str) {
			printf("%s", operand->str);
			gets(stringInput);
			if (stringInput == NULL) {
				free(operand);
				error(errors5[1]);
			}
			emptyResult = getNumberFromStr(emptyResult);
			free(operand);
		}
		else {
			if (operand != NULL) {
				free(operand);
			}
			error(errors5[3]);
		}
	}

}

void runConvertTypeFunc(Result* emptyResult, Function* function, enum VarType targetType) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		switch (targetType) {
		case VarType_Int:
			emptyResult->type = VarType_Int;
			(*(Result*)emptyResult).value = (*(Result*)operand).value;
			break;
		case VarType_Float:
			emptyResult->type = ResultType_Float;
			(*(Result*)emptyResult).value = (*(Result*)operand).value;
			break;
		default:
			break;
		}
		free(operand);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runSqrtFunc(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand) && (*(Result*)operand).value > 0) {
		(*(Result*)emptyResult).value = sqrt((*(Result*)operand).value);
		emptyResult->type = VarType_Float;
		free(operand);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runTrigonomertyFunc(Result* emptyResult, Function* function, enum TrigonometryFuncType functionType) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		switch (functionType) {
		case TrigonomertyFunc_Sin:
			(*(Result*)emptyResult).value = sin((*(Result*)operand).value);
			break;
		case TrigonomertyFunc_Cos:
			(*(Result*)emptyResult).value = cos((*(Result*)operand).value);
			break;
		default:
			return;
		}
		emptyResult->type = ResultType_Float;
		free(operand);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runPow2Func(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		switch (operand->type) {
		case ResultType_Float:
			emptyResult->type = ResultType_Float;
			break;
		case ResultType_Int:
			emptyResult->type = ResultType_Int;
			break;
		default:
			return;
		}
		(*(Result*)emptyResult).value = pow((*(Result*)operand).value, 2);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runAbsFunc(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		switch (operand->type) {
		case ResultType_Float:
			emptyResult->type = ResultType_Float;
			break;
		case ResultType_Int:
			emptyResult->type = ResultType_Int;
			break;
		default:
			return;
		}
		(*(Result*)emptyResult).value = abs((*(Result*)operand).value);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runFileFunc(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (operand != NULL && operand->type == VarType_Str) {
		runFromFile((char*)((Result*)operand)->str);
		free(((Result*)operand)->str);
		free(operand);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runCeilFunc(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		emptyResult->type = ResultType_Float;
		(*(Result*)emptyResult).value = ceil((*(Result*)operand).value);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runModFunc(Result* emptyResult, Function* function) {
	Result* operand = runOperand(function->operands);
	if (isNumber(operand)) {
		double result0 = 0;
		emptyResult->type = ResultType_Float;
		(*(Result*)emptyResult).value = modf((*(Result*)operand).value, &result0);
	}
	else {
		if (operand != NULL) {
			free(operand);
		}
		error(errors5[3]);
	}
}

void runPowFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	Result* operand1 = runOperand(function->operands + 1);
	if (isNumber(operand0) && isNumber(operand1)) {
		if ((*(Result*)operand1).value != 0) {
			(*(Result*)emptyResult).value = pow((*(Result*)operand0).value, (*(Result*)operand1).value);
			emptyResult->type = ResultType_Float;
			free(operand0);
			free(operand1);
		}
		else {
			if (operand0 != NULL) {
				free(operand0);
			}
			if (operand1 != NULL) {
				free(operand1);
			}
			error(errors5[5]);
		}
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		if (operand1 != NULL) {
			free(operand1);
		}
		error(errors5[3]);
	}
}

void runIntMatrixFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	Result* operand1 = runOperand(function->operands + 1);
	Result* operand2 = runOperand(function->operands + 2);
	if (isStr(operand0) && isNumber(operand1) && isNumber(operand2)) {
		if ((*(Result*)operand1).value > 0 && (*(Result*)operand2).value > 0) {
			    if(findVariable(operand0->str) == NULL){
					Variable_* variable = (Variable_*)malloc(sizeof(Variable_));
					if (variable == NULL) {
						matrix_free(emptyResult->matrix);
						error(errors5[7]);
					}
					else {
						variable->name = operand0->str;
						variable->type = VarType_IntMatrix;
						variable->matrix = matrix_init((*(Result*)operand1).value, (*(Result*)operand2).value, VarType_Int);
						if (variable->matrix == NULL) {
							free(variable);
							error(errors5[7]);
						}
						addVariable(variable);
					}
				}
				else {
					error(errors5[12]);
				}
			free(operand0);
			free(operand1);
			free(operand2);
		}
		else {
			if (operand0 != NULL) {
				free(operand0);
			}
			if (operand1 != NULL) {
				free(operand1);
			}
			if (operand2 != NULL) {
				free(operand2);
			}
			error(errors5[7]);
		}
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		if (operand1 != NULL) {
			free(operand1);
		}
		if (operand2 != NULL) {
			free(operand2);
		}
		error(errors5[3]);
	}
}

void runFloatMatrixFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	Result* operand1 = runOperand(function->operands + 1);
	Result* operand2 = runOperand(function->operands + 2);
	if (isStr(operand0) && isNumber(operand1) && isNumber(operand2)) {
		if ((*(Result*)operand1).value > 0 && (*(Result*)operand2).value > 0) {
			if (findVariable(operand0->str) == NULL) {
				Variable_* variable = (Variable_*)malloc(sizeof(Variable_));
				if (variable == NULL) {
					matrix_free(emptyResult->matrix);
					error(errors5[7]);
				}
				else {
					variable->name = operand0->str;
					variable->type = VarType_FloatMatrix;
					variable->matrix = matrix_init((*(Result*)operand1).value, (*(Result*)operand2).value, VarType_Float);
					if (variable->matrix == NULL) {
						free(variable);
						error(errors5[7]);
					}
					addVariable(variable);
				}
			}
			else {
				error(errors5[12]);
			}
			free(operand0);
			free(operand1);
			free(operand2);
		}
		else {
			if (operand0 != NULL) {
				free(operand0);
			}
			if (operand1 != NULL) {
				free(operand1);
			}
			if (operand2 != NULL) {
				free(operand2);
			}
			error(errors5[7]);
		}
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		if (operand1 != NULL) {
			free(operand1);
		}
		if (operand2 != NULL) {
			free(operand2);
		}
		error(errors5[3]);
	}
}

void runResizeMatrixFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	Result* operand1 = runOperand(function->operands + 1);
	Result* operand2 = runOperand(function->operands + 2);
	if (isStr(operand0) && isNumber(operand1) && isNumber(operand2)) {
		if ((*(Result*)operand1).value > 0 && (*(Result*)operand2).value > 0) {
			Variable_* variable = findVariable(operand0->str);
			if (variable != NULL) {
					variable->matrix = matrix_resize(variable->matrix, (*(Result*)operand1).value, (*(Result*)operand2).value);
					if (variable->matrix == NULL) {
						error(errors5[7]);
					}
			}
			else {
				error(errors5[8]);
			}
			free(operand0);
			free(operand1);
			free(operand2);
		}
		else {
			if (operand0 != NULL) {
				free(operand0);
			}
			if (operand1 != NULL) {
				free(operand1);
			}
			if (operand2 != NULL) {
				free(operand2);
			}
			error(errors5[7]);
		}
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		if (operand1 != NULL) {
			free(operand1);
		}
		if (operand2 != NULL) {
			free(operand2);
		}
		error(errors5[3]);
	}
}

void runMatrixDetFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	if (isStr(operand0)) {
			Variable_* variable = findVariable(operand0->str);
			if (variable != NULL) {
				emptyResult->value = matrix_det(variable->matrix);
				if (emptyResult->value < 0) {
					error(errors5[3]);
				}
				else {
					emptyResult->type = ResultType_Float;
				}
			}
			else {
				error(errors5[8]);
			}
			free(operand0);
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		error(errors5[3]);
	}
}

void runMatrixTFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	if (isStr(operand0)) {
		Variable_* variable = findVariable(operand0->str);
		if (variable != NULL) {
			emptyResult->matrix = matrix_T(variable->matrix);
			if (emptyResult->matrix == NULL) {
				error(errors5[3]);
			}
			else {
				if (emptyResult->matrix->elementsType == VarType_Float) {
					emptyResult->type = ResultType_FloatMatrix;
				}
				else {
					emptyResult->type = ResultType_IntMatrix;
				}
			}
		}
		else {
			error(errors5[8]);
		}
		free(operand0);
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		error(errors5[3]);
	}
}

void runMatrixInverseFunc(Result* emptyResult, Function* function) {
	Result* operand0 = runOperand(function->operands);
	if (isStr(operand0)) {
		Variable_* variable = findVariable(operand0->str);
		if (variable != NULL) {
			emptyResult->matrix = matrix_inverse(variable->matrix);
			if (emptyResult->matrix == NULL) {
				error(errors5[3]);
			}
			else {
				if (emptyResult->matrix->elementsType == VarType_Float) {
					emptyResult->type = ResultType_FloatMatrix;
				}
				else {
					emptyResult->type = ResultType_IntMatrix;
				}
			}
		}
		else {
			error(errors5[8]);
		}
		free(operand0);
	}
	else {
		if (operand0 != NULL) {
			free(operand0);
		}
		error(errors5[3]);
	}
}

Result* getUndefinedResult() {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	result->type = ResultType_Undefined;
	return result;
}

Result* runStr(Operand* operand) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	char* data = (char*)((Operand*)operand)->data;
	result->str = data;
	result->type = ResultType_Str;
	return result;
}

Result* runVariable(Operand* operand) {
	char* name = (char*)((Operand*)operand)->data;
	Variable_* variable = findVariable(name);
	if (variable == NULL) {
		error2(errors5[8]);
		return NULL;
	}
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	if (variable->type == VarType_Int || variable->type == VarType_Float) {
		(*(Result*)result).value = (*(Variable_*)variable).value;
	}
	else if (variable->type == VarType_IntMatrix || variable->type == VarType_FloatMatrix) {
		((Result*)result)->matrix = ((Variable_*)variable)->matrix;
	}
	switch ((enum VarType)variable->type) {
	case VarType_Float:
		((Result*)result)->type = ResultType_Float;
		break;
	case VarType_FloatMatrix:
		((Result*)result)->type = ResultType_FloatMatrix;
		break;
	case VarType_Int:
		((Result*)result)->type = ResultType_Int;
		break;
	case VarType_IntMatrix:
		((Result*)result)->type = ResultType_IntMatrix;
		break;
	case VarType_Str:
		((Result*)result)->type = ResultType_Str;
		break;
	}
	return result;
}

Result* runNumber(Const* number) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	switch ((*(Const*)number).type) {
	case VarType_Float:
		(*(Result*)result).type = ResultType_Float;
		(*(Result*)result).value = *(double*)(*(Const*)number).data;
		break;
	case VarType_Int:
		(*(Result*)result).type = ResultType_Int;
		(*(Result*)result).value = *(double*)(*(Const*)number).data;
		break;
	default:
		free(result);
		return NULL;
	}
	return result;
}

Result* runLogicConst(Const* logicConst) {
	if ((*(Const*)logicConst).type == VarType_Logic) {
		Result* result = (Result*)malloc(sizeof(Result));
		if (result == NULL) {
			error2(errors5[0]);
			return NULL;
		}
		(*(Result*)result).type = ResultType_Logic;
		(*(Result*)result).value = *(short*)(*(Const*)logicConst).data;
		return result;
	}
	else {
		return NULL;
	}
}

Result* runMatrixElement(MatrixElement* element) {
	Variable_* variable = findVariable(element->matrixName);
	if (variable == NULL || variable->matrix == NULL) {
		error2(errors5[8]);
		return NULL;
	}
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	Result* row = runOperand(element->rowIndex);
	Result* column = runOperand(element->columnIndex);
	if (row == NULL || column == NULL || row->type != ResultType_Int || column->type != ResultType_Int || row->value < 0 || column->value < 0 || row->value >= (*variable->matrix).rowsCount || column->value >= (*variable->matrix).columnsCount) {
		error2(errors5[7]);
		return NULL;
	}
	(*result).value = variable->matrix->elements[(int)row->value][(int)row->value];
	(*result).type = variable->matrix->elementsType;
	if (row != NULL)
		free(row);
	if (column != NULL)
		free(column);
	return result;
}

Result* runOperand(Operand* operand) {
	switch ((*(Operand*)operand).type) {
	case OperandType_Arithmetic:
		return runArithmetic((Arithmetic*)operand->data);
	case OperandType_Function:
		return runFunction((Function*)operand->data);
	case OperandType_Number:
		return runNumber((Const*)operand->data);
	case OperandType_Unar:
		return runUnar((Operand*)operand->data);
	case OperandType_Variable:
		return runVariable((Operand*)operand);
	case OperandType_Str:
		return runStr((Operand*)operand);
	case OperandType_Inverse:
		return runInverse((Operand*)operand);
	case OperandType_LogicConst:
		return runLogicConst((Const*)operand->data);
	case OperandType_Logic:
		return runLogic((Logic*)operand->data);
	case OperandType_MatrixElement:
		return runMatrixElement((MatrixElement*)operand->data);
	case OperandType_MatrixInitializator:
		return runMatrixInitializator((MatrixInitializator*)operand->data);
	default:
		return NULL;
	}
}

Result* runUnar(Operand* operand) {
	Result* result = runOperand(operand);
	if (result != NULL) {
		if ((*(Result*)result).type == ResultType_Float) {
			(*(Result*)result).value = -(double)(*(Result*)result).value;
		}
		else if ((*(Result*)result).type == ResultType_Int) {
			(*(Result*)result).value = -(int)(*(Result*)result).value;
		}
		else {
			free(result);
			return NULL;
		}
	}
	return result;
}

Result* runInverse(Operand* operand) {
	Result* result = runOperand((Operand*)operand->data);
	if (result != NULL) {
		switch ((*(Result*)result).type) {
		case ResultType_Logic:
		case ResultType_Int:
		case ResultType_Float:
			(*(Result*)result).value = 1 - (int)(*(Result*)result).value;
			break;
		default:
			free(result);
			return NULL;
		}
	}
	return result;
}

Result* runFunction(Function* function) {
	(*environment).type = 3;
	(*environment).line = (*(Function*)function).line;
	(*environment).pos = (*(Function*)function).pos;
	(*environment).str = (*(Function*)function).name;
	Result* result;
	result = (Result*)malloc(sizeof(Result));
	memset(stringInput, 0, sizeof(stringInput));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	result->type = ResultType_Spec;
	result->value = 0.0;
	if ((*(Function*)function).count == 0) {
		if (!(strcmp((*(Function*)function).name, "help"))) {
			runHelpFunc(result);
		}
		else if (!(strcmp((*(Function*)function).name, "about"))) {
			runAboutFunc(result);
		}
		else if (!(strcmp((*(Function*)function).name, "lang"))) {
			runLangFunc(result);
		}
		else if (!(strcmp((*(Function*)function).name, "input"))) {
			runInputFunc(result, function, InputType_WithoutText);
		}
		else {
			error(errors5[2]);
		}
	}
	else if ((*(Function*)function).count == 1) {
		if (!(strcmp((*(Function*)function).name, "int"))) {
			runConvertTypeFunc(result, function, VarType_Int);
		}
		else if (!(strcmp((*(Function*)function).name, "float"))) {
			runConvertTypeFunc(result, function, VarType_Float);
		}
		else if (!(strcmp((*(Function*)function).name, "sqrt"))) {
			runSqrtFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "sin"))) {
			runTrigonomertyFunc(result, function, TrigonomertyFunc_Sin);
		}
		else if (!(strcmp((*(Function*)function).name, "cos"))) {
			runTrigonomertyFunc(result, function, TrigonomertyFunc_Cos);
		}
		else if (!(strcmp((*(Function*)function).name, "pow2"))) {
			runPow2Func(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "abs"))) {
			runAbsFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "file"))) {
			runFileFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "input"))) {
			runInputFunc(result, function, InputType_WithText);
		}
		else if (!(strcmp((*(Function*)function).name, "ceil"))) {
			runCeilFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "mod"))) {
			runModFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "det"))) {
			runMatrixDetFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "T"))) {
			runMatrixTFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "inverse"))) {
			runMatrixInverseFunc(result, function);
		}
		else {
			error(errors5[2]);
		}
	}
	else if ((*(Function*)function).count == 2) {
		if (!(strcmp((*(Function*)function).name, "pow"))) {
			runPowFunc(result, function);
		}
		else {
			error(errors5[2]);
		}
	}
	else if ((*(Function*)function).count == 3) {
		if (!(strcmp((*(Function*)function).name, "intMatrix"))) {
			runIntMatrixFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "floatMatrix"))) {
			runFloatMatrixFunc(result, function);
		}
		else if (!(strcmp((*(Function*)function).name, "resizeMatrix"))) {
			runResizeMatrixFunc(result, function);
		}
		else {
			error(errors5[2]);
		}
	}
	else {
		error(errors5[2]);
	}
	return result;
}

Result* runOperation(Result* one, Result* two, enum OpType type) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	double res;
	double oneDouble;
	double twoDouble;
	int oneInt;
	int twoInt;
	short oneIsFloat = 0;
	short twoIsFloat = 0;
	short IsNullDivided = 0;
	if (one->type == ResultType_Float) {
		oneDouble = (*(Result*)one).value;
		oneIsFloat = 1;
	}
	else {
		oneInt = (int)(*(Result*)one).value;
		oneIsFloat = 0;
	}
	if (two->type == ResultType_Float) {
		twoDouble = (*(Result*)two).value;
		twoIsFloat = 1;
	}
	else {
		twoInt = (int)(*(Result*)two).value;
		twoIsFloat = 0;
	}
	if (type == OpType_Div) {
		if ((isMatrix(one) && isMatrix(two)) || isMatrix(two)) {
			error2(errors5[11]);
			return NULL;
		}
		if (isMatrix(one)) {
			((Result*)result)->matrix = matrix_multiplication_withNumber(one->matrix, 1/(*two).value);
			if (((Result*)result)->matrix == NULL) {
				error2(errors5[10]);
				free(result);
				return NULL;
			}
			else {
				result->type = ResultType_FloatMatrix;
			}
			return result;
		}
		else if (oneIsFloat && twoIsFloat) {
			if (twoDouble != 0) {
				res = oneDouble / twoDouble;
			}
			else {
				IsNullDivided = 1;
			}
		}
		else if (!oneIsFloat && twoIsFloat) {
			if (twoDouble != 0) {
				res = oneInt / twoDouble;
			}
			else {
				IsNullDivided = 1;
			}
		}
		else if (oneIsFloat && !twoIsFloat) {
			if (twoInt != 0) {
				res = oneDouble / twoInt;
			}
			else {
				IsNullDivided = 1;
			}
		}
		else {
			if (twoInt != 0) {
				res = (double)oneInt / twoInt;
			}
			else {
				IsNullDivided = 1;
			}
		}
		if (IsNullDivided) {
			error2(errors5[9]);
			free(result);
			return NULL;
		}
		else {
			(*(Result*)result).value = res;
			result->type = ResultType_Float;
		}
		return result;
	}
	else if (type == OpType_Mul) {
		if (isMatrix(one) && isMatrix(two)) {
			((Result*)result)->matrix = matrix_multiplication_withMatrix(one->matrix, two->matrix);
			if (((Result*)result)->matrix == NULL) {
				error2(errors5[10]);
				free(result);
				return NULL;
			}
			else {
				result->type = ResultType_FloatMatrix;
			}
			return result;
		}
		else if (isMatrix(one) && (two->type == ResultType_Float || two->type == ResultType_Int)) {
			((Result*)result)->matrix = matrix_multiplication_withNumber(one->matrix, (*two).value);
			if (((Result*)result)->matrix == NULL) {
				error2(errors5[10]);
				free(result);
				return NULL;
			}
			else {
				result->type = ResultType_FloatMatrix;
			}
			return result;
		}
		else if (isMatrix(two) && (one->type == ResultType_Float || one->type == ResultType_Int)) {
			((Result*)result)->matrix = matrix_multiplication_withNumber(two->matrix, (*one).value);
			if (((Result*)result)->matrix == NULL) {
				error2(errors5[10]);
				free(result);
				return NULL;
			}
			else {
				result->type = ResultType_FloatMatrix;
			}
			return result;
		}
		else if (oneIsFloat && twoIsFloat) {
			res = oneDouble * twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt * twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble * twoInt;
		}
		else{
			res = oneInt * twoInt;
		}
		(*(Result*)result).value = res;
		result->type = ResultType_Float;
		return result;
	}
	else if (type == OpType_Sub) {
	if (isMatrix(one) && isMatrix(two)) {
	((Result*)result)->matrix = matrix_substraction(one->matrix, two->matrix);
		if (((Result*)result)->matrix == NULL) {
			error2(errors5[10]);
			free(result);
			return NULL;
		}
		else if ((((Result*)result)->matrix)->elementsType == VarType_Int) {
			result->type = ResultType_IntMatrix;
		}
		else {
			result->type = ResultType_FloatMatrix;
		}
		return result;
	}
	else if ((isMatrix(one) && !isMatrix(two)) || (!isMatrix(one) && isMatrix(two))) {
		error2(errors5[11]);
		return NULL;
	}
		else if (oneIsFloat && twoIsFloat) {
			res = oneDouble - twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt - twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble - twoInt;
		}
		else {
			res = oneInt - twoInt;
		}
		if (!oneIsFloat && !twoIsFloat) {
			(*(Result*)result).value = (int)res;
			result->type = ResultType_Int;
		}
		else {
			(*(Result*)result).value = res;
			result->type = ResultType_Float;
		}
		return result;
	}
	else if (type == OpType_Add) {
		if (isMatrix(one) && isMatrix(two)) {
			((Result*)result)->matrix = matrix_addition(one->matrix, two->matrix);
			if (((Result*)result)->matrix == NULL) {
				error2(errors5[10]);
				free(result);
				return NULL;
			}
			else if ((((Result*)result)->matrix)->elementsType == VarType_Int) {
				result->type = ResultType_IntMatrix;
			}
			else {
				result->type = ResultType_FloatMatrix;
			}
			return result;
		}
		else if ((isMatrix(one) && !isMatrix(two)) || (!isMatrix(one) && isMatrix(two))) {
			error2(errors5[11]);
			return NULL;
		}
		else if (oneIsFloat && twoIsFloat) {
			res = oneDouble + twoDouble;
		}
		else if (!oneIsFloat && twoIsFloat) {
			res = oneInt + twoDouble;
		}
		else if (oneIsFloat && !twoIsFloat) {
			res = oneDouble + twoInt;
		}
		else {
			res = oneInt + twoInt;
		}
		if (!oneIsFloat && !twoIsFloat) {
			(*(Result*)result).value = (int)res;
			result->type = ResultType_Int;
		}
		else {
			(*(Result*)result).value = res;
			result->type = ResultType_Float;
		}
		return result;
	}
}

Result* runLogicOperation(Result* one, Result* two, enum LOpType type) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	if ((one->type == ResultType_IntMatrix || one->type == ResultType_FloatMatrix) && (two->type == ResultType_IntMatrix || two->type == ResultType_FloatMatrix)) {
		if (type == LOpType_Equals) {
			(*(Result*)result).value = matrix_equals(one->matrix, two->matrix);
			(*(Result*)result).type = ResultType_Logic;
		}
		else if (type == LOpType_NotEquals) {
			(*(Result*)result).value = 1 - (int)matrix_equals(one->matrix, two->matrix);
			(*(Result*)result).type = ResultType_Logic;
		}
		else {
			error(errors5[11]);
			free(result);
			return NULL;
		}
		return result;
	}
	else {
		double oneDouble = (*(Result*)one).value;
		double twoDouble = (*(Result*)two).value;
		(*(Result*)result).type = ResultType_Logic;
		switch (type) {
		case LOpType_And:
			(*(Result*)result).value = (double)(oneDouble == twoDouble == 1.0);
			break;
		case LOpType_Or:
			(*(Result*)result).value = (double)(oneDouble == 1.0 || twoDouble == 1.0);
			break;
		case LOpType_Equals:
			(*(Result*)result).value = (double)(oneDouble == twoDouble);
			break;
		case LOpType_NotEquals:
			(*(Result*)result).value = (double)(oneDouble != twoDouble);
			break;
		case LOpType_More:
			(*(Result*)result).value = (double)(oneDouble > twoDouble);
			break;
		case LOpType_EqualsOrMore:
			(*(Result*)result).value = (double)(oneDouble >= twoDouble);
			break;
		case LOpType_Less:
			(*(Result*)result).value = (double)(oneDouble < twoDouble);
			break;
		case LOpType_EqualsOrLess:
			(*(Result*)result).value = (double)(oneDouble <= twoDouble);
			break;
		}
	}
	return result;
}

Result* runArithmetic(Arithmetic* arithmetic) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	cvector* operands = (cvector*)malloc(sizeof(cvector));
	if (operands == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	cvector* operations = (cvector*)malloc(sizeof(cvector));
	if (operations == NULL) {
		free(operands);
		error2(errors5[0]);
		return NULL;
	}
	cvector* expression = (cvector*)malloc(sizeof(cvector));
	if (expression == NULL) {
		free(operands);
		free(operations);
		error2(errors5[0]);
		return NULL;
	}
	cvector_init(expression);
	cvector_init(operands);
	cvector_init(operations);
	for (int j = 0; j < (*(Arithmetic*)arithmetic).operandsCount; j++) {
		cvector_push_back(operands, runOperand((Operand*)arithmetic->operands + j));
		if ((Result*)cvector_get(operands, j) == NULL || ((Result*)cvector_get(operands, j))->type == ResultType_Spec || ((Result*)cvector_get(operands, j))->type == ResultType_Str) {
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			return NULL;
		}
	}
	for (int j = 0; j < (*(Arithmetic*)arithmetic).operationsCount; j++) {
		cvector_push_back(operations, (enum OpType*)arithmetic->operations + j);
	}
	int i = 0;
	if (cvector_size(operands) != 1) {
		cvector_push_back(expression, cvector_get(operands, i));
		while (i < cvector_size(operands) - 1) {
			cvector_push_back(expression, cvector_get(operations, i));
			cvector_push_back(expression, cvector_get(operands, i + 1));
			i++;
		}
		i = 0;
		while (i < cvector_size(expression) - 2) {
			if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Mul) {
				Result* result = runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Mul);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Div) {
				Result* result = runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Div);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else {
				i += 2;
			}
		}
		i = 0;
		while (i < cvector_size(expression) - 2) {
			if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Sub) {
				Result* result = runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Sub);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum OpType*)cvector_get(expression, i + 1)) == OpType_Add) {
				Result* result = runOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), OpType_Add);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
		}
	}
	else {
		result = cvector_get(operands, 0);
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		return result;
	}
	result = cvector_get(expression, 0);
	cvector_free(operands);
	free(operands);
	cvector_free(operations);
	free(operations);
	cvector_free(expression);
	free(expression);
	return result;
}

Result* runLogic(Logic* logic) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	cvector* operands = (cvector*)malloc(sizeof(cvector));
	if (operands == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	cvector* operations = (cvector*)malloc(sizeof(cvector));
	if (operations == NULL) {
		free(operands);
		error2(errors5[0]);
		return NULL;
	}
	cvector* expression = (cvector*)malloc(sizeof(cvector));
	if (expression == NULL) {
		free(operands);
		free(operations);
		error2(errors5[0]);
		return NULL;
	}
	cvector_init(expression);
	cvector_init(operands);
	cvector_init(operations);
	for (int j = 0; j < (*(Logic*)logic).operandsCount; j++) {
		cvector_push_back(operands, runOperand((Operand*)logic->operands + j));
		if ((Result*)cvector_get(operands, j) == NULL || ((Result*)cvector_get(operands, j))->type == ResultType_Spec || ((Result*)cvector_get(operands, j))->type == ResultType_Str) {
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			return NULL;
		}
	}
	for (int j = 0; j < (*(Logic*)logic).operationsCount; j++) {
		cvector_push_back(operations, (enum LOpType*)logic->operations + j);
	}
	int i = 0;
	if (cvector_size(operands) != 1) {
		cvector_push_back(expression, cvector_get(operands, i));
		while (i < cvector_size(operands) - 1) {
			cvector_push_back(expression, cvector_get(operations, i));
			cvector_push_back(expression, cvector_get(operands, i + 1));
			i++;
		}
		i = 0;
		while (i < cvector_size(expression) - 2) {
			if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_And) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_And);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_Or) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_Or);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_Equals) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_Equals);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_NotEquals) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_NotEquals);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_More) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_More);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_EqualsOrMore) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_EqualsOrMore);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_Less) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_Less);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else if ((*(enum LOpType*)cvector_get(expression, i + 1)) == LOpType_EqualsOrLess) {
				Result* result = runLogicOperation((Result*)cvector_get(expression, i), (Result*)cvector_get(expression, 2 + i), LOpType_EqualsOrLess);
				if (result == NULL) {
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					return result;
				}
				cvector_set(expression, i, result);
				vector_delete(expression, i + 1);
				vector_delete(expression, i + 1);
			}
			else {
				i += 2;
			}
		}
	}
	else {
		result = cvector_get(operands, 0);
		result->type = ResultType_Logic;
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		return result;
	}
	result = cvector_get(expression, 0);
	result->type = ResultType_Logic;
	cvector_free(operands);
	free(operands);
	cvector_free(operations);
	free(operations);
	cvector_free(expression);
	free(expression);
	return result;
}

Result* runMatrixInitializator(MatrixInitializator* matrix_Init) {
	Result* result = (Result*)malloc(sizeof(Result));
	if (result == NULL) {
		error2(errors5[0]);
		return NULL;
	}
	Matrix* newMatrix = matrix_init_undefinedElementsType(matrix_Init->rowsCount, (matrix_Init->rows[0])->columnsCount);
	Result* element;
	short isFirstElement = 1;
	for (int i = 0; i < matrix_Init->rowsCount; i++) {
		for (int j = 0; j < (matrix_Init->rows[0])->columnsCount; j++) {
			if ((matrix_Init->rows[i])->columnsCount < (matrix_Init->rows[0])->columnsCount) {
				result->type = ResultType_Undefined;
				matrix_free(newMatrix);
				return result;
			}
			element = runOperand(matrix_Init->rows[i] + j);
			if (element == NULL) {
				result->type = ResultType_Undefined;
				matrix_free(newMatrix);
				return result;
			}
			if (isFirstElement) {
				if (element->type == ResultType_Int)
				{
					newMatrix->elementsType = VarType_Int;
					result->type = ResultType_IntMatrix;
				}
				else if (element->type == ResultType_Float)
				{
					newMatrix->elementsType = VarType_Float;
					result->type = ResultType_FloatMatrix;
				}
				isFirstElement = 0;
			}
			matrix_setElement(newMatrix, i, j, element->value);
		}
	}
	result->matrix = newMatrix;
	return result;
}

void runSetting(Part* part) {
	Variable_* variable = findVariable((char*)(*(Operand*)((Setting*)part->data)->variable).data);
	if (variable == NULL) {
		variable = (Variable_*)malloc(sizeof(Variable_));
		if (variable == NULL) {
			error2(errors5[0]);
			return;
		}
		if ((*(Operand*)((Setting*)part->data)->variable).type == OperandType_MatrixElement) {
			variable->name = (char*)((MatrixElement*)((Operand*)((Setting*)part->data)->variable)->data)->matrixName;
		}
		else {
			variable->name = (char*)(*(Operand*)((Setting*)part->data)->variable).data;
		}
	}
	else if(variable->isSystem == 1){
		error2(errors5[13]);
		return;
	}
	Result* result = runOperand((Operand*)((Setting*)part->data)->source);
	if (result != NULL) {
		switch ((*(Result*)result).type) {
		case ResultType_Float:
			if ((*(Operand*)((Setting*)part->data)->variable).type == OperandType_MatrixElement) {
				Variable_* var = findVariable(variable->name);
				if (var == NULL) {
					error2(errors5[8]);
					return;
				}
				else if (var != NULL && (*var).type != VarType_FloatMatrix) {
					error2(errors5[6]);
					return;
				}
				else if (var != NULL && (*var).type == VarType_FloatMatrix) {
					Result* row = runOperand(((MatrixElement*)((Operand*)((Setting*)part->data)->variable)->data)->rowIndex);
					Result* column = runOperand(((MatrixElement*)((Operand*)((Setting*)part->data)->variable)->data)->columnIndex);
					if (row != NULL && row->type == ResultType_Int && row->value >= 0 && column != NULL && column->type == ResultType_Int && column->value >= 0 && row->value < var->matrix->rowsCount && column->value < var->matrix->columnsCount) {
						((Variable_*)var)->matrix = matrix_setElement(((Variable_*)var)->matrix, (int)row->value, (int)column->value, (*(Result*)result).value);
					}
					else {
						error2(errors5[7]);
						return;
					}
					if (row != NULL)
						free(row);
					if (column != NULL)
						free(column);
					return;
				}
			}
			else {
				variable->type = VarType_Float;
				(*(Variable_*)variable).value = (*(Result*)result).value;
			}
			break;
		case ResultType_Int:
			if ((*(Operand*)((Setting*)part->data)->variable).type == OperandType_MatrixElement) {
				Variable_* var = findVariable(variable->name);
				if (var == NULL) {
					error2(errors5[8]);
					return;
				}
				else if (var != NULL && (*var).type != VarType_IntMatrix) {
					error2(errors5[6]);
					return;
				}
				else if (var != NULL && (*var).type == VarType_IntMatrix) {
					Result* row = runOperand(((MatrixElement*)((Operand*)((Setting*)part->data)->variable)->data)->rowIndex);
					Result* column = runOperand(((MatrixElement*)((Operand*)((Setting*)part->data)->variable)->data)->columnIndex);
					if (row != NULL && row->type == ResultType_Int && row->value >= 0 && column != NULL && column->type == ResultType_Int && column->value >= 0 && row->value < var->matrix->rowsCount && column->value < var->matrix->columnsCount) {
						((Variable_*)var)->matrix = matrix_setElement(((Variable_*)var)->matrix, (int)row->value, (int)column->value, (*(Result*)result).value);
					}
					else {
						error2(errors5[7]);
						return;
					}
					if (row != NULL)
						free(row);
					if (column != NULL)
						free(column);
					return;
				}
			}
			else {
				variable->type = VarType_Int;
				(*(Variable_*)variable).value = (*(Result*)result).value;
			}
			break;
		case ResultType_IntMatrix:
			variable->type = VarType_IntMatrix;
			variable->matrix = matrix_copy(((Result*)result)->matrix);
			break;
		case ResultType_FloatMatrix:
			variable->type = VarType_FloatMatrix;
			variable->matrix = matrix_copy(((Result*)result)->matrix);
			break;
		default:
			return;
		}
		addVariable(variable);
	}
}

void runDel(Part* part) {
	Variable_* variable = findVariable((*(Operand*)part->data).data);
	deleteVariable(variable);
}

short runIfElif(Part* part) {
	Logic_Construction* logic_construction = part->data;
	Result* result = runLogic(logic_construction->condition);
	if ((*(Result*)result).type == ResultType_Logic && (*(Result*)result).value == 1.0) {
		run(logic_construction->expression);
	}
	return (short)(*(Result*)result).value;
}

void runElse(Part* part) {
	Logic_Construction* logic_construction = part->data;
	run(logic_construction->expression);
}

void runRepeat(Part* part) {
	Logic_Construction* logic_construction = part->data;
	Result* result = runLogic(logic_construction->condition);
	while ((*(Result*)result).type == ResultType_Logic && (*(Result*)result).value == 1.0) {
		for (int i = 0; i < ((Expression*)logic_construction->expression)->count; i++) {
			runPart(((Expression*)logic_construction->expression)->parts + i);
		}
		result = runLogic(logic_construction->condition);
	}
	freeExpression(logic_construction->expression);
}

void runNewLine() {
	printf("\n");
}

void runPrintStr(Part* part) {
	printf("%s", part->data);
}

int runMatrixIf(Part* part) {
	if (part->type == PartType_Arithmetic) {
		Arithmetic* arithmetic = ((Operand*)part->data)->data;
		if (arithmetic->operandsCount == 1) {
			Operand operand = arithmetic->operands[0];
			if (operand.type == OperandType_Variable) {
				Variable_* var = findVariable(operand.data);
				if (var != NULL && var->matrix != NULL) {
					Result* result = malloc(sizeof(Result));
					if (result == NULL) {
						error2(errors5[0]);
						return 0;
					}
					if (var->type == VarType_FloatMatrix || var->type == VarType_IntMatrix) {
						result->matrix = var->matrix;
						if (result->matrix->elementsType == VarType_Float) {
							result->type = ResultType_FloatMatrix;
						}
						else {
							result->type = ResultType_IntMatrix;
						}
					}
					else {
						return 0;
					}
					printResult(result);
					return 1;
				}
				return 0;
			}
			return 0;
		}
		return 0;
	}
	return 0;
}

void runPart(Part* part) {
	Result* result;
	switch (part->type) {
	case PartType_Set:
		runSetting(part);
		break;
	case PartType_Del:
		runDel(part);
		break;
	case PartType_Arithmetic:
	case PartType_Logic:
		if (runMatrixIf(part)) {
			break;
		}
		result = runOperand((Operand*)part->data);
		if (result != NULL) {
			printResult(result);
		}
		break;
	case PartType_NewLine:
		runNewLine();
		break;
	case PartType_Str:
		runPrintStr(part);
		break;
	case PartType_Logic_Construction:
		if ((*(Logic_Construction*)part->data).logicPartType == LogicPartType_If) {
			value = runIfElif(part);
		}
		else if ((*(Logic_Construction*)part->data).logicPartType == LogicPartType_Elif && !value) {
			value = runIfElif(part);
		}
		else if ((*(Logic_Construction*)part->data).logicPartType == LogicPartType_Else && !value) {
			runElse(part);
		}
		else if ((*(Logic_Construction*)part->data).logicPartType == LogicPartType_Repeat) {
			runRepeat(part);
		}
		break;
	}
}

short initTablesAndVariables() {
	variables = (cvector*)malloc(sizeof(cvector));
	if (variables == NULL) {
		error2(errors5[0]);
		return 0;
	}
	cvector_init(variables);
	Variable_* PI = malloc(sizeof(Variable_));
	if (PI == NULL) {
		error2(errors5[0]);
		return 0;
	}
	PI->name = "PI";
	PI->type = VarType_Float;
	PI->value = 3.1415926535;
	PI->isSystem = 1;
	cvector_push_back(variables, PI);
	Variable_* E = malloc(sizeof(Variable_));
	if (E == NULL) {
		error2(errors5[0]);
		return 0;
	}
	E->name = "E";
	E->type = VarType_Float;
	E->value = 2.7182818284;
	E->isSystem = 1;
	cvector_push_back(variables, E);
	stringInput = malloc(sizeof(char) * MAX_EXPRESSION);
	memset(stringInput, 0, sizeof(stringInput));
	return 1;
}

void run(Expression* expression) {
	environment = getCurrent();
	(*environment).type = 2;
	for (int i = 0; i < expression->count; i++) {
		runPart(expression->parts + i);
	}
	freeExpression(expression);
}

void freeTables() {
	cvector_free(variables);
	free(variables);
}