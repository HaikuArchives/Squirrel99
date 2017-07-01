#ifndef VARTOKEN_H
#define VARTOKEN_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "simpleToken.h"
#include "AToken.h"

struct VarToken : SimpleToken {

       /* constructor */		VarToken();
       /* destructor */ virtual	~VarToken();
       /* constructor */		VarToken(ANTLRTokenType t);
       /* constructor */		VarToken(ANTLRTokenType 	t,
    					                 ANTLRChar *	text,
					                     int		line);
       /* copy constructor */	VarToken(const VarToken &);

       VarToken &	    		operator = (const VarToken &);
       virtual void	          	dumpNode(const char * s=0);
       virtual ANTLRChar * 		getText() const {return pText;};
       virtual ANTLRAbstractToken *     makeToken(ANTLRTokenType ANTLRTokenType,
					                              ANTLRChar	 *text,
					                              int		 line);
       virtual void 			setText(const ANTLRChar *s);

       ANTLRChar *		    	pText;
private:
       void     				init();
};

#endif
