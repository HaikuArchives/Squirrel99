/*
 * Squirrel project
 *
 * file ?	SQI-plot.cpp
 * what	?	Data ploting builtin predicates
 * who	?	jlv
 * when	?	11/03/99
 * last	?	11/03/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include "SQI-AddOn.h"
#include "2DPlot.h"

// now some infos on the package

char *Plot_Pkg_name = "Plot";
char *Plot_Pkg_purpose = "Data Ploting builtins";
char *Plot_Pkg_author = "[e-]";
char *Plot_Pkg_version = "0.1";
char *Plot_Pkg_date = "11/03/99";


/*
 * function    : SQI_Plot_Plot2D
 * purpose     : Builtin predicate "2dplot"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Plot_Plot2D(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  int width = 640;
  int height = 480;		
	
  if(args->Length())
  {
  	if(args->Length()==2)
  	{
		SQI_Number *w = IsInteger((*args)[0]);
		SQI_Number *h = IsInteger((*args)[1]);
		
		if(w && h)
		{
			width = Number2Int(w);
			height = Number2Int(h);				
		}
		else
  			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"2dplot","width and height must be integer number"));		  		
  	}
  	else
  		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"2dplot","when dimension specified, 2 numbers must be used as input"));
  }
  
  return NewPlot2D(squirrel,width,height);
}

/*
 * function    : SQI_Plot_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Plot_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("plot2d");
  InitPlot2D();
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
  Map->AddAddOn(addon_id,Plot_Pkg_name,Plot_Pkg_purpose,Plot_Pkg_author,Plot_Pkg_version,Plot_Pkg_date,SQI_Plot_Remove);
  
  Map->AddFunc("plot2d",
	       Plot_Pkg_name,
	       "Output a new 2D Ploting window",
	       "plot2d (width height)",
	       SQI_Plot_Plot2D
	       );
}
