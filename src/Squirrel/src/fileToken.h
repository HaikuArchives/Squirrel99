#ifndef FileToken_H
#define FileToken_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "noleakTok.h"
#include "AToken.h"

class DLGLexer;

struct FileToken : NoLeakToken {

       /* constructor */		FileToken();
       /* destructor */ virtual	~FileToken();
       /* constructor */		FileToken(ANTLRTokenType t);
       /* constructor */		FileToken(ANTLRTokenType t,
    					                 ANTLRChar *	text,
					                     int		line);
       /* copy constructor */	FileToken(const FileToken &);

       FileToken &			    operator = (const FileToken &);
       virtual void	          	dumpNode(const char * s=0);
       virtual ANTLRChar * 		getText() const {return pText;};
       virtual ANTLRAbstractToken *	makeToken(
                        						ANTLRTokenType	ANTLRTokenType,
                    					        ANTLRChar	*text,
                    					        int		line);
       virtual void 			setText(const ANTLRChar *s);

       int	        			col;		// column
       ANTLRChar *		    	pText;
       DLGLexer *               dlgLexer;
private:
       void				        init();
};

#endif

