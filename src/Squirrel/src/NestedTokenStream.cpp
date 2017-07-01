//
//  NestedTokenStream.cpp
//  

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "NestedTokenStream.h"
#include "DLexerBase.h"
#include "DLGLexer.h"

_ANTLRTokenPtr NestedTokenStream::getToken() {

    _ANTLRTokenPtr  result;

//
//  if this is the last token stream in the stack
//    then don't close it at EOF - leave it active
//
    while (1) {
//
//  is it possible that someone might want to skip EOF ?
//
      do {
        skipFlag=0;
        result=stack[depth-1]->getToken();
      } while (skipFlag);

      if (result->getType() == eofToken) {
        if (depth > 1) {
////      delete stack[depth-1];
          stack[depth-1]=0;
          depth--;
          stack[depth-1]->setParser(currentParser);
          if (track_columns) stack[depth-1]->trackColumns();
          continue;
        };
      };
      break;
    };
    return result;
}

ANTLRParser *  NestedTokenStream::setParser(ANTLRParser *newParser) {

    ANTLRParser     *old;

    old=currentParser;
    currentParser=newParser;
    stack[depth-1]->setParser(currentParser);
    return old;
}

ANTLRParser *  NestedTokenStream::getParser() {
    return currentParser;
}

void NestedTokenStream::newTokenStream(DLGLexer *ats) {
    
    DLGLexer            **newStack;
    int                 newDepth;

    if (depth >= maxDepth) {
      newDepth=(maxDepth+5)*2;
      newStack=new DLGLexer *[newDepth];
      if (newStack == 0) {
        panic("NestedTokenStream can't allocated DLGLexer stack");
      };
      if (stack != 0) {
        for (int i=0; i < depth; i++) { newStack[i]=stack[i]; }; 
        delete [] stack;
      };
      stack=newStack;      
      maxDepth=newDepth;
    };
    stack[depth]=ats;
    depth++;
    if (track_columns) ats->trackColumns();
}

void NestedTokenStream::init() {
    currentParser=0;
    stack=0;
    depth=0;
    maxDepth=0;
    track_columns=0;
    skipFlag=0;
    eofToken=(ANTLRTokenType)1;     /* default value of Eof */
}

NestedTokenStream::NestedTokenStream() {
  init();
}

NestedTokenStream::NestedTokenStream(DLGLexer *ats) {
  init();
  newTokenStream(ats);
}

NestedTokenStream::~NestedTokenStream() {
    for (int i=depth-1; i >= 0; i--) {
////  delete stack[i];
    }
    delete [] stack;
}

void NestedTokenStream::setEofToken(ANTLRTokenType t) {
    eofToken=t;
}

void NestedTokenStream::setToken(ANTLRAbstractToken *t) {
    token_to_fill=t;
}  

void NestedTokenStream::panic(const char *msg) {
	fprintf(stderr, "NestedTokenStream panic: %s\n", msg);
	exit(PCCTS_EXIT_FAILURE);
}

void NestedTokenStream::trackColumns() {
    track_columns=1;
    stack[depth-1]->trackColumns();
}

int NestedTokenStream::begcol() {
    return stack[depth-1]->begcol();
}

void NestedTokenStream::skip() {
    skipFlag=1;
};
