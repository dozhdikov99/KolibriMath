/*
* Writen by Dozhdikov Igor (igoru99@gmail.com)
* License: GNU GPL v2.0
* (C) 2021 year.
*/

#include "utils.h"

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