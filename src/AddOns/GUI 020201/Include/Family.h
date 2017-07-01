/*
 * Squirrel project
 *
 * file ?	Family.h
 * what	?   Store all the font family & style
 * who	?	jlv
 * when	?	11/21/99
 * last	?	11/22/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <map>
#include <Font.h>

void InitFontFamilies(SQI_Squirrel *squirrel); // Get all the installed font

void GetFontFamilies(SQI_List *list); // Return a list of all the family installed

SQI_List *GetFontStyles(string *name); // Return a list of all the style installed for a family

bool ExistFont(string *name); // Return true if the font exist

bool ExistStyle(font_family family,string *style); // return true if the style exist for the font
