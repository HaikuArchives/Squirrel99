/*
 * Squirrel project
 *
 * file ?	SQI-time.cpp
 * what	?	time & timing builtin predicates
 * who	?	jlv
 * when	?	08/10/99
 * last	?	03/28/00
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include "SQI-AddOn.h"
#include <be/kernel/OS.h>

// now some infos on the package

char *Time_Pkg_name = "Time";
char *Time_Pkg_purpose = "Time & Timing builtins";
char *Time_Pkg_author = "Kirilla";
char *Time_Pkg_version = "0.3";
char *Time_Pkg_date = "09/28/00";


/*
 * function    : SQI_Time_Clock
 * purpose     : Builtin predicate "clock"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Time_Clock(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return new SQI_Number(squirrel->LocalHeap,clock());
}

/*
 * function    : SQI_Time_time
 * purpose     : Builtin predicate "time"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Time_Time(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return new SQI_Number(squirrel->LocalHeap,time(NULL));
}

/*
 * function    : SQI_Time_ctime
 * purpose     : Builtin predicate "ctime"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Time_Ctime(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  string str;

  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ctime","need 1 argument"));
  
  SQI_Number *t = IsInteger((*args)[0]);

  if(t)
    {
		long val;
		if(t->IsA()==SQI_LONG)
			val = t->lData();
      	else
			val = static_cast<long>(t->iData());

		str = ctime(&val);
		str.erase(str.length()-1);

		return new SQI_String(squirrel->LocalHeap,str); 
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ctime","need 1 number (long)"));
}

/*
 * function    : SQI_Time_C2sec
 * purpose     : Builtin predicate "c2sec"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Time_C2Sec(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  string *str;

  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"c2sec","need 1 argument"));
  
  SQI_Number *t = IsNumber((*args)[0]);

  if(t)
    {
      if(t->IsA()==SQI_LONG)
		return new SQI_Number(squirrel->LocalHeap,static_cast<double>(t->lData())/CLOCKS_PER_SEC);
      else
		return new SQI_Number(squirrel->LocalHeap,static_cast<double>(t->iData())/CLOCKS_PER_SEC);
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"c2sec","need 1 number (long)"));
}

/*
 * function    : SQI_Time_Timing
 * purpose     : Builtin predicate "timing"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Time_Timing(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"timing","need at least one argument"));
  else
    {
    	bigtime_t t0;	
    	
      	SQI_Keyword *name = IsKeyword((*args)[0]);
      	if(name)
      	{
      		SQI_Object *obj;
      		SQI_Object *ret = NULL;
      		SQI_Node *call = NULL;
      		list<SQI_Object *> *inputs = new list<SQI_Object *>;
      		
      		// we put all the other inputs in the inputs list now
      		
      		for(int i=1;i<args->Length();i++)
      		{
      			obj = (*args)[i];
      			obj->AddRef();
      			inputs->push_back(obj);
      		}
      		
      		call = dynamic_cast<SQI_Node *>(interp->FindCall(name->Data()->c_str()));	
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(inputs);
      		
      		try
      		{
      			t0 =  system_time();
      			ret = squirrel->HopOnLeave(interp,call);
      			t0 = system_time() - t0;	
      		} catch (SQI_Exception *ex) {
      			call->SetArgs(NULL);	
      			call->RemRef();
      			throw(ex);		
      		}	
      		
      		call->SetArgs(NULL);	
      		call->RemRef();
      		return new SQI_Number(squirrel->LocalHeap,(long)t0);
      	}
      	else
      	{
      		SQI_nBlock *block = IsBlock((*args)[0]);
      		if(block)
      		{
	      		try
	      		{
	      			t0 =  system_time();
	      			squirrel->HopOnABlock(interp,block);
	      			t0 = system_time() - t0;	
	      		} catch (SQI_Exception *ex) {
	      			throw(ex);		
	      		}     
	      		
	      	 	return new SQI_Number(squirrel->LocalHeap,(long)t0);			
      		}
      		else
      			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"timing","first input must be a word or a block"));
      	} 
    }	
}

/*
 * function    : SQI_Time_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Time_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("clock");
  Map->RemoveFunc("time");
  Map->RemoveFunc("ctime");
  Map->RemoveFunc("c2sec");
  Map->RemoveFunc("timing");
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
  Map->AddAddOn(addon_id,Time_Pkg_name,Time_Pkg_purpose,Time_Pkg_author,Time_Pkg_version,Time_Pkg_date,SQI_Time_Remove);
  
  Map->AddFunc("clock",
	       Time_Pkg_name,
	       "Output an approximation of processor time used by the program so far",
	       "clock",
	       SQI_Time_Clock
	       );

  Map->AddFunc("time",
	       Time_Pkg_name,
	       "Output the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds",
	       "time",
	       SQI_Time_Time
	       );

  Map->AddFunc("ctime",
	       Time_Pkg_name,
	       "Output the time in a string format from a time value in second (use time)",
	       "ctime atime",
	       SQI_Time_Ctime
	       );

  Map->AddFunc("c2sec",
	       Time_Pkg_name,
	       "Output the time in second from a clock value",
	       "c2sec",
	       SQI_Time_C2Sec
	       );
	       
  Map->AddFunc("timing",
	       Time_Pkg_name,
	       "Output the elapsed time in microseconds of execution of a block or a function",
	       "timing block | (word things ...)",
	       SQI_Time_Timing
	       );	       
}
