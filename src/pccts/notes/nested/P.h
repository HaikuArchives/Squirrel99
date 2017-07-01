/*
 * P: P a r s e r  H e a d e r 
 *
 * Generated from: nested.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-1999
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * with AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR20
 */

#ifndef P_h
#define P_h

#ifndef ANTLR_VERSION
#define ANTLR_VERSION 13320
#endif

#include "AParser.h"


class P : public ANTLRParser {
public:
	static  const ANTLRChar *tokenName(int tk);
protected:
	static  const ANTLRChar *_token_tbl[];
private:
protected:
	static SetWordType setwd1[7];
private:
	void zzdflthandlers( int _signal, int *_retsignal );

public:
	P(ANTLRTokenBuffer *input);
	void statement(void);
	void word(void);
};

#endif /* P_h */
