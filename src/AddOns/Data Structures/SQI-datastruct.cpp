/*
 * Squirrel project
 *
 * file ?	SQI-datastruct.cpp
 * what	?	DataS builtin predicates
 * who	?	jlv
 * when	?	10/28/99
 * last	?	03/21/01
 *
 *
 * (c) Kirilla 1999-2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "Dico.h"
#include "Vector.h"

// now some infos on the package

char *DataS_Pkg_name = "Data Structures";
char *DataS_Pkg_purpose = "Dictionary and Vector objects";
char *DataS_Pkg_author = "Kirilla";
char *DataS_Pkg_version = "0.23";
char *DataS_Pkg_date = "03/21/01";

/*
 * function    : SQI_DataS_Vector
 * purpose     : Builtin predicate "Vector"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_DataS_Vector(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	long capacity=0;
	SQI_Object *init = NULL;
	
	if(args->Length())
	{
		SQI_Number *num = IsInteger((*args)[0]);
		if(num)
		{
			capacity = Number2Long(num);
			if(args->Length()==2)
				init = (*args)[1];
		}	
		else
		{
			SQI_List *lst = IsList((*args)[0]);
			if(lst)
				return NewVectorObject(squirrel,lst);
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Vector","a number or a list is required"));
		}
	}
	
  	return NewVectorObject(squirrel,capacity,init);
}

/*
 * function    : SQI_DataS_Dico
 * purpose     : Builtin predicate "Dictionnary"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_DataS_Dico(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return NewDicoObject(squirrel);
  //return new SQI_ThePointer(squirrel,SQI_DICO);
}

/*
 * function    : SQI_DataS_IsVector
 * purpose     : Builtin predicate "is.vector"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataS_IsVector(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.vector","need 1 argument"));

  SQI_ThePointer *ptr = IsThePointer((*args)[0]);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_VECTOR)
    	return interp->True;
    else
    	return interp->False;
  } 
  else
    return interp->False;
}

/*
 * function    : SQI_DataS_IsDico
 * purpose     : Builtin predicate "is.dictionary"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataS_IsDico(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.dictionary","need 1 argument"));

  SQI_ThePointer *ptr = IsThePointer((*args)[0]);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_DICO)
    	return interp->True;
    else
    	return interp->False;
  } 
  else
    return interp->False;
}

/*
 * function    : SQI_DataS_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInDataS *DataS, DataS where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_DataS_Remove(SQI_BuiltInMap *Map)
{
	UninitDicoObject();	
	UninitVectorObject();	
	
  	Map->RemoveFunc("Dictionary");
  	Map->RemoveFunc("is.vector");
  	Map->RemoveFunc("is.dictionary");
  	Map->RemoveFunc("Vector");
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function DataS the function definied in this package
 * input       : 
 *
 * SQI_BuiltInDataS *DataS, DataS to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
  	//InitDicoObject();
  	//InitVectorObject();	
	
  	Map->AddAddOn(addon_id,DataS_Pkg_name,DataS_Pkg_purpose,DataS_Pkg_author,DataS_Pkg_version,DataS_Pkg_date,SQI_DataS_Remove);
      
  	Map->AddFunc("Dictionary",
	       DataS_Pkg_name,
	       "Output a new Dictionary object",
	       "Dictionary",
	       SQI_DataS_Dico
	       );
	       	       	       
  	Map->AddFunc("Vector",
	       DataS_Pkg_name,
	       "Output a new Vector object",
	       "Vector (size) (initial_value)",
	       SQI_DataS_Vector
	       );
	       
  	Map->AddFunc("is.dictionary",
	       DataS_Pkg_name,
	       "Output true if the input is a dictionary",
	       "is.dictionary thing",
	       SQI_DataS_IsDico
	       );
	       
	Map->AddFunc("is.vector",
	       DataS_Pkg_name,
	       "Output true if the input is a vector",
	       "is.vector thing",
	       SQI_DataS_IsVector
	       );	     	       
}
