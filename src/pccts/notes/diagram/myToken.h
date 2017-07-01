#ifndef MYTOKEN_H
#define MYTOKEN_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "simpleToken.h"
#include "AToken.h"

struct ANTLRToken : SimpleToken {

       /* constructor */		ANTLRToken();
       /* destructor */ virtual	~ANTLRToken();
       /* constructor */		ANTLRToken(ANTLRTokenType t);
       /* constructor */		ANTLRToken(ANTLRTokenType t,
    					                   ANTLRChar *	  text,
					                       int		  line);
       /* copy constructor */	ANTLRToken(const ANTLRToken &);

       ANTLRToken &			    operator = (const ANTLRToken &);
       virtual void	          	dumpNode(const char * s=0);
       virtual ANTLRChar * 		getText() const {return pText;};
       virtual ANTLRAbstractToken *     makeToken(ANTLRTokenType tokenType,
				                    	          ANTLRChar *    text,
					                              int		     line);
       virtual void 			setText(const ANTLRChar *s);

       int      				col;		// column
       ANTLRChar *		    	pText;
private:
       void				        init();
};

#endif
