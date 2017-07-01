//
//  myTokenBuffer.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "myToken.h"
#include "myTokenBuffer.h"
#include "DLexerBase.h"

MyTokenBuffer::MyTokenBuffer(ANTLRTokenStream * in,
			     int		k,
			     int		chksz) :
   ANTLRTokenBuffer(in,k,chksz) {

   ( (DLGLexerBase *)input )->trackColumns();
}

ANTLRAbstractToken * MyTokenBuffer::getANTLRToken() {

   ANTLRToken *		myToken;
   myToken=(ANTLRToken *)ANTLRTokenBuffer::getANTLRToken();
   myToken->col=( (DLGLexerBase *)input ) ->begcol();
   return myToken;
}
