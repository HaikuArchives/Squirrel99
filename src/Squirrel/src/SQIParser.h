/*
 * SQIParser: P a r s e r  H e a d e r 
 *
 * Generated from: SQI-grammar.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-1999
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * with AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR20
 */

#ifndef SQIParser_h
#define SQIParser_h

#ifndef ANTLR_VERSION
#define ANTLR_VERSION 13320
#endif

#include "AParser.h"


#ifndef sqiheader
#define sqiheader
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-ufunc.h"
#include "SQI-interp.h"
#include "SQI-squirrel.h"
#include "SQI-builtin.h"
#include "SQI-clean.h"
#include <stack>
extern stack <char *> TheFilenames;
#endif
class SQIParser : public ANTLRParser {
public:
	static  const ANTLRChar *tokenName(int tk);
protected:
	static  const ANTLRChar *_token_tbl[];
private:

public:
virtual void syn(_ANTLRTokenPtr tok, ANTLRChar *egroup, SetWordType *eset,ANTLRTokenType etok, int k)   
{
	int line;				
	line = LT(1)->getLine();
	
			    syntaxErrCount++;                                   /* MR11 */
	fprintf(stderr, "%s [line %d] syntax error at \"%s\"",TheFilenames.top(),line, LT(1)->getText());
	if ( !etok && !eset ) {fprintf(stderr, "\n"); return;}
	if ( k==1 ) fprintf(stderr, " missing");
	else
	{
		fprintf(stderr, "; \"%s\" not", LT(1)->getText());
		if ( set_deg(eset)>1 ) fprintf(stderr, " in");
	}
	if ( set_deg(eset)>0 ) 
	edecode(eset);
	else fprintf(stderr, " %s", token_tbl[etok]);
	if ( strlen(egroup) > 0 ) 
	fprintf(stderr, " in %s", egroup);
	fprintf(stderr, "\n"); 	
};
protected:
	static SetWordType COMMENT_set[16];
	static SetWordType RC_set[16];
	static SetWordType setwd1[109];
	static SetWordType ID_set[16];
	static SetWordType setwd2[109];
	static SetWordType setwd3[109];
	static SetWordType setwd4[109];
	static SetWordType RRC_set[16];
	static SetWordType setwd5[109];
	static SetWordType EXP_OP_set[16];
	static SetWordType setwd6[109];
	static SetWordType ADD_OP_set[16];
	static SetWordType MULT_OP_set[16];
	static SetWordType MOD_OP_set[16];
	static SetWordType setwd7[109];
	static SetWordType REL_OP_set[16];
	static SetWordType BOOL_OP_set[16];
	static SetWordType WORD_set[16];
	static SetWordType setwd8[109];
	static SetWordType setwd9[109];
	static SetWordType setwd10[109];
private:
	void zzdflthandlers( int _signal, int *_retsignal );

public:
	SQIParser(ANTLRTokenBuffer *input);
	 SQI_nBlock *  interp(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  instruction(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  predicate(int *_retsignal, SQI_Interp *Interp );
	void funcdef(int *_retsignal, SQI_Interp *Interp );
	 list<SQI_Object *> *  lparam(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  eparam(int *_retsignal, SQI_Interp *Interp );
	 SQI_nBlock *  funcbody(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  bodyinst(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  call(int *_retsignal, SQI_Interp *Interp );
	 list<SQI_Object *> *  arguments(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  argument(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  basic(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  counter(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  expression(int *_retsignal, SQI_Interp *Interp );
	 SQI_List *  alist(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  elist(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  block(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  simple(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  boolnegexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  expexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  signexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  multexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  modexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  addexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  relexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  mexpr(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  variable(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  member(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  number(int *_retsignal, SQI_Interp *Interp );
	 SQI_String *  quotedword(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  keyword(int *_retsignal, SQI_Interp *Interp );
	 SQI_Object *  iblock(int *_retsignal, SQI_Interp *Interp );
	 SQI_nControl *  control(int *_retsignal, SQI_Interp *Interp );
};

#endif /* SQIParser_h */
