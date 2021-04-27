/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "parser.h"

int ERROR_CODE = 0;
int IF_TYPE = 0;
short IS_ERROR = 0;
Environment* environment;
char* forStr;
const char* errors3[] = {
	"при парсинге произошла ошибка [код: 3.0].",
	"неожиданный конец выражения [код: 3.1].",
	"ожидалась операция удаления [код: 3.2].",
	"ожидались название переменной или элемент матрицы [код: 3.3].",
	"ожидался символ \',\' или конец выражения [код: 3.4].",
	"ожидался символ \'=\' [код: 3.5].",
	"ошибка при получении числа [код: 3.6].",
	"ожидалось название функции [код: 3.7].",
	"неожиданный символ [код: 3.8].",
	"ожидалось число [код: 3.9].",
	"ошибка выделения памяти [код: 3.10].",
	"строка не может быть частью арифметического выражения [код: 3.11].",
	"индекс элемента массив должен быть целым числом [код: 3.12]."
};

void freeExpression(Expression* expression);
void freeArithmetic(Arithmetic* arithmetic);
void freeLogic(Logic* logic);
void freeOperand(Operand* operand);

Operand* GetOperand(Array* array, int* index);
Operand* GetArithmetic(Array* array, int* index);
Operand* GetLogic(Array* array, int* index);
Expression* GetExpression(Array* array, int* index, int isBody);

void freeUnar(Operand* operand) {
	freeOperand(operand);
}

void freeNumber(Const* number) {
	if (number->data)
		free(number->data);
	free(number);
}

void freeFunction(Function* function) {
	freeOperand(function->operands);
	if (function)
		free(function);
}

void freeMatrixRow(MatrixRow* row) {
	free(row);
}

void freeMatrixInitializator(MatrixInitializator* m) {
	for (int i = 0; i < m->rowsCount; i++) {
		freeMatrixRow(m->rows[i]);
	}
	free(m);
}

void freeOperand(Operand* operand) {
	switch (operand->type) {
	case OperandType_Str:
		break;
	case OperandType_Logic:
		freeLogic(operand->data);
		break;
	case OperandType_Arithmetic:
		freeArithmetic(operand->data);
		break;
	case OperandType_Function:
		freeFunction(operand->data);
		break;
	case OperandType_Inverse:
		freeOperand(operand->data);
		break;
	case OperandType_LogicConst:
	case OperandType_Number:
		break;
	case OperandType_Unar:
		freeOperand(operand->data);
		break;
	case OperandType_MatrixInitializator:
		freeMatrixInitializator(operand->data);
		break;
	}
	free(operand);
}

void freeArithmetic(Arithmetic* arithmetic) {
	freeOperand(arithmetic->operands);
	if (arithmetic)
		free(arithmetic);
}

void freeLogic(Logic* logic) {
	freeOperand(logic->operands);
	if (logic)
		free(logic);
}

void freeDel(char* name) {
	if (name)
		free(name);
}

void freeSet(Setting* set) {
	if (set->variable)
		freeOperand(set->variable);
	if (set->source)
		freeOperand(set->source);
	free(set);
}

void freeLogicConstruction(Logic_Construction* logic_construction) {
	if (logic_construction->condition)
		freeLogic(logic_construction->condition);
	free(logic_construction);
}

void freePart(Part* part) {
	switch (part->type) {
	case PartType_Arithmetic:
		if (part->data)
			freeArithmetic(part->data);
		break;
	case PartType_Logic:
		if (part->data)
			freeLogic(part->data);
		break;
	case PartType_Logic_Construction:
		if (part->data)
			freeLogicConstruction(part->data);
		break;
	case PartType_Del:
		if (part->data)
			freeDel(part->data);
		break;
	case PartType_Set:
		if (part->data)
			freeSet(part->data);
		break;
	case PartType_Str:
		if (part->data)
			free(part->data);
		break;
	}
}

void freeExpression(Expression* expression) {
	for (int i = 0; i < expression->count; i++) {
		freePart(expression->parts + i);
	}
	free(expression);
}

enum TokenType GetType(Array* array, int* index) {
	return (*(Token*)(((Token*)array->data) + *index)).type;
}

void AddIndex(int* index, Array* array, int value) {
	(*index) += value;
	while ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_NewLine) {
		(*index) += value;
	}
	if ((*environment).isFile == 1) {
		if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Str) {
			forStr = malloc((strlen(((Token*)(((Token*)array->data) + *index))->data) + 2) * sizeof(char));
			if (forStr == NULL) {
				error2(errors3[10]);
			}
			else {
				forStr[0] = '\'';
				for (int i = 0; i < strlen(((Token*)(((Token*)array->data) + *index))->data); i++) {
					forStr[i + 1] = *(((Token*)(((Token*)array->data) + *index))->data + i);
				}
				forStr[strlen(((Token*)(((Token*)array->data) + *index))->data) - 2] = '\'';
			}
		}
		else {
			environment->str = ((Token*)(((Token*)array->data) + *index))->data;
		}
	}
	environment->pos = ((Token*)(((Token*)array->data) + *index))->coord;
	environment->line = ((Token*)(((Token*)array->data) + *index))->line;
}

enum OpType* GetOperation(Token* token) {
	enum OpType* operation = (enum OpType*)malloc(sizeof(enum OpType));
	if (token->data == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*token).type == TokenType_OpMul) {
		*operation = OpType_Mul;
	}
	else if ((*token).type == TokenType_OpDiv) {
		*operation = OpType_Div;
	}
	else if ((*token).type == TokenType_OpSub) {
		*operation = OpType_Sub;
	}
	else if ((*token).type == TokenType_OpAdd) {
		*operation = OpType_Add;
	}
	else {
		free(operation);
		return NULL;
	}
	if ((*environment).isFile == 1) {
		environment->str = token->data;
	}
	environment->pos = token->coord;
	environment->line = token->line;
	return operation;
}

enum LOpType* GetLogicOperation(Token* token) {
	enum LOpType* operation = (enum LOpType*)malloc(sizeof(enum LOpType));
	if (token->data == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*token).type == TokenType_And) {
		*operation = LOpType_And;
	}
	else if ((*token).type == TokenType_Or) {
		*operation = LOpType_Or;
	}
	else if ((*token).type == TokenType_Equals) {
		*operation = LOpType_Equals;
	}
	else if ((*token).type == TokenType_NotEquals) {
		*operation = LOpType_NotEquals;
	}
	else if ((*token).type == TokenType_More) {
		*operation = LOpType_More;
	}
	else if ((*token).type == TokenType_EqualsOrMore) {
		*operation = LOpType_EqualsOrMore;
	}
	else if ((*token).type == TokenType_Less) {
		*operation = LOpType_Less;
	}
	else if ((*token).type == TokenType_EqualsOrLess) {
		*operation = LOpType_EqualsOrLess;
	}
	if ((*environment).isFile == 1) {
		environment->str = token->data;
	}
	environment->pos = token->coord;
	environment->line = token->line;
	return operation;
}

Const* GetNumber2(Array* array, int* index) {
	Const* number = (Const*)malloc(sizeof(Const));
	if (number == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*environment).isFile == 1) {
		environment->str = ((Token*)(((Token*)array->data) + *index))->data;
	}
	environment->pos = ((Token*)(((Token*)array->data) + *index))->coord;
	environment->line = ((Token*)(((Token*)array->data) + *index))->line;
	if (GetType(array, index) == TokenType_Int) {
		number->data = (int*)malloc(sizeof(int));
		if (number->data == NULL) {
			free(number);
			ERROR_CODE = 10;
			return NULL;
		}
		*(double*)((*(Const*)number).data) = atoi((*(Token*)(((Token*)array->data) + *index)).data);
		number->type = VarType_Int;
		return number;
	}
	else if (GetType(array, index) == TokenType_Float) {
		number->data = (double*)malloc(sizeof(double));
		if (number->data == NULL) {
			free(number);
			ERROR_CODE = 10;
			return NULL;
		}
		*((double*)(*(Const*)number).data) = atof((*(Token*)(((Token*)array->data) + *index)).data);
		number->type = VarType_Float;
		return number;
	}
	else {
		ERROR_CODE = 9;
		free(number);
		return NULL;
	}
}

Const* GetLogicConst(Array* array, int* index) {
	Const* logicConst = (Const*)malloc(sizeof(Const));
	if (logicConst == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*environment).isFile == 1) {
		environment->str = ((Token*)(((Token*)array->data) + *index))->data;
	}
	environment->pos = ((Token*)(((Token*)array->data) + *index))->coord;
	environment->line = ((Token*)(((Token*)array->data) + *index))->line;
	if (GetType(array, index) == TokenType_False) {
		logicConst->data = (short*)malloc(sizeof(short));
		if (logicConst->data == NULL) {
			free(logicConst);
			ERROR_CODE = 10;
			return NULL;
		}
		*((short*)(*(Const*)logicConst).data) = 0;
		logicConst->type = VarType_Logic;
	}
	else if (GetType(array, index) == TokenType_True) {
		logicConst->data = (short*)malloc(sizeof(short));
		if (logicConst->data == NULL) {
			free(logicConst);
			ERROR_CODE = 10;
			return NULL;
		}
		*((short*)(*(Const*)logicConst).data) = 1;
		logicConst->type = VarType_Logic;
	}
	else {
		ERROR_CODE = 8;
		free(logicConst);
		return NULL;
	}
	return logicConst;
}

Operand* GetMatrixElement(Array* array, int* index) {
	Operand* operand = (Operand*)malloc(sizeof(Operand));
	if (operand == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	int new_index = *index + 1;
	if (GetType(array, index) != TokenType_Name && GetType(array, &new_index) != TokenType_LeftSquareBracket) {
		ERROR_CODE = 8;
		free(operand);
		return NULL;
	}
	char* matrixName = ((Token*)(((Token*)array->data) + *index))->data;
	AddIndex(index, array, 1);
	if (GetType(array, index) == TokenType_End) {
		ERROR_CODE = 8;
		free(operand);
		return NULL;
	}
	AddIndex(index, array, 1);
	Operand* row = GetArithmetic(array, index);
	if (row == NULL) {
		free(operand);
		ERROR_CODE = 8;
		return NULL;
	}
	if (GetType(array, index) != TokenType_Sep) {
		free(operand);
		free(row);
		ERROR_CODE = 8;
		return NULL;
	}
	AddIndex(index, array, 1);
	Operand* column = GetArithmetic(array, index);
	if (column == NULL) {
		free(operand);
		free(row);
		ERROR_CODE = 8;
		return NULL;
	}
	if (GetType(array, index) != TokenType_RightSquareBracket) {
		free(operand);
		free(column);
		free(row);
		ERROR_CODE = 8;
		return NULL;
	}
	MatrixElement* m_e = (MatrixElement*)malloc(sizeof(MatrixElement));
	if (m_e == NULL) {
		free(operand);
		free(column);
		free(row);
		ERROR_CODE = 10;
		return NULL;
	}
	m_e->matrixName = matrixName;
	m_e->rowIndex = row;
	m_e->columnIndex = column;
	operand->type = OperandType_MatrixElement;
	operand->data = m_e;
	AddIndex(index, array, 1);
	return operand;
}

Operand* GetVariable(Array* array, int* index) {
	Operand* operand = (Operand*)malloc(sizeof(Operand));
	if (operand == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*environment).isFile == 1) {
		environment->str = ((Token*)(((Token*)array->data) + *index))->data;
	}
	environment->pos = ((Token*)(((Token*)array->data) + *index))->coord;
	environment->line = ((Token*)(((Token*)array->data) + *index))->line;
	if (GetType(array, index) == TokenType_Name) {
		(*(Operand*)operand).data = (char*)(*(Token*)(((Token*)array->data) + *index)).data;
		(*(Operand*)operand).type = OperandType_Variable;
		return operand;
	}
	else {
		ERROR_CODE = 8;
		free(operand);
		return NULL;
	}
}

Operand* GetFunction(Array* array, int* index) {
	Operand* operand;
	Function* function;
	cvector* args = (cvector*)malloc(sizeof(cvector));
	if (args == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*environment).isFile == 1) {
		environment->str = ((Token*)(((Token*)array->data) + *index))->data;
	}
	environment->pos = ((Token*)(((Token*)array->data) + *index))->coord;
	environment->line = ((Token*)(((Token*)array->data) + *index))->line;
	cvector_init(args);
	int new_index = *index + 1;
	enum TokenType next = GetType(array, &new_index);
	if (GetType(array, index) == TokenType_Name && next == TokenType_LeftBracket) {
		function = (Function*)malloc(sizeof(Function));
		if (function == NULL) {
			cvector_free(args);
			free(args);
			ERROR_CODE = 10;
			return NULL;
		}
		function->name = (char*)(*(Token*)(((Token*)array->data) + *index)).data;
		function->line = environment->line;
		function->pos = environment->pos;
		AddIndex(index, array, 2);
		Operand* arg;
		if ((*(((Token*)array->data) + *index)).type != TokenType_Str) {
			arg = GetArithmetic(array, index);
		}
		else {
			arg = GetOperand(array, index);
		}
		if (arg != NULL) {
			cvector_push_back(args, arg);
			while (GetType(array, index) != TokenType_RightBracket) {
				if (GetType(array, index) == TokenType_Sep) {
					AddIndex(index, array, 1);
				}
				while (GetType(array, index) == TokenType_NewLine) {
					AddIndex(index, array, 1);
				}
				environment->str = ((Token*)(((Token*)array->data) + *index))->data;
				if (GetType(array, index) == TokenType_RightBracket) {
					ERROR_CODE = 8;
					freeOperand(arg);
					free(function);
					cvector_free(args);
					free(args);
					return NULL;
				}
				if (GetType(array, index) != TokenType_Str) {
					arg = GetArithmetic(array, index);
				}
				else {
					arg = GetOperand(array, index);
				}
				if (arg != NULL) {
					cvector_push_back(args, arg);
				}
				else {
					cvector_free(args);
					free(args);
					free(function);
					return NULL;
				}
			}
			(*function).count = cvector_size(args);
			function->operands = (Operand*)malloc(sizeof(Operand) * cvector_size(args));
			if (function->operands == NULL) {
				freeOperand(arg);
				free(function);
				cvector_free(args);
				free(args);
				ERROR_CODE = 10;
				return NULL;
			}
			for (int i = 0; i < cvector_size(args); i++) {
				function->operands[i] = *(Operand*)cvector_get(args, i);
			}
			operand = (Operand*)malloc(sizeof(Operand));
			if (operand == NULL) {
				freeOperand(arg);
				free(function);
				cvector_free(args);
				free(args);
				ERROR_CODE = 10;
				return NULL;
			}
			operand->data = function;
			operand->type = OperandType_Function;
			AddIndex(index, array, 1);
			cvector_free(args);
			return operand;
		}
		else if (arg == NULL && GetType(array, index) == TokenType_RightBracket) {
			function->count = 0;
			function->operands = NULL;
			operand = (Operand*)malloc(sizeof(Operand));
			if (operand == NULL) {
				free(function);
				cvector_free(args);
				free(args);
				ERROR_CODE = 10;
				return NULL;
			}
			operand->data = function;
			operand->type = OperandType_Function;
			cvector_free(args);
			free(args);
			AddIndex(index, array, 1);
			return operand;
		}
		else {
			ERROR_CODE = 8;
			for (int i = 0; i < cvector_size(args); i++) {
				freeOperand(cvector_get(args, i));
			}
			free(function);
			cvector_free(args);
			free(args);
			return NULL;
		}
	}
	else {
		cvector_free(args);
		free(args);
		ERROR_CODE = 7;
		return NULL;
	}
}

Operand* GetOperand(Array* array, int* index) {
	Operand* operand;
	if (GetType(array, index) == TokenType_OpSub) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		AddIndex(index, array, 1);
		Operand* p = GetOperand(array, index);
		if (p == NULL) {
			free(operand);
			return NULL;
		}
		operand->data = p;
		operand->type = OperandType_Unar;
		return operand;
	}
	else if (GetType(array, index) == TokenType_Inverse) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		AddIndex(index, array, 1);
		Operand* p = GetOperand(array, index);
		if (p == NULL) {
			free(operand);
			return NULL;
		}
		operand->data = p;
		operand->type = OperandType_Inverse;
		return operand;
	}
	else if (GetType(array, index) == TokenType_False || GetType(array, index) == TokenType_True) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		Const* p = GetLogicConst(array, index);
		if (p == NULL) {
			free(operand);
			return NULL;
		}
		operand->data = p;
		operand->type = OperandType_LogicConst;
		AddIndex(index, array, 1);
		return operand;
	}
	else if (GetType(array, index) == TokenType_Name) {
		int lastIndex = *index;
		operand = GetMatrixElement(array, index);
		if (operand == NULL) {
			AddIndex(index, array, -(*index - lastIndex));
			operand = GetFunction(array, index);
			if (operand == NULL) {
				AddIndex(index, array, -(*index - lastIndex));
				operand = GetVariable(array, index);
				if (operand == NULL) {
					return NULL;
				}
				AddIndex(index, array, 1);
			}
		}

		return operand;
	}
	else if (GetType(array, index) == TokenType_Int || GetType(array, index) == TokenType_Float) {
		Const* number = NULL;
		number = GetNumber2(array, index);
		AddIndex(index, array, 1);
		if (number == NULL) {
			ERROR_CODE = 6;
			return NULL;
		}
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			freeNumber(number);
			ERROR_CODE = 10;
			return NULL;
		}
		operand->data = number;
		operand->type = OperandType_Number;
		return operand;
	}
	else if (GetType(array, index) == TokenType_LeftBracket) {
		AddIndex(index, array, 1);
		int lastIndex = *index;
		operand = GetArithmetic(array, index);
		if (operand == NULL && GetType(array, index) != TokenType_RightBracket) {
			return NULL;
		}
		else if (operand == NULL) {
			AddIndex(index, array, -(*index - lastIndex));
			operand = GetLogic(array, index);
			if (operand == NULL && GetType(array, index) != TokenType_RightBracket) {
				return NULL;
			}
		}
		AddIndex(index, array, 1);
		return operand;
	}
	else if (GetType(array, index) == TokenType_RightBracket) {
		return NULL;
	}
	else if (GetType(array, index) == TokenType_Str) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		operand->data = ((Token*)(((Token*)array->data) + *index))->data;
		operand->type = OperandType_Str;
		AddIndex(index, array, 1);
		return operand;
	}
	else {
		ERROR_CODE = 8;
		return NULL;
	}
}

Operand* GetArithmetic(Array* array, int* index) {
	Operand* operand;
	Arithmetic* arithmetic = (Arithmetic*)malloc(sizeof(Arithmetic));
	if (arithmetic == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	cvector* operands = (cvector*)malloc(sizeof(cvector));
	if (operands == NULL) {
		free(arithmetic);
		ERROR_CODE = 10;
		return NULL;
	}
	cvector* operations = (cvector*)malloc(sizeof(cvector));
	if (operations == NULL) {
		free(arithmetic);
		free(operands);
		ERROR_CODE = 10;
		return NULL;
	}
	Operand* op = GetOperand(array, index);
	if (op != NULL) {
		if (op->type == OperandType_Str) {
			ERROR_CODE = 11;
			freeOperand(op);
			free(operands);
			free(operations);
			free(arithmetic);
			return NULL;
		}
	}
	cvector_init(operands);
	cvector_init(operations);
	if (op != NULL) {
		cvector_push_back(operands, op);
		while (GetType(array, index) == TokenType_OpDiv || GetType(array, index) == TokenType_OpMul || GetType(array, index) == TokenType_OpSub || GetType(array, index) == TokenType_OpAdd) {
			cvector_push_back(operations, GetOperation(((Token*)array->data) + *index));
			AddIndex(index, array, 1);
			op = GetOperand(array, index);
			if (op != NULL) {
				if (op->type == OperandType_Str) {
					ERROR_CODE = 11;
					freeOperand(op);
					for (int i = 0; i < cvector_size(operands); i++) {
						freeOperand(cvector_get(operands, i));
					}
					for (int i = 0; i < cvector_size(operations); i++) {
						free((enum OpType*)cvector_get(operations, i));
					}
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					free(arithmetic);
					return NULL;
				}
				cvector_push_back(operands, op);
			}
			else {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum OpType*)cvector_get(operations, i));
				}
				cvector_free(operands);
				free(operands);
				cvector_free(operations);
				free(operations);
				free(arithmetic);
				return NULL;
			}

		}
		if (GetType(array, index) == TokenType_And || GetType(array, index) == TokenType_Or || GetType(array, index) == TokenType_Equals || GetType(array, index) == TokenType_NotEquals || GetType(array, index) == TokenType_More || GetType(array, index) == TokenType_EqualsOrMore || GetType(array, index) == TokenType_Less || GetType(array, index) == TokenType_EqualsOrLess || GetType(array, index) == TokenType_Inverse) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum LOpType*)cvector_get(operations, i));
				}
			}
			free(arithmetic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 8;
			return NULL;
		}
		arithmetic->operands = (Operand*)malloc(sizeof(Operand) * cvector_size(operands));
		if (arithmetic->operands == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum OpType*)cvector_get(operations, i));
				}
			}
			free(arithmetic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		arithmetic->operandsCount = cvector_size(operands);
		for (int i = 0; i < cvector_size(operands); i++) {
			arithmetic->operands[i] = *(Operand*)cvector_get(operands, i);
		}
		arithmetic->operations = (enum OpType*)malloc(sizeof(enum OpType) * cvector_size(operations));
		if (arithmetic->operations == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum OpType*)cvector_get(operations, i));
				}
			}
			free(arithmetic->operands);
			free(arithmetic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		arithmetic->operationsCount = cvector_size(operations);
		for (int i = 0; i < cvector_size(operations); i++) {
			arithmetic->operations[i] = *(enum OpType*)cvector_get(operations, i);
		}
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum OpType*)cvector_get(operations, i));
				}
			}
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		operand->data = arithmetic;
		operand->type = OperandType_Arithmetic;
		return operand;
	}
	else {
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		free(arithmetic);
		return NULL;
	}
}

Operand* GetLogic(Array* array, int* index) {
	Operand* operand;
	Logic* logic = (Logic*)malloc(sizeof(Logic));
	if (logic == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	cvector* operands = (cvector*)malloc(sizeof(cvector));
	if (operands == NULL) {
		free(logic);
		ERROR_CODE = 10;
		return NULL;
	}
	cvector* operations = (cvector*)malloc(sizeof(cvector));
	if (operations == NULL) {
		free(logic);
		free(operands);
		ERROR_CODE = 10;
		return NULL;
	}
	Operand* op = GetOperand(array, index);
	if (op != NULL) {
		if (op->type == OperandType_Str) {
			ERROR_CODE = 11;
			freeOperand(op);
			free(operands);
			free(operations);
			free(logic);
			return NULL;
		}
	}
	cvector_init(operands);
	cvector_init(operations);
	if (op != NULL) {
		cvector_push_back(operands, op);
		while (GetType(array, index) == TokenType_And || GetType(array, index) == TokenType_Or || GetType(array, index) == TokenType_Less || GetType(array, index) == TokenType_EqualsOrLess || GetType(array, index) == TokenType_Equals || GetType(array, index) == TokenType_NotEquals || GetType(array, index) == TokenType_More || GetType(array, index) == TokenType_EqualsOrMore) {
			cvector_push_back(operations, GetLogicOperation(((Token*)array->data) + *index));
			AddIndex(index, array, 1);
			op = GetOperand(array, index);
			if (op != NULL) {
				if (op->type == OperandType_Str) {
					ERROR_CODE = 11;
					freeOperand(op);
					for (int i = 0; i < cvector_size(operands); i++) {
						freeOperand(cvector_get(operands, i));
					}
					for (int i = 0; i < cvector_size(operations); i++) {
						free((enum LOpType*)cvector_get(operations, i));
					}
					cvector_free(operands);
					free(operands);
					cvector_free(operations);
					free(operations);
					free(logic);
					return NULL;
				}
				cvector_push_back(operands, op);
			}
			else {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum LOpType*)cvector_get(operations, i));
				}
				cvector_free(operands);
				free(operands);
				cvector_free(operations);
				free(operations);
				free(logic);
				return NULL;
			}
		}
		logic->operands = (Operand*)malloc(sizeof(Operand) * cvector_size(operands));
		if (logic->operands == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum LOpType*)cvector_get(operations, i));
				}
			}
			free(logic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		logic->operandsCount = cvector_size(operands);
		for (int i = 0; i < cvector_size(operands); i++) {
			logic->operands[i] = *(Operand*)cvector_get(operands, i);
		}
		logic->operations = (enum LOpType*)malloc(sizeof(enum LOpType) * cvector_size(operations));
		if (logic->operations == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum LOpType*)cvector_get(operations, i));
				}
			}
			free(logic->operands);
			free(logic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		logic->operationsCount = cvector_size(operations);
		for (int i = 0; i < cvector_size(operations); i++) {
			logic->operations[i] = *(enum LOpType*)cvector_get(operations, i);
		}
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			if (cvector_size(operands) != 0) {
				for (int i = 0; i < cvector_size(operands); i++) {
					freeOperand(cvector_get(operands, i));
				}
			}
			if (cvector_size(operations) != 0) {
				for (int i = 0; i < cvector_size(operations); i++) {
					free((enum LOpType*)cvector_get(operations, i));
				}
			}
			free(logic->operands);
			free(logic->operations);
			free(logic);
			cvector_free(operands);
			free(operands);
			cvector_free(operations);
			free(operations);
			ERROR_CODE = 10;
			return NULL;
		}
		operand->data = logic;
		operand->type = OperandType_Logic;
		return operand;
	}
	else {
		cvector_free(operands);
		free(operands);
		cvector_free(operations);
		free(operations);
		free(logic);
		return NULL;
	}
}

MatrixRow* GetMatrixRow(Array* array, int* index) {
	Operand* operand;
	cvector* columns = (cvector*)malloc(sizeof(cvector));
	if (columns == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	MatrixRow* row = (MatrixRow*)malloc(sizeof(MatrixRow));
	if (row == NULL) {
		free(columns);
		ERROR_CODE = 10;
		return NULL;
	}
	cvector_init(columns);
	int count = 0;
	while (GetType(array, index) != TokenType_RightFigureBracket) {
		if (GetType(array, index) == TokenType_End) {
			free(row);
			for (int i = 0; i < cvector_size(columns); i++) {
				free(cvector_get(columns, i));
			}
			cvector_free(columns);
			free(columns);
			ERROR_CODE = 1;
			return NULL;
		}
		else if (GetType(array, index) == TokenType_Sep) {
			if (count != 0) {
				free(row);
				for (int i = 0; i < cvector_size(columns); i++) {
					free(cvector_get(columns, i));
				}
				cvector_free(columns);
				free(columns);
				ERROR_CODE = 8;
				return NULL;
			}
			AddIndex(index, array, 1);
			count++;
		}
		else if (GetType(array, index) == TokenType_LeftFigureBracket) {
			AddIndex(index, array, 1);
			count = 0;
			operand = GetOperand(array, index);
			if (operand == NULL) {
				free(row);
				for (int i = 0; i < cvector_size(columns); i++) {
					free(cvector_get(columns, i));
				}
				cvector_free(columns);
				free(columns);
				return NULL;
			}
			else {
				cvector_push_back(columns, operand);
			}
			AddIndex(index, array, 1);
		}
		else {
			free(row);
			for (int i = 0; i < cvector_size(columns); i++) {
				free(cvector_get(columns, i));
			}
			cvector_free(columns);
			free(columns);
			ERROR_CODE = 8;
			return NULL;
		}
	}
	row->columnsCount = cvector_size(columns);
	row->columns = (Operand*)malloc(sizeof(Operand) * ((*row).columnsCount));
	if (row->columns == NULL) {
		free(row);
		for (int i = 0; i < cvector_size(columns); i++) {
			free(cvector_get(columns, i));
		}
		cvector_free(columns);
		free(columns);
		ERROR_CODE = 10;
		return NULL;
	}
	for (int i = 0; i < row->columnsCount; i++) {
		row->columns[i] = *(Operand*)cvector_get(columns, i);
	}
	return row;
}

Operand* GetMatrixInitializator(Array* array, int* index) {
	MatrixInitializator* m = (MatrixInitializator*)malloc(sizeof(MatrixInitializator));
	MatrixRow* row;
	if (m == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	cvector* rows = (cvector*)malloc(sizeof(cvector));
	if (rows == NULL) {
		free(m);
		ERROR_CODE = 10;
		return NULL;
	}
	cvector_init(rows);
	if (GetType(array, index) != TokenType_LeftFigureBracket) {
		free(m);
		ERROR_CODE = 8;
		return NULL;
	}
	int count = 0;
	AddIndex(index, array, 1);
	while (GetType(array, index) != TokenType_RightFigureBracket) {
		if (GetType(array, index) == TokenType_End) {
			free(m);
			for (int i = 0; i < cvector_size(rows); i++) {
				free(cvector_get(rows, i));
			}
			cvector_free(rows);
			free(rows);
			ERROR_CODE = 1;
			return NULL;
		}
		else if (GetType(array, index) == TokenType_Sep) {
			if (count != 0) {
				free(m);
				for (int i = 0; i < cvector_size(rows); i++) {
					free(cvector_get(rows, i));
				}
				cvector_free(rows);
				free(rows);
				ERROR_CODE = 8;
				return NULL;
			}
			AddIndex(index, array, 1);
			count++;
		}
		else if (GetType(array, index) == TokenType_LeftFigureBracket) {
			AddIndex(index, array, 1);
			count = 0;
			row = GetMatrixRow(array, index);
			if (row == NULL) {
				free(m);
				for (int i = 0; i < cvector_size(rows); i++) {
					free(cvector_get(rows, i));
				}
				cvector_free(rows);
				free(rows);
				return NULL;
			}
			else {
				cvector_push_back(rows, row);
			}
			AddIndex(index, array, 1);
		}
		else {
			free(m);
			for (int i = 0; i < cvector_size(rows); i++) {
				free(cvector_get(rows, i));
			}
			cvector_free(rows);
			free(rows);
			ERROR_CODE = 8;
			return NULL;
		}
	}
	m->rowsCount = cvector_size(rows);
	m->rows = (MatrixRow*)malloc(sizeof(MatrixRow) * (m->rowsCount));
	if (m->rows == NULL) {
		free(m);
		for (int i = 0; i < cvector_size(rows); i++) {
			free(cvector_get(rows, i));
		}
		cvector_free(rows);
		free(rows);
		ERROR_CODE = 10;
		return NULL;
	}
	for (int i = 0; i < m->rowsCount; i++) {
		m->rows[i] = (MatrixRow*)cvector_get(rows, i);
	}
	return m;
}

Part* GetSetting(Array* array, int* index) {
	Part* part = (Part*)malloc(sizeof(Part));
	part->type = PartType_Error;
	if (part == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	int lastIndex = *index;
	Operand* operand = GetMatrixElement(array, index);
	if (operand == NULL) {
		AddIndex(index, array, -(*index - lastIndex));
		operand = GetVariable(array, index);
		if (operand == NULL) {
			ERROR_CODE = 3;
			free(part);
			return NULL;
		}
		AddIndex(index, array, 1);
	}
	if (GetType(array, index) == TokenType_OpMul || GetType(array, index) == TokenType_OpSub || GetType(array, index) == TokenType_OpDiv || GetType(array, index) == TokenType_OpAdd) {
		enum OpType lastOperationType = (*(Token*)(((Token*)array->data) + *index)).type;
		if ((*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_OpEquals) {
			AddIndex(index, array, 1);
			freeOperand(operand);
			ERROR_CODE = 5;
			free(part);
			return NULL;
		}
		AddIndex(index, array, 2);
		Operand* source;
		int last = *index;
		if (operand->type == OperandType_Variable) {
			source = GetMatrixInitializator(array, index);
			if (source == NULL) {
				AddIndex(index, array, -(*index - lastOperationType));
				source = GetArithmetic(array, index);
				if (source == NULL) {
					freeOperand(operand);
					ERROR_CODE = 10;
					return part;
				}
			}
		}
		else if (operand->type == OperandType_MatrixElement) {
			source = GetArithmetic(array, index);
			if (source == NULL) {
				freeOperand(operand);
				ERROR_CODE = 10;
				return part;
			}
		}
		Arithmetic* arithmetic = (Arithmetic*)malloc(sizeof(Arithmetic));
		if (arithmetic == NULL) {
			freeOperand(operand);
			freeOperand(source);
			ERROR_CODE = 10;
			return part;
		}
		arithmetic->operandsCount = 2;
		arithmetic->operationsCount = 1;
		arithmetic->operands = (Operand*)malloc(sizeof(Operand) * 2);
		if (arithmetic->operands == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic);
			ERROR_CODE = 10;
			return part;
		}
		arithmetic->operands[0] = *operand;
		arithmetic->operands[1] = *source;
		arithmetic->operations = (enum OpType*)malloc(sizeof(enum OpType));
		if (arithmetic->operations == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic);
			ERROR_CODE = 10;
			return part;
		}
		if (lastOperationType == TokenType_OpMul) {
			arithmetic->operations[0] = OpType_Mul;
		}
		else if (lastOperationType == TokenType_OpSub) {
			arithmetic->operations[0] = OpType_Sub;
		}
		else if (lastOperationType == TokenType_OpDiv) {
			arithmetic->operations[0] = OpType_Div;
		}
		else if (lastOperationType == TokenType_OpAdd) {
			arithmetic->operations[0] = OpType_Add;
		}
		Operand* expression = (Operand*)malloc(sizeof(Operand));
		if (expression == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			ERROR_CODE = 10;
			return part;
		}
		expression->data = arithmetic;
		expression->type = OperandType_Arithmetic;
		Setting* setting = (Setting*)malloc(sizeof(Setting));
		if (setting == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			free(expression);
			ERROR_CODE = 10;
			return part;
		}
		setting->variable = operand;
		setting->source = expression;
		part->data = setting;
		part->type = PartType_Set;
		return part;
	}
	else if (GetType(array, index) == TokenType_OpEquals) {
		AddIndex(index, array, 1);
		Operand* source;
		int last = *index;
		if (operand->type == OperandType_Variable) {
			source = GetMatrixInitializator(array, index);
			if (source == NULL) {
				AddIndex(index, array, -(*index - last));
				source = GetArithmetic(array, index);
				if (source == NULL) {
					freeOperand(operand);
					ERROR_CODE = 10;
					return part;
				}
			}
		}
		else if (operand->type == OperandType_MatrixElement) {
			source = GetArithmetic(array, index);
			if (source == NULL) {
				freeOperand(operand);
				ERROR_CODE = 10;
				return part;
			}
		}
		if (source == NULL) {
			freeOperand(operand);
			return part;
		}
		Setting* setting = (Setting*)malloc(sizeof(Setting));
		if (setting == NULL) {
			freeOperand(source);
			freeOperand(operand);
			ERROR_CODE = 10;
			return part;
		}
		setting->variable = operand;
		setting->source = source;
		part->data = setting;
		part->type = PartType_Set;
		return part;
	}
	else {
		free(operand);
		ERROR_CODE = 5;
		free(part);
		return NULL;
	}
}

Part* GetDel(Array* array, int* index) {
	Part* part = (Part*)malloc(sizeof(Part));
	if (part == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if (GetType(array, index) != TokenType_Del) {
		ERROR_CODE = 2;
		free(part);
		return NULL;
	}
	else {
		part->type = PartType_Error;
		AddIndex(index, array, 1);
		Operand* operand = GetVariable(array, index);
		if (operand == NULL) {
			ERROR_CODE = 3;
			return part;
		}
		int new_index = *index + 1;
		enum TokenType next = GetType(array, &new_index);
		if (next != TokenType_End && next != TokenType_Sep && next != TokenType_RightFigureBracket) {
			freeOperand(operand);
			ERROR_CODE = 4;
			return part;
		}
		part->data = operand;
		part->type = PartType_Del;
		AddIndex(index, array, 1);
		return part;
	}
}

Part* GetString(Array* array, int* index) {
	Part* part;
	if (GetType(array, index) != TokenType_Str) {
		ERROR_CODE = 8;
		return NULL;
	}
	else {
		part = (Part*)malloc(sizeof(Part));
		if (part == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = (char*)malloc(strlen((*(Token*)(((Token*)array->data) + *index)).data) * sizeof(char));
		if (part->data == NULL) {
			free(part);
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = (*(Token*)(((Token*)array->data) + *index)).data;
		part->type = PartType_Str;
		AddIndex(index, array, 1);
		return part;
	}
}

Part* Get_Logic_Construction(Array* array, int* index, int type) {
	Part* part = (Part*)malloc(sizeof(Part));
	if (part == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((GetType(array, index) != TokenType_If && type == 0) || (GetType(array, index) != TokenType_Repeat && type == 1) || (GetType(array, index) != TokenType_Elif && type == 2) || (GetType(array, index) != TokenType_Else && type == 3)) {
		ERROR_CODE = 8;
		free(part);
		return NULL;
	}
	part->type = PartType_Error;
	AddIndex(index, array, 1);
	Operand* logic;
	if (type != 3) {
		if (GetType(array, index) != TokenType_LeftBracket) {
			ERROR_CODE = 8;
			return part;
		}
		AddIndex(index, array, 1);
		logic = GetLogic(array, index);
		if (logic == NULL) {
			return part;
		}
		if (GetType(array, index) != TokenType_RightBracket) {
			ERROR_CODE = 8;
			return part;
		}
		AddIndex(index, array, 1);
	}
	if ((*(((Token*)array->data) + *index)).type != TokenType_LeftFigureBracket) {
		ERROR_CODE = 8;
		if (type != 3) {
			freeOperand(logic);
		}
		return part;
	}
	AddIndex(index, array, 1);
	Expression* expression = GetExpression(array, index, 1);
	if (expression == NULL) {
		if (type != 3) {
			freeOperand(logic);
		}
		return part;
	}
	Logic_Construction* logic_construction = (Logic_Construction*)malloc(sizeof(Logic_Construction));
	if (logic_construction == NULL) {
		ERROR_CODE = 10;
		if (type != 3) {
			freeOperand(logic);
		}
		freeExpression(expression);
		return part;
	}
	if (type != 3) {
		logic_construction->condition = logic->data;
	}
	else {
		logic_construction->condition = NULL;
	}
	logic_construction->expression = expression;
	switch (type) {
	case 0:
		logic_construction->logicPartType = LogicPartType_If;
		break;
	case 1:
		logic_construction->logicPartType = LogicPartType_Repeat;
		break;
	case 2:
		logic_construction->logicPartType = LogicPartType_Elif;
		break;
	case 3:
		logic_construction->logicPartType = LogicPartType_Else;
		break;
	}
	part->data = logic_construction;
	part->type = PartType_Logic_Construction;
	return part;
}

Part* GetPart(Array* array, int* index) {
	int pos = *index;
	Part* part;
	if (IF_TYPE == 2 || IF_TYPE == 1) {
		part = Get_Logic_Construction(array, index, 2);
		if (part == NULL) {
			//AddIndex(index, array, -(*index - pos));
			part = Get_Logic_Construction(array, index, 3);
			IF_TYPE = 0;
			if (part != NULL && part->type != PartType_Error) {
				return part;
			}
			else if (part != NULL) {
				free(part);
				return NULL;
			}
			//AddIndex(index, array, -(*index - pos));
		}
		else if (part != NULL && part->type == PartType_Error) {
			free(part);
			return NULL;
		}
		else {
			IF_TYPE = 2;
			return part;
		}
	}
	part = GetString(array, index);
	if (part == NULL) {
		AddIndex(index, array, -(*index - pos));
		part = GetDel(array, index);
		if (part == NULL) {
			AddIndex(index, array, -(*index - pos));
			part = GetSetting(array, index);
			if (part == NULL) {
				AddIndex(index, array, -(*index - pos));
				part = Get_Logic_Construction(array, index, 0);
				if (part == NULL) {
					AddIndex(index, array, -(*index - pos));
					part = Get_Logic_Construction(array, index, 1);
					if (part == NULL) {
						AddIndex(index, array, -(*index - pos));
						Operand* operand;
						operand = GetArithmetic(array, index);
						if (operand == NULL) {
							AddIndex(index, array, -(*index - pos));
							operand = GetLogic(array, index);
							if (operand == NULL) {
								return NULL;
							}
							part = (Part*)malloc(sizeof(Part));
							if (part == NULL) {
								freeOperand(operand);
								ERROR_CODE = 10;
								return NULL;
							}
							part->data = operand;
							part->type = PartType_Logic;
						}
						else {
							part = (Part*)malloc(sizeof(Part));
							if (part == NULL) {
								freeOperand(operand);
								ERROR_CODE = 10;
								return NULL;
							}
							part->data = operand;
							part->type = PartType_Arithmetic;
						}
						IF_TYPE = 0;
					}
					else if (part != NULL && part->type == PartType_Error) {
						free(part);
						return NULL;
					}
					else {
						IF_TYPE = 0;
					}
				}
				else if (part != NULL && part->type == PartType_Error) {
					free(part);
					return NULL;
				}
				else {
					IF_TYPE = 1;
				}
			}
			else if (part != NULL && part->type == PartType_Error) {
				free(part);
				return NULL;
			}
			else {
				IF_TYPE = 0;
			}
		}
		else if (part != NULL && part->type == PartType_Error) {
			free(part);
			return NULL;
		}
		else {
			IF_TYPE = 0;
		}
	}
	return part;
}

Expression* GetExpression(Array* array, int* index, int isBody) {
	cvector* parts = (cvector*)malloc(sizeof(cvector));
	if (parts == NULL) {
		error2(errors3[10]);
		return NULL;
	}
	Part* part;
	cvector_init(parts);
	short isFinish = 0;
	while (((*(((Token*)array->data) + *index)).type != TokenType_End && !isBody) || ((*(((Token*)array->data) + *index)).type != TokenType_RightFigureBracket && isBody)) {
		if ((*(((Token*)array->data) + *index)).type == TokenType_Sep) {
			AddIndex(index, array, 1);
			isFinish = 0;
			continue;
		}
		else if ((*(((Token*)array->data) + *index)).type == TokenType_NewLineSep) {
			AddIndex(index, array, 1);
			isFinish = 0;
			part = (Part*)malloc(sizeof(Part));
			if (part == NULL) {
				error2(errors3[10]);
				return NULL;
			}
			part->data = NULL;
			part->type = PartType_NewLine;
			cvector_push_back(parts, part);
			part = NULL;
			continue;
		}
		else if ((*(((Token*)array->data) + *index)).type == TokenType_NewLine) {
			AddIndex(index, array, 1);
			continue;
		}
		part = GetPart(array, index);
		if (part != NULL) {
			cvector_push_back(parts, part);
			isFinish = 1;
		}
		else {
			isFinish = -1;
			break;
		}
	}
	if (isFinish == 0) {
		if (!isBody) {
			error(errors3[1]);
		}
		for (int j = 0; j < cvector_size(parts); j++)
			freePart(cvector_get(parts, j));
		return NULL;
	}
	else if (isFinish == -1) {
		if (!isBody) {
			error(errors3[ERROR_CODE]);
		}
		cvector_free(parts);
		free(parts);
		return NULL;
	}
	else {
		Expression* expression = (Expression*)malloc(sizeof(Expression));
		if (expression == NULL) {
			cvector_free(parts);
			free(parts);
			if (!isBody) {
				error2(errors3[ERROR_CODE]);
			}
			return NULL;
		}
		expression->count = cvector_size(parts);
		expression->parts = (Part*)malloc(sizeof(Part) * cvector_size(parts));
		if (expression->parts == NULL) {
			free(expression);
			cvector_free(parts);
			free(parts);
			if (!isBody) {
				error2(errors3[ERROR_CODE]);
			}
			return NULL;
		}
		for (int j = 0; j < cvector_size(parts); j++) {
			expression->parts[j] = *(Part*)cvector_get(parts, j);
		}
		cvector_free(parts);
		free(parts);
		AddIndex(index, array, 1);
		return expression;
	}
}

Expression* parse(Array* array) {
	environment = getCurrent();
	(*environment).type = 1;
	(*environment).line = 1;
	(*environment).pos = 1;
	int i = 0;
	Expression* expression = GetExpression(array, &i, 0);
	return expression;
}