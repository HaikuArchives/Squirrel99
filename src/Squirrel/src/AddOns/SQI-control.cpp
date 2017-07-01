/*
 * Squirrel project
 *
 * file ?	SQI-workspace.cpp
 * what	?	variable management builtin predicates
 * who	?	jlv
 * when	?	07/31/99
 * last	?	08/09/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include "SQI-AddOn.h"

#include <unistd.h>

// now some infos on the package

char *Control_Pkg_name = "Control";
char *Control_Pkg_purpose = "Various control builtin";
char *Control_Pkg_author = "[e-]";
char *Control_Pkg_version = "0.4";
char *Control_Pkg_date = "08/09/99";

/*
 * function    : SQI_Control_Test
 * purpose     : Builtin predicate "test"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Test(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"test"));
  else
    {
      // when the function is called , we have a number as only argument

      SQI_Object *obj = (*args)[0];

      if(obj->Is()==SQI_NUMBER)
	  squirrel->test = (dynamic_cast<SQI_Number *>(obj))->IsTrue();
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"test","must be a bool expression"));


      return NULL;
    }
}

/*
 * function    : SQI_Control_Output
 * purpose     : Builtin predicate "output"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Output(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()>1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ouput","one argument maximum"));
  else
    {
      // when the function is called , we have a number as only argument

      SQI_Object *obj = (*args)[0];

      squirrel->flag = SQI_SQ_OUTPUT;
      squirrel->context = obj;

      return NULL;
    }
}

/*
 * function    : SQI_Control_Stop
 * purpose     : Builtin predicate "stop"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Stop(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"stop","not argument allowed"));
  else
    {
      squirrel->flag = SQI_SQ_STOP;
      return NULL;
    }
}

/*
 * function    : SQI_Control_Wait
 * purpose     : Builtin predicate "wait"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Wait(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()>1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ouput","one argument maximum"));
  else
    {
      // when the function is called , we have a number as only argument

      SQI_Object *obj = (*args)[0];

      if(obj->Is()==SQI_NUMBER)
	{
	  SQI_Number *num = dynamic_cast<SQI_Number *>(obj);

	  switch(num->IsA())
	    {
	    case SQI_INT:
	      {
		int count = num->iData();
		sleep(count); 
		break;
	      }
	    case SQI_LONG:
	      {
		long count = num->lData();
		sleep(count);
		break;
	      }
	    case SQI_FLOAT:
	      {
		float fcount = num->fData();
		sleep(static_cast<int>(fcount));
		break;
	      }
	    case SQI_DOUBLE:
	      {
		double dcount = num->dData();
		sleep(static_cast<int>(dcount));
		break;
	      }
	    }

	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"wait","argument must be a number"));

      return NULL;
    }
}

/*
 * function    : SQI_Control_Throw
 * purpose     : Builtin predicate "throw"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Throw(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"throw","one argument minimum"));
  else
    {
      SQI_Object *obj = (*args)[0];
      SQI_Object *value = NULL;

      if(args->size()>=2)
	value = (*args)[1];

      if(obj->Is()==SQI_KEYWORD)
	{
	  SQI_Keyword *tag = dynamic_cast<SQI_Keyword *>(obj);

	  throw(new SQI_Exception(tag->Data()->data(),value));
	  
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"throw","the tag must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_Control_True
 * purpose     : Builtin predicate "true"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_True(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return interp->True;
}

/*
 * function    : SQI_Control_False
 * purpose     : Builtin predicate "false"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_False(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return interp->False;
}


/*
 * function    : SQI_Control_Bye
 * purpose     : Builtin predicate "bye"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Bye(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"bye","not argument allowed"));
  else
    {
      squirrel->flag = SQI_SQ_BYE;
      return NULL;
    }
}

/*
 * function    : SQI_Control_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Control_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("test");
  Map->RemoveFunc("output");
  Map->RemoveFunc("stop");
  Map->RemoveFunc("wait");
  Map->RemoveFunc("throw");
}

/*
 * function    : SQI_Control_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 *
 * output      : none
 * side effect : none
 */
void SQI_Control_Init(SQI_BuiltInMap *Map)
{

  Map->AddAddOn(Control_Pkg_name,Control_Pkg_purpose,Control_Pkg_author,Control_Pkg_version,Control_Pkg_date,SQI_Control_Remove);

  Map->AddFunc("test",
	       Control_Pkg_name,
	       "remember the boolean value of the input",
	       "test boolexpr",
	       SQI_Control_Test
	       );

  Map->AddFunc("output",
	       Control_Pkg_name,
	       "return from a procedure with a value",
	       "output value",
	       SQI_Control_Output
	       );

   Map->AddFunc("stop",
	       Control_Pkg_name,
	       "stop the execution of a procedure",
	       "stop",
	       SQI_Control_Output
	       );

   Map->AddFunc("wait",
		Control_Pkg_name,
		"suspend the execution for time 60th second",
		"wait second",
		SQI_Control_Wait
		);

   Map->AddFunc("throw",
		Control_Pkg_name,
		"throw a user error",
		"throw \"tag value",
		SQI_Control_Throw
		);

   Map->AddFunc("true",
		Control_Pkg_name,
		"Output the value TRUE",
		"true",
		SQI_Control_True
		);

   Map->AddFunc("false",
		Control_Pkg_name,
		"Output the value FALSE",
		"false",
		SQI_Control_False
		);
}

