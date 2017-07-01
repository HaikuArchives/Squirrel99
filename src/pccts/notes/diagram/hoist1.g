//
//  ul.g
//  12-Apr-95
//  experiment with semantic predicates and hoisting
//

<<
#include "DLGLexer.h"
#include "myToken.h"

int main() {

   DLGFileInput		in(stdin);
   DLGLexer		lexer(&in,2000);
   ANTLRTokenBuffer	pipe(&lexer,3);
   ANTLRToken		aToken;
   P			parser(&pipe);

   lexer.setToken(&aToken);
   parser.init();
   parser.statement();

   return 0;
}

int     isU (char *s) {return (*s >= 'A') && (*s <= 'Z') ;};
int     isL (char *s) {return (*s >= 'a') && (*s <= 'z') ;};

>>

#token  Eof    "@"

#token  ID     "[a-z A-Z]"
#token  Number "[0-9]"

#token  WS    "\ "	<<skip();>>
#token  NL    "\n"

class P {

parent  : statement
	| ID
	;
statement
        : upper NL Eof
	| number NL Eof

;upper	: <<isU(LT(1)->getText())>>? ID  <<printf("upper\n");>>
;number : Number  <<printf("number\n");>>
;
}
