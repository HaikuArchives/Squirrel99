//
//  wsDLGLexer.h
//  will be copied into DLGLexer.h
//  requires changes to pccts/dlg/output.c as outlined in output.diff

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

protected:
   int			isWS;
   void			tabAdjust();
   int			begline;
