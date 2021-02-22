/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "parser.h"

int ERROR_CODE = 0;
int ERROR_POS = 0;

float atof2(char* s) {
	int i_Left = 0;
	float   i_Right = 0.0f;
	float   i_Sgn = 1.0f;
	int i = 0;

	if ('-' == s[i])
		i_Sgn = -1.0f,
		i++;

	while ('0' <= s[i] && s[i] <= '9')
		i_Left = i_Left * 10 + s[i++] - '0';
	if ('.' == s[i++])
		while ('0' <= s[i] && s[i] <= '9')
			i_Right = (i_Right + (float)(s[i++] - '0')) / 10.0f;
	return i_Sgn * ((float)i_Left + i_Right);
}

void freeArithmetic(Arithmetic* arithmetic);
void freeOperand(Operand* operand);
Operand* GetOperand(Array* array, int* index);
Operand* GetArithmetic(Array* array, int* index);

void freeUnar(Operand* operand) {
	free(operand);
}

void freeNumber(Const* number) {
	free(number);
}

void freeFunction(Function* function) {
	for (int i = 0; i < function->count; i++) {
		if(function->operands + i)
		freeOperand(function->operands + i);
	}
	if(function)
	free(function);
}

void freeOperand(Operand* operand) {
	if(operand->type == OperandType_Str){
		free(operand->data);
	}
	free(operand);
}

void freeArithmetic(Arithmetic* arithmetic) {
	for (int i = 0; i < arithmetic->operandsCount; i++) {
		if(arithmetic->operands + i)
		freeOperand(arithmetic->operands + i);
	}
	for (int i = 0; i < arithmetic->operationsCount; i++) {
		if(arithmetic->operations + i)
		free(arithmetic->operations + i);
	}
	if(arithmetic)
	free(arithmetic);
}

void freeDel(char* name) {
	if(name)
	free(name);
}

void freeSet(Setting* set) {
	free(set);
}

void freePart(Part* part) {
	switch (part->type) {
	case PartType_Arithmetic:
		if(part->data)
		freeArithmetic(part->data);
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
		if(part->data)
			free(part->data);
		break;
	}
	if(part)
	free(part);
}

void freeExpression(Expression* expression) {
	free(expression->parts);
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
	if(args == NULL){
		ERROR_CODE = 10;
		return NULL;
	}
	cvector_init(args);
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Name && (*(Token*)(((Token*)array->data) + *index+1)).type == TokenType_LeftBracket) {
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
	if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpSub){
		operand = (Operand*)malloc(sizeof(Operand));
		if (operand == NULL) {
			ERROR_CODE = 10;
			return NULL;
		}
		operand->type = OperandType_Unar;
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
	}else if((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Int || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_Float){
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
		operand = GetArithmetic(array, index);
		if (operand == NULL && (*(Token*)(((Token*)array->data) + *index)).type != TokenType_RightBracket) {
			return NULL;
		}
		(*index)++;
		return operand;
	}
	else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_RightBracket) {
		return NULL;
	}else if ((*(Token*)(((Token*)array->data) + *index)).type == TokenType_Str) {
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
	if(operands == NULL){
		free(arithmetic);
				ERROR_CODE = 10;
		return NULL;
	}
	cvector* operations = operations = (cvector*)malloc(sizeof(cvector));
		if(operations == NULL){
		free(arithmetic);
		free(operands);
				ERROR_CODE = 10;
		return NULL;
	}
	Operand* op = GetOperand(array, index);
	if(op != NULL){
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

Part* GetSetting(Array* array, int* index) {
	Part* part;
	Operand* operand = GetVariable(array, index);
	if (operand == NULL) {
		ERROR_CODE = 10;
		return NULL;
	}
	(*index)++;
	if((*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpMul || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpSub || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpDiv || (*(Token*)(((Token*)array->data) + *index)).type == TokenType_OpAdd){
		enum OpType last = (*(Token*)(((Token*)array->data) + *index)).type;
		if((*(Token*)(((Token*)array->data) + *index+1)).type != TokenType_OpEquals){
			freeOperand(operand);
			ERROR_CODE = 5;
			return NULL;
		}
		(*index)+=2;
		Operand* source = GetArithmetic(array, index);
		if (source == NULL) {
		    freeOperand(operand);
			ERROR_CODE = 10;
			return NULL;
		}
		Arithmetic* arithmetic = (Arithmetic*)malloc(sizeof(Arithmetic));
		if(arithmetic == NULL){
			freeOperand(operand);
			freeOperand(source);
			ERROR_CODE = 10;
			return NULL;
		}
		arithmetic->operandsCount = 2;
		arithmetic->operationsCount = 1;
		arithmetic->operands = (Operand*)malloc(sizeof(Operand)*2);
		if(arithmetic->operands == NULL){
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
		}
		arithmetic->operands[0] = *operand;
		arithmetic->operands[1] = *source;
		arithmetic->operations = (enum OpType*)malloc(sizeof(enum OpType));
		if(arithmetic->operations == NULL){
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
		}
		if(last == TokenType_OpMul){
		arithmetic->operations[0] = OpType_Mul;
		}else if(last == TokenType_OpSub){
		arithmetic->operations[0] = OpType_Sub;
		}else if(last == TokenType_OpDiv){
		arithmetic->operations[0] = OpType_Div;
		}else if(last == TokenType_OpAdd){
		arithmetic->operations[0] = OpType_Add;
		}else{
			freeOperand(operand);
			freeOperand(source);
			free(arithmetic->operands);
			free(arithmetic->operations);
			free(arithmetic);
			ERROR_CODE = 10;
			return NULL;
		}
		Operand* expression = (Operand*)malloc(sizeof(Operand));
		if(expression == NULL){
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
		if (setting== NULL) {
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
		if (setting== NULL) {
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
		if ((*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_End && (*(Token*)(((Token*)array->data) + *index + 1)).type != TokenType_Sep) {
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

Part* GetString(Array* array, int* index){
	Part* part;
	if((*(Token*)(((Token*)array->data) + *index)).type != TokenType_Str){
		ERROR_CODE = 8;
		return NULL;
	}else{
		part = (Part*)malloc(sizeof(Part));
		if(part == NULL){
			ERROR_CODE = 10;
			return NULL;
		}
		part->data = (char*)malloc(strlen((*(Token*)(((Token*)array->data) + *index)).data)*sizeof(char));
		if(part->data == NULL){
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

Part* GetPart(Array* array, int* index) {
	int pos = *index;
	Part* part;
	part = GetString(array, index);
	if (part == NULL) {
		*index = pos;
	    part = GetDel(array, index);
		if(part == NULL){
		*index = pos;
		part = GetSetting(array, index);
		if (part == NULL) {
			*index = pos;
			Operand* operand;
			operand = GetArithmetic(array, index);
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
			part->type = PartType_Arithmetic;
		}
		}
	}
	return part;
}

Expression* parse(Array* array) {
	int i = 0;
	cvector* parts = (cvector*)malloc(sizeof(cvector));
	if(parts == NULL){
		error2(ERRORS[10]);
		return NULL;
	}
	Part* part;
	cvector_init(parts);
	short isFinish = 0;
	while ((*(((Token*)array->data) + i)).type != TokenType_End) {
		if ((*(((Token*)array->data) + i)).type == TokenType_Sep) {
			i++;
			isFinish = 0;
		}else if ((*(((Token*)array->data) + i)).type == TokenType_NewLineSep) {
			i++;
			isFinish = 0;
			part = (Part*)malloc(sizeof(Part));
			if(part == NULL){
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
		for(int j = 0; j < cvector_size(parts); j++)
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
		return expression;
	}
}