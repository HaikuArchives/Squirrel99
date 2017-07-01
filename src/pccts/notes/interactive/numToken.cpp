//
//  numToken.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include <stdio.h>
#include <string.h>

#include "numToken.h"
#include "myPanic.h"
#include "P.h"

void NumToken::init() {
  pText=0;
  number=0.0;
}

NumToken::NumToken() {
  init();
  setLine(0);
}

NumToken::~NumToken() {
////   dumpNode("Token Destructor");
   delete [] pText;
   pText=0;
}

NumToken::NumToken(ANTLRTokenType tokenTypeNew) {
   init();
   setType(tokenTypeNew);
   setLine(0);
}

NumToken::NumToken(ANTLRTokenType  tokenTypeNew,
		   ANTLRChar *	   textNew,   
		   int		   lineNew) {

   char *		endptr;

   init();
   setType(tokenTypeNew);
   setLine(lineNew);
   setText(textNew);
   number=strtod(textNew,&endptr);
}         

NumToken::NumToken(const NumToken & from) :
	SimpleToken(from) {
   init();
   col=from.col;
   number=from.number;
   setText(from.pText);
};	

//  create new copy of text - not just another reference to existing text

NumToken & NumToken::operator = (const NumToken & from) {

   this->SimpleToken::operator = (from);

   if (this != &from) {
     col=from.col;
     number=from.number;
     setText(from.pText);
   };
   return *this;
}      

//  create new copy of text - not just another reference to existing text

void NumToken::setText(const ANTLRChar *s) {

   if (pText != 0) {
      delete [] pText;
      pText=0;
   };
   if (s != 0) {
     pText=new char [strlen(s)+1];
     if (pText == 0) myPanic ("NumToken::setText strdup failed");
     strcpy(pText,s);
   };
}

ANTLRAbstractToken * NumToken::makeToken(ANTLRTokenType    tokenType,
					 ANTLRChar *	   text,
					 int		   line) {
   return new NumToken(tokenType,text,line);
}

void NumToken::dumpNode(const char * s) {

   char * theText;

   if (s != 0) {printf("%s ",s);};
   if (getType() == Eof) {
     printf("TokenType \"EOF\" Token # %d\n",serial);
   } else {
     if (pText == 0) {
        theText="";
     } else if (strcmp(pText,"\n") == 0) {
	theText="NL";
     } else {
	theText=pText;
     };	
     printf("TokenType (%s) Text (%s) Token # %d  Line=%d  Col=%d\n",
		P::tokenName(getType()),
		theText,
		serial,		
		getLine(),
		col);
   };
}
