#ifndef MYAST_H
#define MYAST_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "ASTBase.h"
#include "noleakAST.h"
#include "myToken.h"
#include "ATokPtr.h"

class AST : public NoLeakAST {

  public:
       /* constructor */	AST();
       /* constructor */	AST(ANTLRTokenPtr t);
       /* destructor */ virtual	~AST();
       /* copy constructor */	AST(const AST &);	   // new copy of token
       AST &			operator = (const AST &);  // new copy of token
       virtual void		dumpNode(const char * s=0);
       virtual void		preorder_action();

       virtual double		eval();
       double			number;
       ANTLRTokenPtr		pToken;

       AST *			ASTdown() {return (AST *)_down;};
       AST *			ASTright() {return (AST *)_right;};

       virtual const char *	className();
};

#endif
