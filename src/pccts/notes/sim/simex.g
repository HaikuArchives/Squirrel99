//
//  sim.g
//  1-Jul-97
//  pccts 1.33MR7
//
//  demonstrates conversion of semantic predicate validation error into
//    parser exception using features which may not work in the next
//    version of pccts
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

<<

#define PREDFAIL(string) {      \
	_signal=NoSemViableAlt; \
	 PREDFAILmessage=string;   \
	 goto _handler;         \
	}
#include "ATokenBuffer.h"
#include "DLGLexer.h"
#include "myToken.h"
#include <ctype.h>

int isU(char * s) {return isupper(s[0]);};
int isL(char * s) {return islower(s[0]);};

int main() {
   DLGFileInput		in(stdin);
   DLGLexer     	lexer(&in,2000);
   ANTLRTokenBuffer	pipe(&lexer,2);
   ANTLRToken		aToken;
   P			    parser(&pipe);
   int			    parserSignal;
   int			    finalSignal;

   lexer.setToken(&aToken);
   parser.init();
   parser.PREDFAILmessage=0;
   parser.unwinding=0;
   parser.statement(&parserSignal);
   if (parser.unwinding != 0) {
      printf("Signal unwound %d levels\n",parser.unwinding);
      if (parser.PREDFAILmessage != 0) {
     	printf("Message \"%s\"\n",parser.PREDFAILmessage);
      };
   };
   fclose(stdin);
   fclose(stdout);
   return 0;
}
>>

//
// signals are #define in pccts/h/AParser.h
//

exception
   catch NoViableAlt:
	 <<if (!unwinding) {
	     unwinding++;
 	     printf("default handler: NoViableAlt\n");
	   };
	  >>
   catch NoSemViableAlt:
	 <<if (!unwinding) {
	     unwinding++;
	     printf("default handler: NoSemViableAlt\n");
	   };
	  >>
   catch MismatchedToken:
	 <<if (!unwinding) {
	     unwinding++;
	     printf("default handler: MismatchedToken\n");
	   };
	  >>
   default:
	 <<printf("default handler: Non-standard signal\n");>>

#token	ID	"[a-z A-Z]+"
#token  WS	"(\ )+"		<<skip();>>
#token	NL	"\n"

#token  Eof	"@"

class P {

<<public:
     void defaultHandlers(int signal,int *retSignal) {
        zzdflthandlers(signal,retSignal);
     };
     int 		unwinding;
     char *		PREDFAILmessage;
private:
>>

statement : "#token"  upper
	  | lower     ":"
	  ;

upper	:  <<isU(LT(1)->getText())>>?
		 [PREDFAIL("Expected ID starting with upper case char");]
	    ID;

lower   :  <<isL(LT(1)->getText())>>?	ID;


}
