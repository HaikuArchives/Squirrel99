#header <<>>
<<
#include "DLGLexer.h"

int main() {

   DLGFileInput		in(stdin);
   DLGLexer		lexer(&in,2000);
   ANTLRTokenBuffer	pipe(&lexer,2);
   ANTLRCommonToken	aToken;
   P			parser(&pipe);

   lexer.setToken(&aToken);
   parser.init();
   parser.statement();

   return 0;
}

int     isa (char *s ) {
	int i=(*s=='a');
	return i;
};

int     isb (char *s) {
	int i=(*s=='b');
	return i;};

>>

#token  Eof "@"

#token  X	"x"	<<printf("token is x\n");>>
#token  Y	"y"	<<printf("token is y\n");>>
#token  A_or_B  "a | b"	<<printf("token is (a|b)\n");>>

#token  WS    "\ "	<<skip();>>
#token  NL    "\n"	<<skip();>>

class P {
statement
        : ( ax_or_by | bx )* "@" ;
ax_or_by
	: aName X	<<printf("aName x executed\n");>>
        | bName Y	<<printf("bName y executed\n");>>
	;
bx      : bName X	<<printf("bName x executed\n");>>
	;
aName   : <<isa(LT(1)->getText())>>? A_or_B ;
bName   : <<isb(LT(1)->getText()) >>? A_or_B ;
}
