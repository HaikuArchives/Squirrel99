//
// mySyn.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "tokens.h"
#include "myToken.h"
#include "P.h"

void P::syn(ANTLRTokenBase *	tok,
	    ANTLRChar *		egroup,
	    SetWordType *	eset,
	    TokenType		etok,
	    int			k)
{
	int	line;
	int	col;

	line = ((ANTLRTokenBase *)LT(1))->getLine();
	col  = ((ANTLRToken *)LT(1))->col;

	fprintf(stderr, "line %d col %d: syntax error at \"%s\"",
			line,
			col,
			((ANTLRTokenBase *)LT(1))->getText());

	if ( !etok && !eset ) {fprintf(stderr, "\n"); return;};
	if ( k==1 ) {
	  fprintf(stderr, " missing");
	} else {
	  fprintf(stderr, "; \"%s\" not",
		        ((ANTLRTokenBase *)LT(1))->getText());
	  if ( set_deg(eset)>1 ) fprintf(stderr, " in");
	};
	if ( set_deg(eset)>0 ) {
	  edecode(eset);
	} else {
          fprintf(stderr, " %s", token_tbl[etok]);
	};
        if ( strlen(egroup) > 0 ) fprintf(stderr, " in %s", egroup);
	fprintf(stderr, "\n");
}
