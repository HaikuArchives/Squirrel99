//
//  cli.g
//
//  2-Mar-98
//  pccts 1.33MR12
//
//  simple calculator using ASTs for operators: + - * /
//  demonstrates simple cli for interactive parser
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

<<
#include "myToken.h"
#include "myAST.h"
#include "mathAST.h"
#include "myPanic.h"
#include "myTokenBuffer.h"
#include "DLGLexer.h"

char *string;
int   lString=0;
int   maxString=0;

int getInputText(FILE *f) {

    int     c;
    char    *newString;
    int     newMaxString;

    lString=0;

    while ( (c=getc(f)) >= 0) {
      if (lString >= maxString) {
        newMaxString=(maxString+1)*2;
        newString=new char[newMaxString+1];
        if (string != NULL) {
          for (int i=0; i < lString; i++) newString[i]=string[i];
          newString[lString+1]='\0';
          delete [] string;
        }
        string=newString;
        maxString=newMaxString;
      }
      string[lString++]=(char)c;
      string[lString]='\0';
      if (c == '\n') {
        if (lString < 2 || string[lString-2] != '\\') break;
      };        
    };
    if (lString==0 && c < 0) return c;
    return lString;
}

int main() {

   fprintf(stderr,"Simple Interactive Calculator 2-Mar-98\n");
   fprintf(stderr,"\n");
   fprintf(stderr,"expr     : number (+|-|/|*) number\n");
   fprintf(stderr,"expr     : (expr)\n");
   fprintf(stderr,"statement: expr (\";\" expr)* {\";\"}\n");
   fprintf(stderr,"\n");
   fprintf(stderr,"Use backslash to continue expression onto next line\n");
   fprintf(stderr,"\n");

   int      quit=0;

   while (!quit) {

       if (getInputText(stdin) < 0) break;

       DLGStringInput	stringInput(string);
       DLGLexer	    	lexer(&stringInput,2000);
       MyTokenBuffer	pipe(&lexer,1);
       ANTLRToken		aToken( (ANTLRTokenType)0 );
       P			    parser(&pipe);
       ASTBase *		root(0);
       AST *		    rootCopy;
   
       lexer.setToken(&aToken);
       AST::clearList();		// remove statically constructed ASTs
       AST::clearCounter();
       parser.init();
       parser.statement(&root);
       rootCopy=(AST *) root;

       if (rootCopy != 0) {
             rootCopy->destroy();
       };

       AST::destroyList();
   }

   fclose(stdin);
   fclose(stdout);
   return 0;
}
>>

#token  Eof ("end-of-input") "@"

#token  InvalidNumber

#token  SemiColon ("\";\"")  ";"

#token  LP  ("\"(\"")   "\("
#token  RP  ("\")\"")   "\)"
#token  ADD ("\"+\"")   "\+"
#token  SUB ("\"-\"")   "\-"
#token  MUL ("\"*\"")   "\*"
#token  DIV ("\"+\"")   "\/"

#errclass   "operator"  { ADD SUB MUL DIV }

#token	Number	    "[.0-9]+{[dDfFlLeE]{\+\-}[0-9]+}"
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
#token	NL	            "\n"	<<newline();set_endcol(0);skip();>>
#token  Continuation    "\\\n"	<<newline();set_endcol(0);skip();>>

class P {

<<
public:
      virtual void syn(
	      ANTLRAbstractToken *	tok,
	      ANTLRChar *		    egroup,
	      SetWordType *		    eset,
	      ANTLRTokenType		etok,
	      int			        k);
>>


statement!
         : "@"
         |  expr0
                ( SemiColon expr0 )* {SemiColon} "@"
;

expr0!   : e:expr1
			<<
                if (#e != NULL &&
                      syntaxErrCount == 0 &&
                        ( (DLGLexer *) getLexer() )->lexErrCount == 0) {
        			printf("Result=%f\n",#e->eval());
/////////  			#e->dumpTree();
                };
			>>
	      
;
expr1 !	  :  a:expr2	<<#0=#a;>>
		( ADD  b:expr2	<<#0=#(new Add,#0,#b);>>
		| SUB  c:expr2	<<#0=#(new Sub,#0,#c);>>
		) *
;
expr2 !	  :  a:expr3	<<#0=#a;>>
		( MUL  b:expr3	<<#0=#(new Mul,#0,#b);>>
		| DIV  c:expr3	<<#0=#(new Div,#0,#c);>>
		) *
;
expr3 	   : n:Number
	   | LP ! expr1 RP !
;

}
