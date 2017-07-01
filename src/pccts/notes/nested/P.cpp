/*
 * P: P a r s e r  S u p p o r t
 *
 * Generated from: nested.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-1999
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * with AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR20
 */

#define ANTLR_VERSION	13320
#include "pcctscfg.h"
#include "pccts_stdio.h"
#define ANTLR_SUPPORT_CODE
#include "tokens.h"
#include "P.h"

const ANTLRChar *P::tokenName(int tok)   { return _token_tbl[tok]; }

const ANTLRChar *P::_token_tbl[]={
	/* 00 */	"Invalid",
	/* 01 */	"Eof",
	/* 02 */	"WS",
	/* 03 */	"NL",
	/* 04 */	"Include",
	/* 05 */	"Any",
	/* 06 */	"Filename"
};

P::P(ANTLRTokenBuffer *input) : ANTLRParser(input,1,0,0,4)
{
	token_tbl = _token_tbl;
	traceOptionValueDefault=0;		// MR10 turn trace OFF
}

SetWordType P::setwd1[7] = {0x0,0x3,0x0,0x0,0x0,0x2,0x0};
