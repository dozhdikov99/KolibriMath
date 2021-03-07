/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/
#include "lexer.h"
#include <ctype.h>
#include "cvector.h"

int pos = 0;
int line_pos = 0;
char* _string;
Environment* environment = NULL;
const char* errors2[] = {"название переменной и функции не должно начинаться с цифры [код: 2.0].", "неожиданный символ [код: 2.1].", "ошибка выделения памяти [код: 2.2].", "ожидалась цифра или \'.\' [код: 2.3]."};

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
	line_pos++;
	(*environment).pos = line_pos;
}

short IsSep() {
	return current() == ',' || current() == '\0' || current() == '(' || current() == ')' || current() == '=' || current() == '+' || current() == '-' || current() == '*' || current() == '/' || current() == ' ' || current() == '#' || current() == '\n' || current() == '\r' || current() == ';' || current() != '{' || current() != '}' || current() != '&' || current() != '|' || current() != '!' || current() != '<' || current() != '>';
}

Token* GetName() {
	char str[MAX_EXPRESSION];
	Token* token = (Token*)malloc(sizeof(Token));
	if (token == NULL) {
		error2(errors2[2]);
		return NULL;
	}
	token->coord = line_pos;
	token->line = (*environment).line;
	int i = 0;
	while (current() != ',' && current() != '\0' && current() != '(' && current() != ')' && current() != '=' && current() != '+' && current() != '-' && current() != '*' && current() != '/' && current() != ' ' && current() != '#' && current() != '\n' && current() != '\r' && current() != ';' && current() != '{' && current() != '}' && current() != '&' && current() != '|' && current() != '!' && current() != '<' && current() != '>') {
		if (isalpha(current())) {
			str[i] = current();
			i++;
		}
		else if (isdigit(current())) {
			if (i == 0) {
				error(errors2[0]);
				free(token);
				return NULL;
			}
			else {
				str[i] = current();
				i++;
			}
		}
		else if (!IsSep()) {
			error(errors2[1]);
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
		error2(errors2[2]);
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
		error2(errors2[2]);
		return NULL;
	}
	token->coord = line_pos-1;
	token->line = (*environment).line;
	int i = 0;
	while (current() != '\'') {
		str[i] = current();
		i++;
		NextCh();
		if (current() == '\0') {
			error(errors2[1]);
			return NULL;
		}
	}
	str[i] = '\0';
	char* p = malloc(sizeof(char) * (strlen(str) + 1));
	if (p == NULL) {
		error2(errors2[2]);
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
		error2(errors2[2]);
		return NULL;
	}
	token->coord = line_pos;
	token->line = (*environment).line;
	int count = 0;
	int i = 0;
	while (current() != ',' && current() != '\0' && current() != '(' && current() != ')' && current() != '=' && current() != '+' && current() != '-' && current() != '*' && current() != '/' && current() != ' ' && current() != '#' && current() != '\n' && current() != '\r' && current() != ';' && current() != '{' && current() != '}' && current() != '&' && current() != '|' && current() != '!' && current() != '<' && current() != '>') {
		if (isalpha(current())) {
			error(errors2[3]);
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
				error(errors2[1]);
				free(token);
				return NULL;
			}
		}
		else if (isdigit(current())) {
			str[i] = current();
			i++;
		}
		else if (!IsSep()) {
			error(errors2[1]);
			free(token);
			return NULL;
		}
		NextCh();
	}
	str[i] = '\0';
	char* p = malloc(sizeof(char) * strlen(str));
	if (p == NULL) {
		error2(errors2[2]);
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
	environment = getCurrent();
	environment->str = string;
    (*environment).type = 0;
	_string = string;
	cvector* lexems = (cvector*)malloc(sizeof(cvector));
	if (lexems == NULL) {
		error2(errors2[2]);
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
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->coord = line_pos;
				token->line = (*environment).line;
				token->data = "(\0";
				token->type = TokenType_LeftBracket;
				last = TokenType_LeftBracket;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ')') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = ")\0";
				token->type = TokenType_RightBracket;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_RightBracket;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '=') {
				if (last >= 0 && last == TokenType_OpEquals) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				NextCh();
				if (current() != '=') {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "=\0";
					token->type = TokenType_OpEquals;
					last = TokenType_OpEquals;
					token->coord = line_pos;
					token->line = (*environment).line;
					cvector_push_back(lexems, token);
					continue;
				}
				else {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "==\0";
					token->type = TokenType_Equals;
					last = TokenType_Equals;
					token->coord = line_pos - 1;
					token->line = (*environment).line;
				}
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '+') {
				if (last >= 0 && last == TokenType_OpAdd) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "+\0";
				token->type = TokenType_OpAdd;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_OpAdd;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '-') {
				if (last >= 0 && last == TokenType_OpSub) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "-\0";
				token->type = TokenType_OpSub;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_OpSub;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '/') {
				if (last >= 0 && last == TokenType_OpDiv) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "/\0";
				token->type = TokenType_OpDiv;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_OpDiv;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '*') {
				if (last >= 0 && last == TokenType_OpMul) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "*\0";
				token->type = TokenType_OpMul;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_OpMul;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ' ') {
				NextCh();
			}
			else if (current() == ',') {
				if (last >= 0 && last == TokenType_Sep) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = ",\0";
				token->type = TokenType_Sep;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_Sep;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == ';') {
				if (last >= 0 && last == TokenType_NewLineSep) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = ";\0";
				token->type = TokenType_NewLineSep;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_NewLineSep;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '&') {
				if (last >= 0 && last == TokenType_And) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "&\0";
				token->type = TokenType_And;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_And;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '|') {
				if (last >= 0 && last == TokenType_Or) {
					error(errors2[1]);
					result = -1;
					break;
				}
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "|\0";
				token->type = TokenType_Or;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_Or;
				cvector_push_back(lexems, token);
				NextCh();
			}
			else if (current() == '!') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "!=\0";
					token->type = TokenType_NotEquals;
					token->coord = line_pos - 1;
					token->line = (*environment).line;
					last = TokenType_NotEquals;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "!\0";
					token->type = TokenType_Inverse;
					token->coord = line_pos;
					token->line = (*environment).line;
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
					error2(errors2[2]);
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "<=\0";
					token->type = TokenType_EqualsOrLess;
					token->coord = line_pos - 1;
					token->line = (*environment).line;
					last = TokenType_EqualsOrLess;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "<\0";
					token->type = TokenType_Less;
					token->coord = line_pos;
					token->line = (*environment).line;
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
					error2(errors2[2]);
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "<=\0";
					token->type = TokenType_EqualsOrLess;
					token->coord = line_pos - 1;
					token->line = (*environment).line;
					last = TokenType_EqualsOrLess;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = "<\0";
					token->type = TokenType_Less;
					token->coord = line_pos;
					token->line = (*environment).line;
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
					error2(errors2[2]);
					result = -1;
					break;
				}
				NextCh();
				if (current() == '=') {
					token->data = malloc(sizeof(char) * 2);
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = ">=\0";
					token->type = TokenType_EqualsOrMore;
					token->coord = line_pos - 1;
					token->line = (*environment).line;
					last = TokenType_EqualsOrMore;
				}
				else {
					token->data = malloc(sizeof(char));
					if (token->data == NULL) {
						error2(errors2[2]);
						free(token);
						result = -1;
						break;
					}
					token->data = ">\0";
					token->type = TokenType_More;
					token->coord = line_pos;
					token->line = (*environment).line;
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
				error2(errors2[2]);
				result = -1;
				break;
			}
			token->data = malloc(sizeof(char));
			if (token->data == NULL) {
				error2(errors2[2]);
				free(token);
				result = -1;
				break;
			}
			token->data = "{\0";
			token->type = TokenType_LeftFigureBracket;
			token->coord = line_pos;
			token->line = (*environment).line;
			last = TokenType_LeftFigureBracket;
			cvector_push_back(lexems, token);
			NextCh();
			}
			else if (current() == '}') {
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "}\0";
				token->type = TokenType_RightFigureBracket;
				token->coord = line_pos;
				token->line = (*environment).line;
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
			else if (current() == '\r') {
				NextCh();
			}else if(current() == '\n'){
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "\n\0";
				token->type = TokenType_NewLine;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_NewLine;
				cvector_push_back(lexems, token);
				NextCh();
				(*environment).line++;
				(*environment).pos = 1;

				line_pos = 1;
			}
			else {
				error(errors2[1]);
				result = -1;
				break;
			}
		}
		else {
			if(current() == '\n'){
				token = (Token*)malloc(sizeof(Token));
				if (token == NULL) {
					error2(errors2[2]);
					result = -1;
					break;
				}
				token->data = malloc(sizeof(char));
				if (token->data == NULL) {
					error2(errors2[2]);
					free(token);
					result = -1;
					break;
				}
				token->data = "\n\0";
				token->type = TokenType_NewLine;
				token->coord = line_pos;
				token->line = (*environment).line;
				last = TokenType_NewLine;
				cvector_push_back(lexems, token);
				(*environment).line++;
				(*environment).pos = 1;
				line_pos = 1;
			}
			NextCh();
		}
	}
	pos = 0;
	line_pos = 0;
	if (result != -1 && cvector_size(lexems) != 0) {
		Array* array = (Array*)malloc(sizeof(Array));
		if (array == NULL) {
			error2(errors2[2]);
			freeLexems(lexems);
			cvector_free(lexems);
			free(lexems);
			return NULL;
		}
		array->data = (Token*)malloc(sizeof(Token) * (cvector_size(lexems) + 1));
		if (array->data == NULL) {
			error2(errors2[2]);
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
			error2(errors2[2]);
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