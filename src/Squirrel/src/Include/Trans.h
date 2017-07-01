/*
 * Squirrel project
 *
 * file ?	Trans.h
 * what	?   Translators structure
 * who	?	jlv
 * when	?	02/25/00
 * last	?	12/04/00
 *
 *
 * (c) electron Technology 2000
 */
 
#ifndef TRANS_H
#define TRANS_H  

#include <TranslationKit.h> 
#include <TranslatorRoster.h>
 
typedef struct TTranslator {
	int32 id;
	int32 type;
	char *mime;	
} TTranslator;

typedef map<string, TTranslator, less<string> > TMTranslators; 

#endif
