//
//  calcex1.g
//
//  29-May-97
//  pccts 1.33MR5
//
//  simple calculator using tokens rather than ASTs for operators: + - * /
//
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

<<
#include "myToken.h"
#include "myAST.h"
#include "myPanic.h"
#include "myTokenBuffer.h"
#include "DLGLexer.h"

int main() {

   {

   DLGFileInput		in(stdin);
   DLGLexer		lexer(&in,2000);
   MyTokenBuffer	pipe(&lexer,1);
   ANTLRToken		aToken( (ANTLRTokenType)0 );
   P			parser(&pipe);
   ASTBase *		root(0);
   AST *		rootCopy;

   lexer.setToken(&aToken);
   AST::clearList();		// remove statically constructed ASTs
   AST::clearCounter();
   parser.init();
   parser.statement(&root);
   rootCopy=(AST *) root;

   if (rootCopy != 0) {
////     rootCopy->preorder();
////     rootCopy->dumpTree();
     rootCopy->destroy();
   };
////   AST::dumpList();
   AST::destroyList();

   };

   fclose(stdin);
   fclose(stdout);
   return 0;
}
>>

#token	Eof	"@"

#token  InvalidNumber

#token	Number	"[.0-9]+{[dDfFlLeE]{\+\-}[0-9]+}"
		<<double	d;
		  char *	endptr;
		  d=strtod(_lextext,&endptr);
		  if (*endptr != 0) {
		     return InvalidNumber;
		  };
		>>
#token	WS	"[\ \t]"	<<skip();
				 if (*_lextext=='\t') {
				   _endcol=((_endcol-1) & ~7) + 8;
				   // DLG will add 1 more because it
				   //   assumes \t is a printing character
				   // See wstabadjust.cpp for ws strings
				 };
				>>
#token	NL	"\n"		<<newline();set_endcol(0);>>

class P {

<<
public:
      virtual void syn(
	      ANTLRAbstractToken *	tok,
	      ANTLRChar *		egroup,
	      SetWordType *		eset,
	      ANTLRTokenType		etok,
	      int			k);
      double	result;
>>

statement : ( expr1 > [result]
			<<printf("Result=%f\n",result);>>
	      NL)* "@"
;
expr1 > [double r] : <<double b;>>
	      expr2 > [$r]
			 ( "\+"^ expr2 > [b] <<$r+=b;>>
			 | "\-"^ expr2 > [b] <<$r-=b;>>
			 ) *
;
expr2 > [double r] : <<double b;>>
	      expr3 > [$r] 
			( "\*"^ expr3 > [b] <<$r*=b;>>
			| "\/"^ expr3 > [b] <<$r/=b;>>
			) *
;
expr3 > [double r] : n:Number	<<$r=mytoken($n)->number;>>
		   | "\(" expr1 > [$r] "\)"
;

}
