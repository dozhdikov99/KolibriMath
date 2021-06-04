/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/

#ifndef  DATA_STRUCTURES_H

#define DATA_STRUCTURES_H

enum TokenType {
	TokenType_Float,
	TokenType_Int,
	TokenType_Name,
	TokenType_LeftBracket,
	TokenType_RightBracket,
	TokenType_OpAdd,
	TokenType_OpSub,
	TokenType_OpDiv,
	TokenType_OpMul,
	TokenType_OpEquals,
	TokenType_Del,
	TokenType_Sep,
	TokenType_NewLineSep,
	TokenType_Str,
	TokenType_If,
	TokenType_Repeat,
	TokenType_LeftFigureBracket,
	TokenType_RightFigureBracket,
	TokenType_And,
	TokenType_Or,
	TokenType_Equals,
	TokenType_NotEquals,
	TokenType_More,
	TokenType_EqualsOrMore,
	TokenType_Less,
	TokenType_EqualsOrLess,
	TokenType_Inverse,
	TokenType_False,
	TokenType_True,
	TokenType_Elif,
	TokenType_Else,
	TokenType_NewLine,
	TokenType_LeftSquareBracket,
	TokenType_RightSquareBracket,
	TokenType_End
};

typedef struct {
	char* data;
	enum TokenType type;
	int coord;
	int line;
} Token;

typedef struct {
	Token* data;
	int size;
} Array;

enum PartType {
	PartType_Set,
	PartType_Arithmetic,
	PartType_Del,
	PartType_NewLine,
	PartType_Str,
	PartType_Logic,
	PartType_Logic_Construction,
	PartType_Error
};

enum VarType {
	VarType_Int,
	VarType_Float,
	VarType_Undefined,
	VarType_Str,
	VarType_Logic,
	VarType_IntMatrix,
	VarType_FloatMatrix
};

enum OperandType {
	OperandType_Function,
	OperandType_Number,
	OperandType_Variable,
	OperandType_Arithmetic,
	OperandType_Unar,
	OperandType_Logic,
	OperandType_LogicConst,
	OperandType_Str,
	OperandType_Inverse,
	OperandType_MatrixElement,
	OperandType_MatrixInitializator
};

enum OpType {
	OpType_Add,
	OpType_Mul,
	OpType_Sub,
	OpType_Div,
};

enum LOpType {
	LOpType_And,
	LOpType_Or,
	LOpType_Equals,
	LOpType_NotEquals,
	LOpType_More,
	LOpType_EqualsOrMore,
	LOpType_Less,
	LOpType_EqualsOrLess
};

enum LogicPartType {
	LogicPartType_If,
	LogicPartType_Repeat,
	LogicPartType_Elif,
	LogicPartType_Else
};

typedef struct {
	void* data;
	enum PartType type;
} Part;

typedef struct {
	void* data;
	enum VarType type;
} Const;

typedef struct {
	void* data;
	enum OperandType type;
} Operand;

typedef struct {
	Operand* columns;
	int columnsCount;
} MatrixRow;

typedef struct {
	MatrixRow** rows;
	int rowsCount;
} MatrixInitializator;

typedef struct {
	char* matrixName;
	void* rowIndex;
	void* columnIndex;
} MatrixElement;

typedef struct {
	char* name;
	Operand* operands;
	int count;
	int line;
	int pos;
} Function;

typedef struct {
	Operand* operands;
	enum OpType* operations;
	int operandsCount;
	int operationsCount;
} Arithmetic;

typedef struct {
	Operand* operands;
	enum LOpType* operations;
	int operandsCount;
	int operationsCount;
} Logic;

typedef struct {
	Operand* variable;
	Operand* source;
} Setting;

typedef struct {
	Part* parts;
	int count;
} Expression;

typedef struct {
	Logic* condition;
	Expression* expression;
	enum LogicPartType logicPartType;
}Logic_Construction;

typedef struct {
	double** elements;
	int rowsCount;
	int columnsCount;
	enum VarType elementsType;
} Matrix;

enum ResultType {
	ResultType_Int,
	ResultType_Float,
	ResultType_Undefined,
	ResultType_Str,
	ResultType_Spec,
	ResultType_Logic,
	ResultType_IntMatrix,
	ResultType_FloatMatrix
};

typedef struct {
	double value;
	char* str;
	Matrix* matrix;
	enum ResultType type;
} Result;

typedef struct {
	char* name;
	double value;
	Matrix* matrix;
	enum VarType type;
	short isSystem;
} Variable_;

typedef struct {
	void* function;
	int args;
	enum VarType* argTypes;
} Function_;

typedef struct
{
	char* file;
	char* str;
	short isFile;
	short type; // 0 - lexer, 1 - parser, 2 - interpretator
	unsigned short line;
	unsigned short pos;
} Environment;

#endif

