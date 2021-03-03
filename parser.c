/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "parser.h"

int ERROR_CODE = 0;
int ERROR_POS = 0;
int IF_TYPE = 0;

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
	//for (int i = 0; i < function->count; i++) {
	//	if (function->operands + i)
			freeOperand(function->operands);
	//}
	if (function)
		free(function);
}

void freeOperand(Operand* operand) {
	switch (operand->type) {
	case OperandType_Str:
		free(operand->data);
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
	//	freeNumber(operand->data);
		break;
	case OperandType_Number:
		//freeNumber(operand->data);
		break;
	case OperandType_Unar:
		freeOperand(operand->data);
		break;
	//case OperandType_Variable:
	//	free(operand->data);
	//	break;
	}
	free(operand);
}

void freeArithmetic(Arithmetic* arithmetic) {
	//for (int i = 0; i < arithmetic->operandsCount; i++) {
	//	if (arithmetic->operands + i)
			freeOperand(arithmetic->operands);
	//}
	//for (int i = 0; i < arithmetic->operationsCount; i++) {
	//	if (arithmetic->operations + i)
//			free(arithmetic->operations);
	//}
	if (arithmetic)
		free(arithmetic);
}

void freeLogic(Logic* logic) {
	//for (int i = 0; i < logic->operandsCount; i++) {
	//	if (logic->operands + i)
			freeOperand(logic->operands);
	//}
	//for (int i = 0; i < logic->operationsCount; i++) {
	//	if (logic->operations + i)
	//		free(logic->operations);
	//}
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
	//if (part)
	//	free(part);
}

void freeExpression(Expression* expression) {
	for (int i = 0; i < expression->count; i++) {
		freePart(expression->parts+i);
	}
	free(expression);
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
	return operation;
}

Const* GetNumber2(Array* array, int* index) {
	Const* number = (Const*)malloc(sizeof(Const));
	if (number == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Int) {
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
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Float) {
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
		ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_False) {
		logicConst->data = (short*)malloc(sizeof(short));
		if (logicConst->data == NULL) {
			free(logicConst);
			ERROR_CODE = 10;
			return NULL;
		}
		*((short*)(*(Const*)logicConst).data) = 0;
		logicConst->type = VarType_Logic;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_True) {
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

Operand* GetVariable(Array* array, int* index) {
	Operand* operand = (Operand*)malloc(sizeof(Operand));
	if (operand == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Name) {
		(*(Operand*)operand).data = (char*)(*(Token*)(((Token*)array->data) + *index)).data;
		(*(Operand*)operand).type = OperandType_Variable;
		return operand;
	}
	else {
		ERROR_CODE = 8;
		ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
	cvector_init(args);
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Name && (*(Token*)(((Token*)array->data) + *index + 1)).type == TokenType_LeftBracket) {
		function = (Function*)malloc(sizeof(Function));
		if (function == NULL) {
			cvector_free(args);
			free(args);
			ERROR_CODE = 10;
			return NULL;
		}
		function->name = (char*)(*(Token*)(((Token*)array->data) + *index)).data;
		(*index) += 2;
		Operand* arg;
		if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Str) {
			arg = GetArithmetic(array, index);
		}
		else {
			arg = GetOperand(array, index);
		}
		if (arg != NULL) {
			cvector_push_back(args, arg);
			while ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_RightBracket) {
				if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Sep) {
					(*index)++;
				}
				if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_RightBracket) {
					ERROR_CODE = 8;
					ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
					freeOperand(arg);
					free(function);
					cvector_free(args);
					free(args);
					return NULL;
				}
				if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Str) {
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
			(*index)++;
			cvector_free(args);
			return operand;
		}
		else if (arg == NULL && (*(Token*)(((Token*)array->data) + *index)).type == TokenType_RightBracket) {
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
			(*index)++;
			return operand;
		}
		else {
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
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpSub) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		(*index)++;
		Operand* p = GetOperand(array, index);
		if (p == NULL) {
			free(operand);
			return NULL;
		}
		operand->data = p;
		operand->type = OperandType_Unar;
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Inverse) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		(*index)++;
		Operand* p = GetOperand(array, index);
		if (p == NULL) {
			free(operand);
			return NULL;
		}
		operand->data = p;
		operand->type = OperandType_Inverse;
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_False || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_True) {
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
		(*index)++;
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Name) {
		operand = GetFunction(array, index);
		if (operand == NULL) {
			operand = GetVariable(array, index);
			(*index)++;
			if (operand == NULL) {
				return NULL;
			}
		}
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Int || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Float) {
		Const* number = NULL;
		number = GetNumber2(array, index);
		(*index)++;
		if (number == NULL) {
			ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_LeftBracket) {
		(*index)++;
		int lastIndex = *index;
		operand = GetArithmetic(array, index);
		if (operand == NULL && (*(Token*)(((Token*)array->data) + *index)).type != TokenType_RightBracket) {
			return NULL;
		}
		else if (operand == NULL) {
			*index = lastIndex;
			operand = GetLogic(array, index);
			if (operand == NULL && (*(Token*)(((Token*)array->data) + *index)).type != TokenType_RightBracket) {
				return NULL;
			}
		}
		(*index)++;
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_RightBracket) {
		return NULL;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Str) {
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		operand->data = ((Token*)(((Token*)array->data) + *index))->data;
		operand->type = OperandType_Str;
		(*index)++;
		return operand;
	}
	else {
		ERROR_CODE = 8;
		ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
			ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
		while ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpDiv || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpMul || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpSub || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpAdd) {
			cvector_push_back(operations, GetOperation(((Token*)array->data) + *index));
			(*index)++;
			op = GetOperand(array, index);
			if (op != NULL) {
				if (op->type == OperandType_Str) {
					ERROR_CODE = 11;
					ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
				ERROR_CODE = 10;
				return NULL;
			}
		
		}
		if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_And || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Or || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Equals || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_NotEquals || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_More || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_EqualsOrMore || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Less || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_EqualsOrLess || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Inverse) {
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
		ERROR_CODE = 10;
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
			ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
		while ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_And || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Or || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Equals || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_NotEquals || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_More || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_EqualsOrMore || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Less || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_EqualsOrLess) {
			cvector_push_back(operations, GetLogicOperation(((Token*)array->data) + *index));
			(*index)++;
			op = GetOperand(array, index);
			if (op != NULL) {
				if (op->type == OperandType_Str) {
					ERROR_CODE = 11;
					ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
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
				ERROR_CODE = 10;
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
		ERROR_CODE = 10;
		return NULL;
	}
}

Part* GetSetting(Array* array, int* index) {
	Part* part;
	Operand* operand = GetVariable(array, index);
	if (operand == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	(*index)++;
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpMul || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpSub || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpDiv || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpAdd) {
		enum OpType last = (*(Token*)(((Token*)array->data) + *index)).type;
		if ((*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_OpEquals) {
			freeOperand(operand);
			ERROR_CODE = 5;
			return NULL;
		}
		(*index) += 2;
		Operand* source = GetArithmetic(array, index);
		if (source == NULL) {
			freeOperand(operand);
			ERROR_CODE = 10;
			return NULL;
		}
		Arithmetic* arithmetic = (Arithmetic*)malloc(sizeof(Arithmetic));
		if (arithmetic == NULL) {
			freeOperand(operand);
			freeOperand(source);
			ERROR_CODE = 10;
			return NULL;
		}
		arithmetic->operandsCount = 2;
		arithmetic->operationsCount = 1;
		arithmetic->operands = (Operand*)malloc(sizeof(Operand) * 2);
		if (arithmetic->operands == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
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
			return NULL;
		}
		if (last == TokenType_OpMul) {
			arithmetic->operations[0] = OpType_Mul;
		}
		else if (last == TokenType_OpSub) {
			arithmetic->operations[0] = OpType_Sub;
		}
		else if (last == TokenType_OpDiv) {
			arithmetic->operations[0] = OpType_Div;
		}
		else if (last == TokenType_OpAdd) {
			arithmetic->operations[0] = OpType_Add;
		}
		else {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
		}
		Operand* expression = (Operand*)malloc(sizeof(Operand));
		if (expression == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
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
			return NULL;
		}
		setting->variable = operand;
		setting->source = expression;
		part = (Part*)malloc(sizeof(Part));
		if (part == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(setting);
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = setting;
		part->type = PartType_Set;
		return part;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpEquals) {
		(*index)++;
		Operand* source = GetArithmetic(array, index);
		if (source == NULL) {
			freeOperand(operand);
			ERROR_CODE = 10;
			return NULL;
		}
		Setting* setting = (Setting*)malloc(sizeof(Setting));
		if (setting == NULL) {
			freeOperand(source);
			freeOperand(operand);
			ERROR_CODE = 10;
			return NULL;
		}
		setting->variable = operand;
		setting->source = source;
		part = (Part*)malloc(sizeof(Part));
		if (part == NULL) {
			freeOperand(operand);
			freeOperand(source);
			free(setting);
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = setting;
		part->type = PartType_Set;
		return part;
	}
	else {
		(*index)--;
		free(operand);
		ERROR_CODE = 5;
		ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
		return NULL;
	}
}

Part* GetDel(Array* array, int* index) {
	Part* part;
	if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Del) {
		ERROR_CODE = 2;
		ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
		return NULL;
	}
	else {
		(*index)++;
		Operand* operand = GetVariable(array, index);
		if (operand == NULL) {
			ERROR_CODE = 3;
			ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
			return NULL;
		}
		if ((*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_End && (*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_Sep && (*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_RightFigureBracket) {
			freeOperand(operand);
			ERROR_CODE = 4;
			ERROR_POS = (*(Token*)(((Token*)array->data) + *index)).coord;
			return NULL;
		}
		part = (Part*)malloc(sizeof(Part));
		if (part == NULL) {
			freeOperand(operand);
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = operand;
		part->type = PartType_Del;
		(*index)++;
		return part;
	}
}

Part* GetString(Array* array, int* index) {
	Part* part;
	if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Str) {
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
		(*index)++;
		return part;
	}
}

Part* Get_Logic_Construction(Array* array, int* index, int type) {
	Part* part;
	if (((*(Token*)(((Token*)array->data) + *index)).type != TokenType_If && type == 0) || ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Repeat && type == 1) || ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Elif && type == 2) || ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Else && type == 3)) {
		ERROR_CODE = 8;
		return NULL;
	}
	(*index)++;
	Operand* logic;
	if (type != 3) {
		if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_LeftBracket) {
			ERROR_CODE = 8;
			return NULL;
		}
		(*index)++;
		logic = GetLogic(array, index);
		if (logic == NULL) {
			return NULL;
		}
		(*index)++;
	}
	if ((*(Token*)(((Token*)array->data) + *index)).type != TokenType_LeftFigureBracket) {
		ERROR_CODE = 8;
		if (type != 3) {
			freeOperand(logic);
		}
		return NULL;
	}
	(*index)++;
	Expression* expression = GetExpression(array, index, 1);
	if (expression == NULL) {
		if (type != 3) {
			freeOperand(logic);
		}
		return NULL;
	}
	Logic_Construction* logic_construction = (Logic_Construction*)malloc(sizeof(Logic_Construction));
	if (logic_construction == NULL) {
		ERROR_CODE = 10;
		if (type != 3) {
			freeOperand(logic);
		}
		freeExpression(expression);
		return NULL;
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
	part = (Part*)malloc(sizeof(Part));
	if (part == NULL) {
		ERROR_CODE = 10;
		freeOperand(logic);
		freeExpression(expression);
		freeLogicConstruction(logic_construction);
		return NULL;
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
			part = Get_Logic_Construction(array, index, 3);
			IF_TYPE = 0;
			if (part != NULL) {
				return part;
			}
		}
		else {
			IF_TYPE = 2;
			return part;
		}
	}
	part = GetString(array, index);
	if (part == NULL) {
		*index = pos;
		part = GetDel(array, index);
		if (part == NULL) {
			*index = pos;
			part = GetSetting(array, index);
			if (part == NULL) {
				*index = pos;
				part = Get_Logic_Construction(array, index, 0);
				if (part == NULL) {
					*index = pos;
					part = Get_Logic_Construction(array, index, 1);
					if (part == NULL) {
						*index = pos;
						Operand* operand;
						operand = GetArithmetic(array, index);
						if (operand == NULL) {
							*index = pos;
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
					else {
						IF_TYPE = 0;
					}
				}
				else {
					IF_TYPE = 1;
				}
			}
			else {
				IF_TYPE = 0;
			}
		}
		else {
			IF_TYPE = 0;
		}
	}
	return part;
}

Expression* GetExpression(Array* array, int* index, int isBody) {
	int i = *index;
	cvector* parts = (cvector*)malloc(sizeof(cvector));
	if (parts == NULL) {
		error2(ERRORS[10]);
		return NULL;
	}
	Part* part;
	cvector_init(parts);
	short isFinish = 0;
	while (((*(((Token*)array->data) + i)).type != TokenType_End && !isBody) || ((*(((Token*)array->data) + i)).type != TokenType_RightFigureBracket && isBody)) {
		if ((*(((Token*)array->data) + i)).type == TokenType_Sep) {
			i++;
			isFinish = 0;
		}
		else if ((*(((Token*)array->data) + i)).type == TokenType_NewLineSep) {
			i++;
			isFinish = 0;
			part = (Part*)malloc(sizeof(Part));
			if (part == NULL) {
				error2(ERRORS[10]);
				return NULL;
			}
			part->data = NULL;
			part->type = PartType_NewLine;
			cvector_push_back(parts, part);
			part = NULL;
		}
		part = GetPart(array, &i);
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
		error(ERRORS[1], ERROR_POS);
		for (int j = 0; j < cvector_size(parts); j++)
			freePart(cvector_get(parts, j));
		return NULL;
	}
	else if (isFinish == -1) {
		error(ERRORS[ERROR_CODE], ERROR_POS);
		cvector_free(parts);
		free(parts);
		return NULL;
	}
	else {
		Expression* expression = (Expression*)malloc(sizeof(Expression));
		if (expression == NULL) {
			cvector_free(parts);
			free(parts);
			error2(ERRORS[ERROR_CODE]);
			return NULL;
		}
		expression->count = cvector_size(parts);
		expression->parts = (Part*)malloc(sizeof(Part) * cvector_size(parts));
		if (expression->parts == NULL) {
			free(expression);
			cvector_free(parts);
			free(parts);
			error2(ERRORS[ERROR_CODE]);
			return NULL;
		}
		for (int j = 0; j < cvector_size(parts); j++) {
			expression->parts[j] = *(Part*)cvector_get(parts, j);
		}
		cvector_free(parts);
		free(parts);
		*index = i + 1;
		return expression;
	}
}

Expression* parse(Array* array) {
	int i = 0;
	Expression* expression = GetExpression(array, &i, 0);
	return expression;
}