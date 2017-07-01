//
//  modeStack.cpp
//
//  26-May-95
//  pccts 1.32b5
//
//  These routines allow one to push and pop the #lexclass mode so that
//    #lexclass recognizers can be "called" from another #lexclass and
//    be reused to some extent
//
//  Based on routines originally written by David Seidel
//                           (dave@numega.com or 75342.2034@compuserve.com)
//
//  To use these routines require changes to pccts/dlg/output.c as
//    outlined in notes/dlg/output_diff.c
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include <stdio.h>
#include "tokens.h"
#include "DLGLexer.h"

void DLGLexer::pushMode(int newMode,void (DLGLexer::*func)()) {
   if(stackDepth == (MaxModeStack - 1) ) {
      panic("Mode stack overflow ");
   } else {
      modeStack[stackDepth] = automaton;	// getMode() in next version
      funcStack[stackDepth] = func;
      stackDepth++;
      mode(newMode);
   };
}

void DLGLexer::popMode() {

   void (DLGLexer::*thisFunc)();

   if(stackDepth <= 0) {
     panic("Mode stack underflow ");
   } else {
      stackDepth--;
      thisFunc=funcStack[stackDepth];
      mode(modeStack[stackDepth]);
      modeStack[stackDepth]=0;
      funcStack[stackDepth]=0;
	/* this call might result in indirect recursion of popMode() */
      if (thisFunc != 0) {
	(void) (this->*thisFunc)();
      };
   };
}

void DLGLexer::resetModeStack() {
   stackDepth=0;
   modeStack[0]=0;
   funcStack[0]=0;
}
