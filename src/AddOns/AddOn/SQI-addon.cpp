/*
 * Squirrel project
 *
 * file ?	SQI-AddOns.cpp
 * what	?	AddOn management builtin predicate
 * who	?	jlv
 * when	?	08/02/99
 * last	?	12/06/99
 *
 *
 * (c) electron Technology 1999-2000
 */

// First the needed header

#include <iostream.h>
#include "SQI-AddOn.h"

// now some infos on the package

char *AddOn_Pkg_name = "AddOn";
char *AddOn_Pkg_purpose = "AddOns managment";
char *AddOn_Pkg_author = "[e-]";
char *AddOn_Pkg_version = "0.6";
char *AddOn_Pkg_date = "12/06/00";

/*
 * function    : SQI_AddOn_List
 * purpose     : Builtin predicate "addon.list"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_AddOn_List(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  SQI_List *addons = new SQI_List(squirrel->LocalHeap);
  
  interp->BuiltInMap->ListAddOn(addons);
 
  return addons;
}

/*
 * function    : SQI_AddOn_Info
 * purpose     : Builtin predicate "addon.info"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_AddOn_Info(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"addon.info","you must specify the name of the AddOn"));

  SQI_Object *addon = (*args)[0];

  if((addon->Is()==SQI_STRING)||(addon->Is()==SQI_KEYWORD))
    {
      SQI_String *name = dynamic_cast<SQI_String *>(addon);
      string *str;
      
      str = interp->BuiltInMap->GetAddOnInfo(name->Data());

 	  SQI_String *ret = new SQI_String(squirrel->LocalHeap,*str);
  	  
  	  delete str;
  	  
  	  return ret;
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"addon.info","argument must be a word/string"));
}

/*
 * function    : SQI_AddOn_Func
 * purpose     : Builtin predicate "addon.func"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_AddOn_Func(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"addon.func","you must specify the name of the AddOn"));

  SQI_Object *addon = (*args)[0];

  if(addon->Is()==SQI_KEYWORD || addon->Is()==SQI_STRING)
    {
      SQI_String *name = dynamic_cast<SQI_String *>(addon);
      SQI_List *lst = new SQI_List(squirrel->LocalHeap);

      interp->BuiltInMap->GetAddOnFunc(lst,name->Data());
       
  	  return lst;
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"addon.func","argument must be a word or a string"));
}

/*
 * function    : SQI_AddOn_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("addon.list");
  Map->RemoveFunc("addon.info");
  Map->RemoveFunc("addon.func");
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
  Map->AddAddOn(addon_id,AddOn_Pkg_name,AddOn_Pkg_purpose,AddOn_Pkg_author,AddOn_Pkg_version,AddOn_Pkg_date,SQI_AddOn_Remove);

  Map->AddFunc("addon.list",
	       AddOn_Pkg_name,
	       "Return a list of all the AddOn currently loaded",
	       "addon.list",
	       SQI_AddOn_List
	       );

  Map->AddFunc("addon.info",
	       AddOn_Pkg_name,
	       "Print a info on an AddOn",
	       "addon.info \"AddOn_name",
	       SQI_AddOn_Info
	       );

  Map->AddFunc("addon.func",
	       AddOn_Pkg_name,
	       "Print the name of all the function of AddOn",
	       "addon.func \"AddOn_name",
	       SQI_AddOn_Func
	       );
}

