//
//  myTokenBuffer.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "myTokenBuffer.h"
#include "myToken.h"
#include "DLexerBase.h"

MyTokenBuffer::MyTokenBuffer(ANTLRTokenStream * in,
			     int		k,
			     int		chksz) :
   ANTLRTokenBuffer(in,k,chksz) {}

int     isa (char *s ) {
	int i=(*s=='a');
	return i;
}

int     isb (char *s) {
	int i=(*s=='b');
	return i;}

ANTLRAbstractToken * MyTokenBuffer::getToken() {

   ANTLRToken *		myToken;

   myToken=(ANTLRToken *)ANTLRTokenBuffer::getToken();

   if (myToken->getType() == A_OR_B) {
      if (isa(myToken->getText())) {
	myToken->setType(A);
      } else if (isb(myToken->getText())) {
	myToken->setType(B);
      } else {
        //nothing
      };
   };
   return myToken;
}
