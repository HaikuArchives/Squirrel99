//
//  simpleToken.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "simpleToken.h"

SimpleToken::SimpleToken(ANTLRTokenType t) {
  serial=++counter;
  setType(t);
  _line = 0;
}

SimpleToken::SimpleToken() {
  serial=++counter;
  setType((ANTLRTokenType)0);
  _line = 0;
}

int SimpleToken::counter=0;

