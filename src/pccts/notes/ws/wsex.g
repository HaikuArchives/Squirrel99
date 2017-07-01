//
//  wsex.g
//
//  28-May-97
//  pccts 1.33MR5
//
//  print out column information for tokens
//  demonstrate use of derived DLG classes by passing white space
//   following tokens as an item in the ANTLRToken
//
//  28-May-97  Use reference counted tokens
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#lexmember <<
  protected:
    int			isWS;
    void		tabAdjust();
    int			begline;
>>

<<
#include "wsDLG.h"
#include "wsToken.h"
#include "myPanic.h"
#include "ATokenBuffer.h"
#include "wsAST.h"

int main() {

  ASTBase *		root(0);
  AST *			rootCopy;

  {
    DLGFileInput	in(stdin);
    MyDLG		lexer(&in,2000);
    ANTLRTokenBuffer	pipe(&lexer,1);
    ANTLRToken		aToken( (ANTLRTokenType)0 );
    P			parser(&pipe);

    lexer.setToken(&aToken);

#if DEBUG_LEXER
    lexer.debugLexer(1);
#endif

    AST::clearList();		// remove statically constructed ASTs
    AST::clearCounter();

    parser.init();
    parser.statement(&root);
  };

  rootCopy=(AST *) root;

  if (rootCopy != 0) {
////    rootCopy->preorder();
////    rootCopy->dumpTree();
    rootCopy->destroy();
  };

  printf("AST elements on NoLeak List\n");
  AST::dumpList();
  AST::destroyList();

  fclose(stdin);
  fclose(stdout);
  return 0;
}

>>

#token	Eof	"@"
#token	ID	"[a-z A-Z 0-9]+"
#token	WS	"[\ \t]+"	<<isWS=1;tabAdjust();>>
#token	NL	"\n"		<<isWS=1;tabAdjust();>>
#token  	"/\*"		<<isWS=1;tabAdjust();more();mode(LC_Com);>>

#lexclass LC_Com
#token  Com	"\*\/"		<<mode(START);tabAdjust();>>
#token  	"\n"		<<more();tabAdjust();>>
#token          "~[ ]"		<<more();tabAdjust();>>

#tokclass  WSthings {WS NL Com}

#lexclass START
class P {

<<
public:
      virtual void syn(
	      ANTLRAbstractToken *	tok,
	      ANTLRChar *		egroup,
	      SetWordType *		eset,
	      ANTLRTokenType		etok,
	      int			k);
>>

// leading whitespace special case

statement : ( w:WSthings  <<mytoken($w)->dumpNode();>> )*
	    ( id:ID	  <<mytoken($id)->dumpNode();>> )*
	    "@"
;
}
