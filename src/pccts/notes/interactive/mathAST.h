#ifndef MATHAST_H
#define MATHAST_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "myAST.h"

struct Add : public AST {
      virtual double 		eval();
      virtual const char *	className();
};
struct Sub : public AST {
      virtual double 		eval();
      virtual const char *	className();
};
struct Mul : public AST {
      virtual double 		eval();
      virtual const char *	className();
};
struct Div : public AST {
      virtual double 		eval();
      virtual const char *	className();
};

#endif
