//
//  modeStack.h
//
//  26-May-95
//  pccts 1.32b5
//
//  These routines allow one to push and pop the #lexclass mode so that
//    #lexclass recognizers can be "called" from another #lexclass and
//    be reused to some extent
//
//  Based on routines originally written by David Seidel
//                            (dave@numega.com or 75342.2034@compuserve.com)
//
//  To use these routines require changes to pccts/dlg/output.c as
//    outlined in notes/dlg/output_diff.c
//
//  will be copied into DLGLexer.h
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

protected:
   enum  		{MaxModeStack=10};
   int	 		modeStack[MaxModeStack];
					         /* stack of pointer to func */
   void 		(DLGLexer::*funcStack[MaxModeStack])();
   int  		stackDepth;	
public:
   void			pushMode(int newMode,void (DLGLexer::*func)());
   void			popMode();
   void			resetModeStack();
   void			popOnSep();
