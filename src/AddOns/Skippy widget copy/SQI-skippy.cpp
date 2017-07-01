/*
 * Squirrel project
 *
 * file ?	SQI-skippy.cpp
 * what	?	Skippy builtin predicates
 * who	?	jlv
 * when	?	02/18/00
 * last	?	01/24/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
 // First the needed header

#include "SQI-AddOn.h"
#include "SQI-font.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bsheet.h"
#include "SQI-brender.h"
#include "Skippy.h"

// now some infos on the package

char *Skippy_Pkg_name = "Skippy";
char *Skippy_Pkg_purpose = "Skippy widget";
char *Skippy_Pkg_author = "Kirilla";
char *Skippy_Pkg_version = "0.11";
char *Skippy_Pkg_date = "01/24/01";

/*
 * function    : SQI_Skippy_Skippy
 * purpose     : Builtin predicate "Sheet"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Skippy_Sheet(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Sheet","need one input"));
	else
	{
		SQI_List *size = IsList((*args)[0]);
		if(size)
		{
			int width = -1;
			int height = -1;
			List2Size(size,&width,&height);
			
			return new SQI_BSheet(squirrel,width,height,B_WILL_DRAW|B_PULSE_NEEDED|B_FRAME_EVENTS);	
		}
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"Sheet","input must be a list"));	
	}	
}
/*
 * function    : SQI_GUI_IsSheet
 * purpose     : Builtin predicate "is.sheet"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Skippy_IsSheet(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsSheet((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.sheet","need one input"));
}

/*
 * function    : SQI_Skippy_Skippy
 * purpose     : Builtin predicate "Skippy"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Skippy_Skippy(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Skippy","need one input"));
	else
	{
		SQI_BSheet *sheet = IsSheet((*args)[0]);
		if(sheet)		
			return new Skippy(squirrel,sheet);
		else
		{
			SQI_Image *img = IsImage((*args)[0]);
			if(img)
			{
				SQI_BRender *render = new SQI_BRender(img);
				return new Skippy(squirrel,render);	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Skippy","input must be a Sheet widget or an image"));	
		}	
	}	
}

/*
 * function    : SQI_Skippy_IsSkippy
 * purpose     : Builtin predicate "is.skippy"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Skippy_IsSkippy(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsSkippy((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.skippy","need one input"));
}

/*
 * function    : SQI_Skippy_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Skippy_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("Sheet");
  Map->RemoveFunc("is.sheet");
  Map->RemoveFunc("Skippy");
  Map->RemoveFunc("is.skippy");
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
	InitBSheet();
	InitSkippy();	
	
  	Map->AddAddOn(addon_id,Skippy_Pkg_name,Skippy_Pkg_purpose,Skippy_Pkg_author,Skippy_Pkg_version,Skippy_Pkg_date,SQI_Skippy_Remove);
  
  	Map->AddFunc("Sheet",
	       Skippy_Pkg_name,
	       "Create a new Sheet widget",
	       "Sheet size",
	       SQI_Skippy_Sheet
	       );
	       
	Map->AddFunc("is.sheet",
	       Skippy_Pkg_name,
	       "Output true if the input is a Sheet widget",
	       "is.sheet thing",
	       SQI_Skippy_IsSheet
	       ); 
	       
	       
	Map->AddFunc("Skippy",
	       Skippy_Pkg_name,
	       "Create a new Skippy widget",
	       "Sheet sheet",
	       SQI_Skippy_Skippy
	       );
	       
	Map->AddFunc("is.skippy",
	       Skippy_Pkg_name,
	       "Output true if the input is a Skippy object",
	       "is.skippy thing",
	       SQI_Skippy_IsSkippy
	       );
}

