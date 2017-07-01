//
//  colgcex.g
//
//  28-May-97
//  pccts 1.33MR5
//
//  print out column information for tokens
//  demonstrate use of garbage collection of tokens via reference counting
//
//  compare with output from col/colex.g to see effectiveness of TJP
//    reference count scheme for tokens compared with manual garbage 
//    collection

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

<<
#include "myToken.h"
#include "myAST.h"
#include "myPanic.h"
#include "myTokenBuffer.h"
#include "DLGLexer.h"

int main() {

  ASTBase *		root(0);
  AST *			rootCopy;

  {
    DLGFileInput	in(stdin);
    DLGLexer		lexer(&in,2000);
    MyTokenBuffer	pipe(&lexer,1);
    ANTLRToken		aToken( (ANTLRTokenType)0 );
    P			parser(&pipe);

    lexer.setToken(&aToken);

    AST::clearList();		// remove statically constructed ASTs
    AST::clearCounter();

    parser.init();
    parser.statement(&root);
    rootCopy=(AST *) root;
  };

  if (rootCopy != 0) {
    rootCopy->preorder();
    rootCopy->dumpTree();
    AST::dumpList();
    rootCopy->destroy();
  };

  
  printf("AST elements on NoLeak List\n");
  AST::dumpList();
  AST::destroyList();
  printf("Tokens remaining on NoLeak List after AST::destroyList\n");
  ANTLRToken::dumpList();
  ANTLRToken::destroyList();
  fclose(stdin);
  fclose(stdout);
  return 0;
}
>>

#token	Eof	"@"
#token	ID	"[a-z]+"
#token	WS	"[\ \t]"	<<skip();
				 if (*_lextext=='\t') {
				   _endcol=((_endcol-1) & ~7) + 8;
				   // DLG will add 1 more because it
				   //   assumes \t is a printing character
				   // See wstabadjust.cpp for ws strings
				 };
				>>
#token	NL	"\n"		<<newline();set_endcol(0);skip();>>

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

statement : ( expr0 )* "@"

;expr0	: expr1 {"\="^ expr0}
;expr1  : expr2 ("\+"^ expr2)*
;expr2  : expr3 ("\*"^ expr3 !)*  // note deliberate dropping of token
;expr3  : ID
;

}
