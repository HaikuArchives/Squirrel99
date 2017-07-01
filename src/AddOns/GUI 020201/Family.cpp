/*
 * Squirrel project
 *
 * file ?	Family.cpp
 * what	?   Store all the font family & style
 * who	?	jlv
 * when	?	11/21/99
 * last	?	11/21/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "Family.h"

typedef map<string,SQI_List *,less<string> > FFamilies;

FFamilies Families; // global map of font families

/*
 * function    : InitFontFamilies
 * purpose     : Get all the installed font
 * input       : none
 * output      : none
 * side effect : none
 */
void InitFontFamilies(SQI_Squirrel *squirrel)
{
	int32 numStyles;
	int32 num_families = count_font_families();
	font_family family;
	font_style style;
	uint32 flags;
				
	for(int32 i=0;i<num_families;i++)
	{		
		if(get_font_family(i,&family,&flags)==B_OK)
		{
			numStyles = count_font_styles(family);
			SQI_List *lst = squirrel->LocalHeap->List();
			lst->AddRef();					
							
			for(int32 j=0;j<numStyles;j++)
			{
				if(get_font_style(family,j,&style,&flags)==B_OK)
					lst->Add2End(squirrel->LocalHeap->String(style));	
			}
		
			Families[string(family)] = lst; 	
		}	
	}	
}

/*
 * function    : GetFontFamilies
 * purpose     : Get all the installed font in a list
 * input       : 
 *
 * SQI_List *list, the list to add the string in
 *
 * output      : none
 * side effect : none
 */
void GetFontFamilies(SQI_List *list)
{
	FFamilies::const_iterator i;
	
	for(i=Families.begin();i!=Families.end();i++)
		list->Add2End(list->heap->String(i->first));			
}

/*
 * function    : GetFontStyles
 * purpose     : Fill a list of all the style installed for a family
 * input       : 
 *
 * string *name, family name
 *
 * output      : none
 * side effect : none
 */
SQI_List *GetFontStyles(string *name) 
{
	SQI_List *lst = NULL;			

	lst = Families[*name];

	if(!lst)
		Families.erase(*name);
		
	return lst;
}

/*
 * function    : ExistFont
 * purpose     : Return true if the font exist
 * input       : 
 *
 * string *name, family name
 *
 * output      : bool
 * side effect : none
 */
bool ExistFont(string *name)
{
	SQI_List *lst = NULL;			

	lst = Families[*name];

	if(!lst)
	{
		Families.erase(*name);
		return false;
	}	
	else
		return true;
}

/*
 * function    : ExistStyle
 * purpose     : Return true if the style exist for a font
 * input       : 
 *
 * string *family, family name
 * string *style, style 
 *
 * output      : bool
 * side effect : none
 */
bool ExistStyle(font_family family,string *style)
{
	SQI_List *lst = NULL;			

	lst = Families[string(family)];		

	
	if(!lst)
	{
		Families.erase(string(family));
		return false;
	}	
	else
	{
		SQI_String *s;
		list<SQI_Object *>::const_iterator *i = lst->Iterator();
		list<SQI_Object *>::const_iterator *e = lst->End();
		
		for(i;*i!=*e;(*i)++)
		{
			s = IsString(*(*i));
			if(*(s->Data()) == *style)
				break; 			
		}
		
		if(*i!=*e)
		{
			delete i;
			delete e;
			return true;	
		}		
		else
		{
			delete i;
			delete e;
			return false;				
		}		
	}	
}
