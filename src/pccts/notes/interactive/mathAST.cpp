//
//  mathAST.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "mathAST.h"

double Add::eval() {
	number=ASTdown()->eval()+ASTdown()->ASTright()->eval();
	return number;
};
double Sub::eval() {
	number=ASTdown()->eval()-ASTdown()->ASTright()->eval();
	return number;
};
double Mul::eval() {
	number=ASTdown()->eval()*ASTdown()->ASTright()->eval();
	return number;
};
double Div::eval() {
	number=ASTdown()->eval()/ASTdown()->ASTright()->eval();
	return number;
};

const char * Add::className() {return "Add";};
const char * Sub::className() {return "Sub";};
const char * Mul::className() {return "Mul";};
const char * Div::className() {return "Div";};
