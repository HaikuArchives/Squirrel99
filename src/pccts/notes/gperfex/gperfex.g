//
//  gperfex.g
//
//  29-May-97
//  pccts 1.33MR5
//
//   demonstrate use of GNU gperf for keyword hashing
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#header <<

#include "charbuf.h"

>>


#token Open
#token Close
#token Left
#token Right

#token Door
#token Window

#lexaction <<

#include "keywords.h"

Keywords *	in_word_set (char * string,unsigned int len);
Keywords *	pKeyword;
>>

#token ID	"[a-z]+"
	<<pKeyword=in_word_set(zzlextext,zzendexpr-zzlextext+1);
	  if (pKeyword != 0) {
		printf ("Keyword  is: %s\n",pKeyword->name);
		printf ("zztokens is: %s\n\n",
			zztokens[pKeyword->tokenType]);
		NLA=pKeyword->tokenType;
	  };
	>>

#token NL	"\n"
#token WS	"\ "	<<zzskip();>>

<<
int main() {
	ANTLR (statement(),stdin);

	fclose(stdin);
        fclose(stdout);
	return 0;
}
>>

statement : ( verb { adjective } object NL ) * "@" ;
verb	  : Open | Close ;
adjective : Left | Right ;
object    : Door | Window ;
