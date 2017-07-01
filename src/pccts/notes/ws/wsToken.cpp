//
//  wsToken.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "wsToken.h"
#include "myPanic.h"
#include <stdio.h>
#include "P.h"

void ANTLRToken::init() {
  pText=0;
  ws=0;
  col=0;
}

ANTLRToken::ANTLRToken() {
  init();
}

ANTLRToken::~ANTLRToken() {
////   dumpNode("ANTLRToken Destructor");
   delete [] pText;
   pText=0;
   delete [] ws;
   ws=0;
}

ANTLRToken::ANTLRToken(ANTLRTokenType tokenTypeNew) {
   setType(tokenTypeNew);
   setLine(0);
   init();
}

ANTLRToken::ANTLRToken(ANTLRTokenType 	 tokenTypeNew,
            		   const ANTLRChar * textNew,   
                       int		         lineNew) {
   setType(tokenTypeNew);
   setLine(lineNew);
   init();
   setText(textNew);
}         

ANTLRToken::ANTLRToken(const ANTLRToken & from) :
	SimpleToken(from) {
   init();
   col=from.col;
   setText(from.pText);
   setws(from.ws);
};	

//  create new copy of text - not just another reference to existing text

ANTLRToken & ANTLRToken::operator = (const ANTLRToken & from) {
   this->SimpleToken::operator = (from);
   if (this != &from) {
     col=from.col;
     setText(from.pText);
     setws(from.ws);
   };
   return *this;
}      

//  create new copy of text - not just another reference to existing text

void ANTLRToken::setText(const ANTLRChar *s) {

   if (pText != 0) {
      delete [] pText;
      pText=0;
   };
   if (s != 0) {
     pText=new ANTLRChar [strlen(s)+1];
     if (pText == 0) myPanic ("ANTLRToken::setText strdup failed");
     strcpy(pText,s);
   };
}

void ANTLRToken::setws(ANTLRChar *s) {

   if (ws != 0) {
      delete [] ws;
      ws=0;
   };
   if (s != 0) {
     ws=new ANTLRChar [strlen(s)+1];
     if (ws == 0) myPanic ("ANTLRToken::setws strdup failed");
     strcpy(ws,s);
   };
}

ANTLRAbstractToken * ANTLRToken::makeToken(ANTLRTokenType   tokenType,
                    					   ANTLRChar *	    text,
                    					   int		        line) {
   return new ANTLRToken(tokenType,text,line);
}

void ANTLRToken::dumpNode(const char * s) {

   ANTLRChar * 	theText;
   ANTLRChar *	theWS;
   ANTLRChar *  p;

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
     if (ws != 0) {
       theWS=new ANTLRChar [strlen(ws)+1];
       if (theWS == 0) myPanic ("ANTLRToken::dumpNode strdup failed");
       strcpy(theWS,ws);
       for (p=theWS;*p != 0;p++) {
          if (*p == ' ') *p='.';
          if (*p == '\n') *p='$';
       };
     };
     printf("TokenType (%s) Text (%s) WS (%s) Token # %d  Line=%d  Col=%d\n",
		P::tokenName(getType()),
		theText,
		theWS,
		serial,		
		getLine(),
		col);
     delete [] theWS;
   };
}
