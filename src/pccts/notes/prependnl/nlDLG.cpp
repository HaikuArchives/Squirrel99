//
//  nlDLG.cpp
//  9-May-1995
//  pccts 1.32b5
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "nlDLG.h"
#include "prependnl.h"

MyDLG::MyDLG(DLGInputStream *userStream,unsigned bufsize) :
	DLGLexer(userStream,bufsize),
        prependNLinputStream(userStream) {
  _line--;				// compensate for extra newline
 					//   added by prependNL
  input=&prependNLinputStream;		// replace users input stream
					//   with the one which prepends
					//   a newline and then call the
					//   user specified version
}

MyDLG::~MyDLG() {}

PrependNLinputStream::PrependNLinputStream(DLGInputStream * in) :
   usersInputStream(in),
   firstTime(1) {}

PrependNLinputStream::~PrependNLinputStream() {
///   delete usersInputStream;
}

//
//  this always adds a newline, even if the first call returns an EOF
//  after some thought I decided that eliminating the first newline
//    might prevent some lexical routines from working correctly
//
//  it would be nice to replace the PrependNLinputStream with the user's
//    input stream pointer after the first character is fetched so as
//    to reduce overhead.  However a DLGinputStream doesn't know which
//    DLGLexer called it.  The MyDLG::getToken could do it, and this
//    would replace a per/character subroutine call with a per/token
//    subroutine call.

int PrependNLinputStream::nextChar() {

  int		result;

  if (firstTime) {
     firstTime=0;
     result='\n';
  } else {
     result=usersInputStream->nextChar();
  };
  return result;
}
