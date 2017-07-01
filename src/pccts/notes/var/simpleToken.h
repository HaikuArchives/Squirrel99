#ifndef SIMPLETOKEN_H
#define SIMPLETOKEN_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "AToken.h"

class SimpleToken : public ANTLRRefCountToken {

protected:
	ANTLRTokenType 		_type;
public:

	int 			_line;
	int			    serial;

static  int			counter;

	SimpleToken(ANTLRTokenType t);
	SimpleToken();
	ANTLRTokenType getType() const	{ return _type; }
	void setType(ANTLRTokenType t)	{ _type = t; }
	virtual int getLine() const		{ return _line; }
	void setLine(int line)      	{ _line = line; }
};

#endif
