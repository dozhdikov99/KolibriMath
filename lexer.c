/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "lexer.h"
#include <ctype.h>
#include "cvector.h"

int pos = 0;
char* _string;

void freeToken(Token* token) {
	free(token);
}

void freeLexems(cvector* lexems) {
	for (int i = 0; i < cvector_size(lexems); i++) {
		freeToken(cvector_get(lexems, i));
	}
}

void freeArray(Array* array) {
	for (int i = 0; i < array->size; i++) {
		freeToken((Token*)(array->data) + i);
	}
	free(array);
}

char current() {
	return _string[pos];
}

void NextCh() {
	pos++;
}

short IsSep() {
	return current() == ',' || current() == '\0' || current() == '(' || current() == ')' || current() == '=' || current() == '+' || current() == '-' || current() == '*' || current() == '/' || current() == ' ' || current() == '#' || current() == '\n' || current() == '\r' || current() == ';' || current() != '{' || current() != '}' || current() != '&' || current() != '|' || current() != '!' || current() != '<' || current() != '>';
}

Token* GetName() {
	char str[MAX_EXPRESSION];
	Token* token = (Token*)malloc(sizeof(Token));
	if (token == NULL) {
		error2("Ошибка выделения памяти.");
		return NULL;
	}
	token->coord = pos;
	int i = 0;
	while (current() != ',' && current() != '\0' && current() != '(' && current() != ')' && current() != '=' && current() != '+' && current() != '-' && current() != '*' && current() != '/' && current() != ' ' && current() != '#' && current() != '\n' && current() != '\r' && current() != ';' && current() != '{' && current() != '}' && current() != '&' && current() != '|' && current() != '!' && current() != '<' && current() != '>') {
		if (isalpha(current())) {
			str[i] = current();
			i++;
		}
		else if (isdigit(current())) {
			if (i == 0) {
				error("Название переменной и функции не должно начинаться с цифры.", pos);
				free(token);
				return NULL;
			}
			else {
				str[i] = current();
				i++;
			}
		}
		else if (!IsSep()) {
			error("Недопустимый символ.", pos);
			free(token);
			return NULL;
		}
		NextCh();
	}
	str[i] = '\0';
	if (i == 3 && !strcmp(str, "del")) {
		token->type = TokenType_Del;
	}
	else if (i == 5 && !strcmp(str, "false")) {
		token->type = TokenType_False;
	}
	else if (i == 4 && !strcmp(str, "true")) {
		token->type = TokenType_True;
	}
	else if (i == 2 && !strcmp(str, "if")) {
		token->type = TokenType_If;
	}
	else if (i == 6 && !strcmp(str, "repeat")) {
		token->type = TokenType_Repeat;
	}else if (i == 4 && !strcmp(str, "elif")) {
		token->type = TokenType_Elif;
	}
	else if (i == 4 && !strcmp(str, "else") && current() == '{') {
		token->type = TokenType_Else;
	}
	else {
		token->type = TokenType_Name;
	}
	char* p = malloc(sizeof(char) * strlen(str));
	if (p == NULL) {
		error2("Ошибка выделения памяти.");
		if (token != NULL) {
			free(token);
		}
		return NULL;
	}
	for (int j = 0; j < strlen(str); j++) {
		p[j] = str[j];
	}
	p[i] = '\0';
	token->data = p;
	return token;
}

Token* GetStr() {
	char str[MAX_EXPRESSION];
	Token* token = (Token*)malloc(sizeof(Token));
	if (token == NULL) {
		error2("Ошибка выделения памяти.");
		return NULL;
	}
	token->coord = pos;
	int i = 0;
	while (current() != '\'') {
		str[i] = current();
		i++;
		NextCh();
		if (current() == '\0') {
			error("Неожиданный конец выражения.", pos);
			return NULL;
		}
	}
	str[i] = '\0';
	char* p = malloc(sizeof(char) * (strlen(str) + 1));
	if (p == NULL) {
		error2("Ошибка выделения памяти.");
		if (token != NULL) {
			free(token);
		}
		return NULL;
	}
	for (int j = 0; j < strlen(str) + 1; j++) {
		p[j] = str[j];
	}
	p[i] = '\0';
	token->data = p;
	token->type = TokenType_Str;
	return token;
}


Token* GetNumber() {
	char str[MAX_EXPRESSION];
	Token* token = (Token*)malloc(sizeof(Token));
	if (token == NULL) {
		error2("Ошибка выделения памяти.");
		return NULL;
	}
	token->coord = pos;
	int count = 0;
	int i = 0;
	while (current() != ',' && current() != '\0' && current() != '(' && current() != ')' && current() != '=' && current() != '+' && current() != '-' && current() != '*' && current() != '/' && current() != ' ' && current() != '#' && current() != '\n' && current() != '\r' && current() != ';' && current() != '{' && current() != '}' && current() != '&' && current() != '|' && current() != '!' && current() != '<' && current() != '>') {
		if (isalpha(current())) {
			error("Ожидалась цифра или \'.\'.", pos);
			free(token);
			return NULL;
		}
		else if (current() == '.') {
			if (count == 0) {
				str[i] = current();
				i++;
				count++;
			}
			else {
				error("Недопустимый символ \'.\'.", pos);
				free(token);
				return NULL;
			}
		}
		else if (isdigit(current())) {
			str[i] = current();
			i++;
		}
		else if (!IsSep()) {
			error("Недопустимый символ.", pos);
			free(token);
			return NULL;
		}
		NextCh();
	}
	str[i] = '\0';
	char* p = malloc(sizeof(char) * strlen(str));
	if (p == NULL) {
		error2("Ошибка выделения памяти.");
		if (token != NULL) {
			free(token);
		}
		return NULL;
	}
	for (int j = 0; j < strlen(str); j++) {
		p[j] = str[j];
	}
	p[i] = '\0';
	token->data = p;
	if (count != 0) {
		token->type = TokenType_Float;
	}
	else {
		token->type = TokenType_Int;
	}
	return token;
}

Array* tokenize(char* string) {
	_string = string;
	cvector* lexems = (cvector*)malloc(sizeof(cvector));
	if (lexems == NULL) {
		error2("Ошибка выделения памяти.");
		return NULL;
	}
	cvector_init(lexems);
	char lastSymbol;
	enum TokenType last = 10;
	Token* token = NULL;
	short result = 1;
	short isComment = 0;
	while (current() != '\0') {
		if (current() == '#') {
			if (isComment == 0) {
				isComment = 1;
			}
			else {
				isComment = 0;
				NextCh();
			}
		}
		if (current() == '\0') {
			break;
		}
		if (isComment == 0) {
			if (isdigit(current())) {
				token = GetNumber();
				if (token != NULL) {
					cvector_push_back(lexems, token);
					last = TokenType_Int;
				}
				else {
					result = -1;
					break;
				}
			}
			else if (isalpha(current())) {
				token = GetName();
				if (token != NULL) {
					cvector_push_back(lexems, token);
					last = TokenType_Name;
				}
				else {
					result = -1;
					break;
				}
			}
			else if (current() == '(') {
				if (last >= 0 && last == TokenType_Name && (pos > 0 && _string[pos - 1] == ' ')) {
					error("Неожиданный символ \'(\'.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->coord = pos;
				token->data = "(\0";
				token->type = TokenType_LeftBracket;
				last = TokenType_LeftBracket;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ')') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = ")\0";
				token->type = TokenType_RightBracket;
				token->coord = pos;
				last = TokenType_RightBracket;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '=') {
				if (last >= 0 && last == TokenType_OpEquals) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				NextCh();
				if (current() != '=') {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "=\0";
					token->type = TokenType_OpEquals;
					last = TokenType_OpEquals;
					token->coord = pos;
					cvector_push_back(lexems, token);
					continue;
				}
				else {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "==\0";
					token->type = TokenType_Equals;
					last = TokenType_Equals;
					token->coord = pos - 1;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '+') {
				if (last >= 0 && last == TokenType_OpAdd) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "+\0";
				token->type = TokenType_OpAdd;
				token->coord = pos;
				last = TokenType_OpAdd;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '-') {
				if (last >= 0 && last == TokenType_OpSub) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "-\0";
				token->type = TokenType_OpSub;
				token->coord = pos;
				last = TokenType_OpSub;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '/') {
				if (last >= 0 && last == TokenType_OpDiv) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "/\0";
				token->type = TokenType_OpDiv;
				token->coord = pos;
				last = TokenType_OpDiv;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '*') {
				if (last >= 0 && last == TokenType_OpMul) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "*\0";
				token->type = TokenType_OpMul;
				token->coord = pos;
				last = TokenType_OpMul;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ' ') {
				NextCh();
			}
			else if (current() == ',') {
				if (last >= 0 && last == TokenType_Sep) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = ",\0";
				token->type = TokenType_Sep;
				token->coord = pos;
				last = TokenType_Sep;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ';') {
				if (last >= 0 && last == TokenType_NewLineSep) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = ";\0";
				token->type = TokenType_NewLineSep;
				token->coord = pos;
				last = TokenType_NewLineSep;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '&') {
				if (last >= 0 && last == TokenType_And) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "&\0";
				token->type = TokenType_And;
				token->coord = pos;
				last = TokenType_And;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '|') {
				if (last >= 0 && last == TokenType_Or) {
					error("Неожиданный символ.", pos);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "|\0";
				token->type = TokenType_Or;
				token->coord = pos;
				last = TokenType_Or;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '!') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "!=\0";
					token->type = TokenType_NotEquals;
					token->coord = pos - 1;
					last = TokenType_NotEquals;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "!\0";
					token->type = TokenType_Inverse;
					token->coord = pos;
					last = TokenType_Inverse;
					cvector_push_back(lexems, token);
					continue;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '<') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "<=\0";
					token->type = TokenType_EqualsOrLess;
					token->coord = pos - 1;
					last = TokenType_EqualsOrLess;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "<\0";
					token->type = TokenType_Less;
					token->coord = pos;
					last = TokenType_Less;
					cvector_push_back(lexems, token);
					continue;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '<') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "<=\0";
					token->type = TokenType_EqualsOrLess;
					token->coord = pos - 1;
					last = TokenType_EqualsOrLess;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = "<\0";
					token->type = TokenType_Less;
					token->coord = pos;
					last = TokenType_Less;
					cvector_push_back(lexems, token);
					continue;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '>') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = ">=\0";
					token->type = TokenType_EqualsOrMore;
					token->coord = pos - 1;
					last = TokenType_EqualsOrMore;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2("Ошибка выделения памяти.");
						free(token);
						result = -1;
						break;
					}
					token->data = ">\0";
					token->type = TokenType_More;
					token->coord = pos;
					last = TokenType_More;
					cvector_push_back(lexems, token);
					continue;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '{') {
			token = (Token*)malloc(sizeof(Token));
			if (token == NULL) {
				error2("Ошибка выделения памяти.");
				result = -1;
				break;
			}
			token->data = malloc(sizeof(char));
			if (token->data == NULL) {
				error2("Ошибка выделения памяти.");
				free(token);
				result = -1;
				break;
			}
			token->data = "{\0";
			token->type = TokenType_LeftFigureBracket;
			token->coord = pos;
			last = TokenType_LeftFigureBracket;
			cvector_push_back(lexems, token);
			NextCh();
			}
			else if (current() == '}') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2("Ошибка выделения памяти.");
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2("Ошибка выделения памяти.");
					free(token);
					result = -1;
					break;
				}
				token->data = "}\0";
				token->type = TokenType_RightFigureBracket;
				token->coord = pos;
				last = TokenType_RightFigureBracket;
				cvector_push_back(lexems, token);
				NextCh();
				}
			else if (current() == '\'') {
				NextCh();
				token = GetStr();
				if (token != NULL) {
					cvector_push_back(lexems, token);
					NextCh();
					last = TokenType_Str;
				}
				else {
					result = -1;
					break;
				}
			}
			else if (current() == '\r' || current() == '\n') {
				NextCh();
			}
			else {
				error("Недопустимый символ.", pos);
				result = -1;
				break;
			}
		}
		else {
			NextCh();
		}
	}
	pos = 0;
	if (result != 0 && cvector_size(lexems) != 0) {
		Array* array = (Array*)malloc(sizeof(Array));
		if (array == NULL) {
			error2("Ошибка выделения памяти.");
			freeLexems(lexems);
			cvector_free(lexems);
			free(lexems);
			return NULL;
		}
		array->data = (Token*)malloc(sizeof(Token) * (cvector_size(lexems) + 1));
		if (array->data == NULL) {
			error2("Ошибка выделения памяти.");
			free(array);
			freeLexems(lexems);
			cvector_free(lexems);
			free(lexems);
			return NULL;
		}
		array->size = cvector_size(lexems) + 1;
		for (int i = 0; i < cvector_size(lexems); i++) {
			array->data[i] = *(Token*)cvector_get(lexems, i);
		}
		Token* token = (Token*)malloc(sizeof(Token));
		if (token == NULL) {
			error2("Ошибка выделения памяти.");
			free(array);
			freeLexems(lexems);
			cvector_free(lexems);
			free(lexems);
			return NULL;
		}
		token->data = NULL;
		token->type = TokenType_End;
		token->coord = strlen(_string);
		array->data[cvector_size(lexems)] = *token;
		cvector_free(lexems);
		free(lexems);
		lexems = NULL;
		return array;
	}
	else {
		freeLexems(lexems);
		cvector_free(lexems);
		free(lexems);
		lexems = NULL;
	}
	return NULL;
}