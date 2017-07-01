//
//  myAST.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "myAST.h"
#include <string.h>
#include <stdio.h>
#include "P.h"

AST::AST() :
        pToken(0),
	number(0.0) {}
	
AST::AST(ANTLRTokenPtr tokenNew) : 
	pToken(tokenNew) {}

AST::~AST() {
////  dumpNode("AST Destructor");
   pToken=0;
};

// duplicates node, not entire tree so it may create multiple references
//   to subtree nodes
// creates new copy of token - not just a reference to the old token

AST::AST(const AST & from) :
	NoLeakAST(from) {

   ANTLRToken *		t;

   t=(ANTLRToken *)(from.pToken.operator ->());
   pToken=ANTLRTokenPtr(new ANTLRToken(*t));
}

// duplicates node, not entire tree so it may create multiple references
//   to subtree nodes
// creates new copy of token - not just a reference to the old token

AST & AST::operator = (const AST & from) {

   ANTLRToken *		t;

   this->NoLeakAST::operator = (from);
   if (this != &from) {
      t=(ANTLRToken *)(from.pToken.operator ->());
      pToken=ANTLRTokenPtr(new ANTLRToken(*t));
   };
   return *this;
}

void AST::preorder_action() {

   ANTLRChar *		p=0;
   ANTLRChar 		string[32];
  
   if (pToken == 0) {
      sprintf(string,"%f",number);
   } else if (pToken->getType() != Eof) {
     p=mytoken(pToken)->pText;
     if (p == 0) {
       p="";
     } else if (strcmp(p,"\n") == 0) {
       p="NL";
     };
   };
   if (p != 0) printf(" %s <#%d>",p,serial);
}

void AST::dumpNode(const char * s) {

   ANTLRChar *		p;
   ANTLRChar 		string[32];
   ANTLRToken *		qToken(mytoken(pToken));

   if (s != 0) {printf("%s ",s);};
   if (qToken == 0) {
      sprintf(string,"%f",number);
      printf("TokenType (%s) Value (%s) AST # %d\n",
		className(),
		string,
		serial);
   } else if (qToken->getType() == Eof) {
      printf("TokenType (%s) AST # %d\n",
		P::tokenName(qToken->getType()),
		serial);
   } else if (qToken->getType() == Number) {
      sprintf(string,"%f",qToken->number);
      printf("TokenType (%s) Value (%s) AST # %d  Line=%d  Col=%d\n",
		P::tokenName(qToken->getType()),
		string,
		serial,		
		qToken->getLine(),
		qToken->col);
   } else {
     p=qToken->pText;
     if (p == 0) {
       p="";
     } else if (strcmp(p,"\n") == 0) {
       p="NL";
     };
     printf("TokenType (%s) Text (%s) AST # %d  Line=%d  Col=%d\n",
		P::tokenName(qToken->getType()),
		p,
		serial,		
		qToken->getLine(),
		qToken->col);
   };
}

double AST::eval() {

   double	result;

   if (pToken != 0) {
      result=mytoken(pToken)->number;
   } else {
      result=number;
   };
   return result;
}	

const char * AST::className() {return "AST";};
