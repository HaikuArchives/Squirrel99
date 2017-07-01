/*
 * Squirrel project
 *
 * file ?	SQI-extra.cpp
 * what	?	Extra predicates
 * who	?	jlv
 * when	?	02/24/01
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-utils.h"
#include "SQI-bwidget.h"
#include "SQI-bdigital.h"

// now some infos on the package

char *Extra_Pkg_name = "Widgets";
char *Extra_Pkg_purpose = "Various widgets";
char *Extra_Pkg_author = "Kirilla";
char *Extra_Pkg_version = "0.03";
char *Extra_Pkg_date = "03/23/01";

/*
 * function    : SQI_GUI_Digital
 * purpose     : Builtin predicate "Digital"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Extra_Digital(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_Number *size = IsInteger((*args)[0]);
		if(size)
		{
			uint32 flags = 0;	
			
			if(args->Length()>=2)
			{				
				for(int i = 1;i<args->Length();i++)
				{	
					SQI_Keyword *f = IsKeyword((*args)[i]);
					if(f)
					{
						string *s = f->Data();
						
						if(*s == "navigable")
							flags = flags | B_NAVIGABLE;
						else	
						if(*s == "navigable.jump")
							flags = flags | B_NAVIGABLE_JUMP;
						else
						if(*s == "pulsed")
							flags = flags | B_PULSE_NEEDED;
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Odometer","word is not a valide flag"));	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Odometer","flag must be word"));
				}
			}
			
			return new SQI_BDigital(squirrel,Number2Int(size),flags);	
		}
	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Odometer","need an input ar least"));		
}

/*
 * function    : SQI_GUI_IsDigital
 * purpose     : Builtin predicate "is.viewer"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Extra_IsDigital(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsDigital((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.odometer","need one input"));
}


/*
 * function    : SQI_Extra_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInExtra *Extra, Extra where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Extra_Remove(SQI_BuiltInMap *Map)
{	
	UnInitBDigital();

	Map->RemoveFunc("Odometer");
	Map->RemoveFunc("is.odometer");			
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Extra the function definied in this package
 * input       : 
 *
 * SQI_BuiltInExtra *Extra, Extra to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
	printf("hum\n");
	
  	Map->AddAddOn(addon_id,Extra_Pkg_name,Extra_Pkg_purpose,Extra_Pkg_author,Extra_Pkg_version,Extra_Pkg_date,SQI_Extra_Remove);
 
	Map->AddFunc("Odometer",
	       Extra_Pkg_name,
	       "Create a new odometer widget",
	       "Odometer digits",
	       SQI_Extra_Digital
	       );
	       	          
	Map->AddFunc("is.odometer",
	       Extra_Pkg_name,
	       "Output true if the input is an odometer widget",
	       "is.odometer thing",
	       SQI_Extra_IsDigital
	       );	              	       
}
