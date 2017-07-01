//
//  fileToken.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "noleakTok.h"
#include "fileToken.h"
#include "string.h"
#include "myPanic.h"
#include <stdio.h>
#include "P.h"

void FileToken::init() {
  pText=0;
  dlgLexer=0;
}

FileToken::FileToken() {
  init();
  setLine(0);
}

FileToken::~FileToken() {
////   dumpNode("Token Destructor");
   delete [] pText;
   pText=0;
}

FileToken::FileToken(ANTLRTokenType tokenTypeNew) {
   init();
   setType(tokenTypeNew);
   setLine(0);
}

FileToken::FileToken(ANTLRTokenType 	tokenTypeNew,
           		     ANTLRChar *		textNew,   
          	 	     int			    lineNew) {
   init();
   setType(tokenTypeNew);
   setLine(lineNew);
   setText(textNew);
}         

FileToken::FileToken(const FileToken & from) :
	NoLeakToken(from) {
   init();
   col=from.col;
   setText(from.pText);
   dlgLexer=from.dlgLexer;
};	

//  create new copy of text - not just another reference to existing text

FileToken & FileToken::operator = (const FileToken & from) {

   this->NoLeakToken::operator = (from);

   if (this != &from) {
     col=from.col;
     setText(from.pText);
     dlgLexer=from.dlgLexer;
   };
   return *this;
}      

//  create new copy of text - not just another reference to existing text

void FileToken::setText(const ANTLRChar *s) {

   if (pText != 0) {
      delete [] pText;
      pText=0;
   };
   if (s != 0) {
     pText=new char [strlen(s)+1];
     if (pText == 0) myPanic ("FileToken::setText strdup failed");
     strcpy(pText,s);
   };
}

ANTLRAbstractToken * FileToken::makeToken(ANTLRTokenType	tokenType,
					                       ANTLRChar *		text,
                        		 		   int			    line) {
   return new FileToken(tokenType,text,line);
}

void FileToken::dumpNode(const char * s) {

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
