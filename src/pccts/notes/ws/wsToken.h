#ifndef WSTOKEN_H
#define WSTOKEN_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "simpleToken.h"

struct ANTLRToken : public SimpleToken {

       /* constructor */		ANTLRToken();
       /* destructor */ virtual	~ANTLRToken();
       /* constructor */		ANTLRToken(ANTLRTokenType t);
       /* constructor */		ANTLRToken(ANTLRTokenType    t,
    					                   const ANTLRChar * text,
					                       int		         line);
       /* copy constructor */	ANTLRToken(const ANTLRToken &);

       ANTLRToken &			    operator = (const ANTLRToken &);

       virtual void			    dumpNode(const char * s=0);
       virtual ANTLRChar *      getText() const {return pText;};
       virtual ANTLRAbstractToken *	makeToken(ANTLRTokenType  tokenType,
                					          ANTLRChar       *text,
                					          int		      line);
       virtual void     		setText(const ANTLRChar *s);

       int				        col;		// column
       ANTLRChar *			    pText;
       void				        setws(ANTLRChar *);
       ANTLRChar *			    ws;		// whitespace string
private:
       void				        init();
};

#endif
