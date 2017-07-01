#ifndef YY_PROTOS_H
#define YY_PROTOS_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */
 
#include <stdio.h>

void		yyrestart (FILE *);
int		yylex();
extern char *	yytext;

int		getLexClass();
void		setLexClass(int);
const char *	getLexClassName(int);
void		freeFlexBuffer();

#endif
