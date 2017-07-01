//
//  myAST.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "myAST.h"
#include <string.h>
#include <stdio.h>
#include "P.h"

AST::AST(ANTLRTokenPtr tokenNew) : 
	pToken(tokenNew) {}

AST::~AST() {

//   ANTLRToken *		q;	// using reference counted tokens

////  dumpNode("AST Destructor");

///////////////////////////////////////////////////////////////////////////////
//
//  use this only if ANTLRParser garbage collect tokens is disabled
//    otherwise the body of this function can just be empty
//  when garbage collection of tokens is disabled then destruction of
//    an ANTLRTokenPtr is a no-op - thus it must be  destroyed explicitly
//
//
//   q=mytoken(pToken);			// using reference counted tokens
//   pToken=(ANTLRAbstractToken *)0;	// using reference counted tokens
//   delete q;				// using reference counted tokens

};

// duplicates node, not entire tree so it may create multiple references
//   to subtree nodes
// creates new copy of token - not just a reference to the old token

AST::AST(const AST & from) :
	NoLeakAST(from) {

   ANTLRToken *		t;
   ANTLRToken *		q;

   t=mytoken(from.pToken);
   q=mytoken(pToken);
   pToken=ANTLRTokenPtr(new ANTLRToken(*t));
   delete q;
}

// duplicates node, not entire tree so it may create multiple references
//   to subtree nodes
// creates new copy of token - not just a reference to the old token

AST & AST::operator = (const AST & from) {

   ANTLRToken *		t;

   this->NoLeakAST::operator = (from);
   if (this != &from) {
        t=mytoken(from.pToken);

///////////////////////////////////////////////////////////////////////////////
//
//  use this only if ANTLRParser garbage collect tokens is disabled
//    otherwise the body of this function can just be empty
//  when garbage collection of tokens is disabled then destruction of
//    an ANTLRTokenPtr is a no-op - thus it must be  destroyed explicitly

        delete mytoken(pToken);

///////////////////////////////////////////////////////////////////////////////

        pToken=ANTLRTokenPtr(new ANTLRToken(*t));
   };
   return *this;
}

void AST::preorder_action() {

   ANTLRChar *		p;

   if (pToken->getType() != Eof) {
     p=mytoken(pToken)->pText;
     if (p == 0) {
       p="";
     } else if (strcmp(p,"\n") == 0) {
       p="NL";
     };
     printf(" %s <#%d>",p,serial);
   };
}

void AST::dumpNode(const char * s) {

   ANTLRChar *		p;

   if (s != 0) {printf("%s ",s);};
   if (pToken->getType() == Eof) {
///     printf("TokenType (%s) AST # %d\n",
///		P::tokenName(pToken->getType()),
///		serial);
   } else {
     p=mytoken(pToken)->pText;
     if (p == 0) {
       p="";
     } else if (strcmp(p,"\n") == 0) {
       p="NL";
     };
///     printf("TokenType (%s) Text (%s) AST # %d  Line=%d  Col=%d\n",
///		P::tokenName(pToken->getType()),
///		p,
///		serial,		
///		pToken->getLine(),
///		pToken->col);
     printf("%s\n",p);
   };
}
