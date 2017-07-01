/*
 * Squirrel project
 *
 * file ?	SQI-tracker.cpp
 * what	?	Tracker predicates
 * who	?	jlv
 * when	?	01/08/01
 * last	?	01/08/01
 *
 *
 * (c) Kirilla 2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-utils.h"
#include "SQI-bwidget.h"
#include "SQI-btview.h"

// now some infos on the package

char *Tracker_Pkg_name = "Tracker";
char *Tracker_Pkg_purpose = "Tracker widgets";
char *Tracker_Pkg_author = "[e-]";
char *Tracker_Pkg_version = "0.01";
char *Tracker_Pkg_date = "02/08/01";

/*
 * function    : SQI_GUI_TrackView
 * purpose     : Builtin predicate "TrackView"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Tracker_TrackView(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_List *size = IsList((*args)[0]);
		if(size)
		{
			int width = -1;
			int height = -1;
			uint32 mode = B_FOLLOW_NONE;
			uint32 flags = 0;
			
			SQI_String *dir = IsString((*args)[1]);
			
			if(dir)
			{	
				if(args->Length()>=3)
				{				
					for(int i = 2;i<args->Length();i++)
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
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Viewer","word is not a valide flag"));	
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Viewer","flag must be word"));
					}
				}
				
				// time to get the size from the list
				
				List2Size(size,&width,&height);	
				
				return new SQI_BTView(squirrel,width,height,dir->Data()->c_str(),mode,flags);
			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TrackView","second input must be a string"));		
		}
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"TrackView","need an input at least"));			
}

/*
 * function    : SQI_GUI_IsTrackView
 * purpose     : Builtin predicate "is.trackview"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Tracker_IsTrackView(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsTView((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.trackview","need one input"));
}


/*
 * function    : SQI_Tracker_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInTracker *Tracker, Tracker where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Tracker_Remove(SQI_BuiltInMap *Map)
{	
	UnInitBTView();

	Map->RemoveFunc("TrackView");
	Map->RemoveFunc("is.trackview");		
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Tracker the function definied in this package
 * input       : 
 *
 * SQI_BuiltInTracker *Tracker, Tracker to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
	
  	Map->AddAddOn(addon_id,Tracker_Pkg_name,Tracker_Pkg_purpose,Tracker_Pkg_author,Tracker_Pkg_version,Tracker_Pkg_date,SQI_Tracker_Remove);
 
	Map->AddFunc("TrackView",
	       Tracker_Pkg_name,
	       "Create a new Tracker widget",
	       "TrackView size dir",
	       SQI_Tracker_TrackView
	       );
	       	          
	Map->AddFunc("is.viewer",
	       Tracker_Pkg_name,
	       "Output true if the input is a Viewer widget",
	       "is.viewer thing",
	       SQI_Tracker_IsTrackView
	       );             	       
}
