//
//  nested.g
//  4-Mar-97
//  pccts 1.33MR12
//
//  Demonstrates how to handle statements like #include which
//    requires a lexer to open another DLGInputStream
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#header <<>>

#lexprefix <<

#include    "NestedTokenStream.h"

char *mystrdup(const char *);

>>

#lexmember <<
    NestedTokenStream           *nestedTokenStream;
    const char *                filename;
    virtual _ANTLRTokenPtr      getToken();
>>

<<
#include "DLGLexer.h"
#include "myToken.h"
#include "NestedTokenStream.h"
#include "ATokenBuffer.h"

int main() {

   DLGFileInput		    in(stdin);
   DLGLexer		        lexer(&in,2000);
   NestedTokenStream    nestedTokenStream(&lexer);
   ANTLRTokenBuffer	    pipe(&nestedTokenStream,1);
   ANTLRToken   		aToken;
   P			        parser(&pipe);

   lexer.filename="stdin";
   lexer.nestedTokenStream=&nestedTokenStream;
   lexer.setToken(&aToken);

   nestedTokenStream.trackColumns();

////   nestedTokenStream.setToken(&aToken);
////   nestedTokenStream.setEofToken(Eof);

   parser.init();
   parser.statement();

   fclose(stdin);
   fclose(stdout);
   return 0;
}

char *mystrdup(const char *s) {
  int   l=strlen(s);
  char  *t=new char[l+1];
  strcpy(t,s);
  return t;
}

_ANTLRTokenPtr DLGLexer::getToken() {
  FileToken         *result;
  result=(FileToken *)DLGLexerBase::getToken();
  result->col=DLGLexerBase::begcol();
  result->dlgLexer=this;
  return result;
}
>>

#token  Eof         "@"
#token  WS          "[\t \ ]+"      <<skip();>>
#token  NL          "\n"            <<skip();newline();set_endcol(0);>>

#token  Include     "#include"
    <<mode(LC_INCLUDE); 
            //
            // skip() depends on the way your parser works
            // does it want to see the Include token or not ?
            //
      skip();
    >>

#token  Any          "~[\ \t \n #]+"

#lexclass LC_INCLUDE

#token  WS          "[\t \ ]+"      <<skip();>>
#token  NL          "\n"            <<skip();newline();set_endcol(0);>>
#token  Filename    "( [A-Z a-z 0-9 \. _]+ ) | ( \" ~[\ \n \t \r \"]* \" )"
<<
    mode(START);
    char *p=_lextext;
    if (*p=='\"') p++;
    for (char *q=p; *q != '\0'; q++) if (*q=='\"') *q='\0';
    const char *    nestedFilename=mystrdup(p);
    FILE *          nestedFILE=fopen(nestedFilename,"r");
    if (nestedFILE == 0) {
      fprintf(stderr,"Can't open #include file \"%s\"\n",nestedFilename);
    } else {
      DLGFileInput  *dlgFileInput=new DLGFileInput(nestedFILE);
      DLGLexer      *dlgLexer=new DLGLexer(dlgFileInput,2000);
      dlgLexer->nestedTokenStream=nestedTokenStream;
      dlgLexer->setToken(token_to_fill);
      dlgLexer->filename=nestedFilename;
      nestedTokenStream->newTokenStream(dlgLexer);
    };
//
//  There is a coordination problem with NestedTokenStream.
//  When NestedTokenStream::getToken() calls the DLGLexer::getToken()
//    the lexer will not return until it has a token (possibly an EOF token).
//  If the action routine for Filename calls skip() then the returned token
//    will be the one following the Filename.  This is not good, since that
//    token would be passed to the parser before text from the included file.
//  In this case the token should be saved until the nested file has been
//    completely read.
//  If the user doesn't call skip then the token should *not* be held until
//    the nested file has been read, but there is no way for NestedTokenStream
//    the right thing to do unless the Filename action routine tells it.
//
//  This tells the NestedTokenStream to skip this token, rather than doing the
//    skip in the DLGLexer.  It solves the problem without requiring the
//    NestedTokenStream to stash the lookahead token until needed.
//
//  This is *only* needed following a call to NestedTokenStream::newTokenStream()
//    when you want to skip the Filename token.
//
    nestedTokenStream->skip();
>>

#lexclass START

class P {

statement   : (word)* "@" ;

word        : w:Any
                <<fprintf(stdout,"Text=%-16s line=%-3d  col=%-3d  file=%s\n",
                  mytoken($w)->getText(),
                  mytoken($w)->getLine(),
                  mytoken($w)->col,
                  mytoken($w)->dlgLexer->filename);
                >>                
;

}
