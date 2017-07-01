#ifndef NUMTOKEN_H
#define NUMTOKEN_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "AToken.h"
#include "simpleToken.h"

struct NumToken : SimpleToken {

       /* constructor */		NumToken();
       /* destructor */ virtual	~NumToken();
       /* constructor */		NumToken(ANTLRTokenType t);
       /* constructor */		NumToken(ANTLRTokenType   t,
    					                 ANTLRChar *	  text,
					                     int		  line);
       /* copy constructor */	NumToken(const NumToken &);

       NumToken &			    operator = (const NumToken &);
       virtual void	          	dumpNode(const char * s=0);
       virtual ANTLRChar * 		getText() const {return pText;};
       virtual ANTLRAbstractToken * 	makeToken(ANTLRTokenType  t,
					                              ANTLRChar	  *text,
					                              int		  line);
       virtual void 			setText(const ANTLRChar *s);

       int      				col;
       ANTLRChar *	    		pText;
       double			    	number;
private:
       void     				init();
};

#endif

