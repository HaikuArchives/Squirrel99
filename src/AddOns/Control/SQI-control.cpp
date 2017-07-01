/*
 * Squirrel project
 *
 * file ?	SQI-workspace.cpp
 * what	?	variable management builtin predicates
 * who	?	jlv
 * when	?	07/31/99
 * last	?	12/13/00
 *
 *
 * (c) electron Technology 1999-2000
 */

// First the needed header

#include "SQI-AddOn.h"

#include <unistd.h>
#include <Message.h>
#include <Messenger.h>

// now some infos on the package

char *Control_Pkg_name = "Control";
char *Control_Pkg_purpose = "Various control builtin";
char *Control_Pkg_author = "Kirilla";
char *Control_Pkg_version = "0.12";
char *Control_Pkg_date = "12/13/00";

const uint32 MSG_INTERP_BYE = 'IBy';

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

SQI_Object *SQI_Control_Test(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_Control_Output(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>1)
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

SQI_Object *SQI_Control_Stop(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"stop","no argument allowed"));
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

SQI_Object *SQI_Control_Wait(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>1)
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

SQI_Object *SQI_Control_Throw(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"throw","one argument minimum"));
  else
    {
      SQI_Object *obj = (*args)[0];
      SQI_Object *value = NULL;

      if(args->Length()>=2)
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

SQI_Object *SQI_Control_Bye(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"bye","not argument allowed"));
  else
    {
    	if(interp->Director)
    	{	
    		BMessenger msgr(NULL,interp->Director);
    		BMessage *m = new BMessage(MSG_INTERP_BYE);
    		msgr.SendMessage(m);
    	}		
      	squirrel->flag = SQI_SQ_BYE;
      	return NULL;
    }
}

/*
 * function    : SQI_Control_Break
 * purpose     : Builtin predicate "break"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Break(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"break","no argument allowed"));
  else
    {
      squirrel->flag = SQI_SQ_BREAK;
      return NULL;
    }
}

/*
 * function    : SQI_Control_Continue
 * purpose     : Builtin predicate "continue"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Continue(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()>0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"continue","no argument allowed"));
  else
    {
      squirrel->flag = SQI_SQ_CONTINUE;
      return NULL;
    }
}

/*
 * function    : SQI_Control_Call
 * purpose     : Builtin predicate "call"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Control_Call(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"call","need at least one argument"));
  else
    {
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
      			ret = squirrel->HopOnLeave(interp,call);	
      		} catch (SQI_Exception *ex) {
      			call->SetArgs(NULL);	
      			call->RemRef();
      			//string *str = ex->Print();
				//interp->GlobalVar->Set(squirrel,"_error",interp->GlobalHeap->String(*str),NO_SYSTEM_HOOK);
				//delete str;
      			throw(ex);		
      		}	
      		
      		call->SetArgs(NULL);	
      		call->RemRef();
      		return ret;
      	}
      	else
      		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"call","first input must be a word"));
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
  Map->RemoveFunc("bye");
  Map->RemoveFunc("break");
  Map->RemoveFunc("continue");
  Map->RemoveFunc("call");
}

/*
 * function    : SQI_Control_Init
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

  Map->AddAddOn(addon_id,Control_Pkg_name,Control_Pkg_purpose,Control_Pkg_author,Control_Pkg_version,Control_Pkg_date,SQI_Control_Remove);

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
	       SQI_Control_Stop
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
		
  Map->AddFunc("bye",
	       Control_Pkg_name,
	       "Quit Squirrel",
	       "bye",
	       SQI_Control_Bye
	       );
	       
   Map->AddFunc("break",
	       Control_Pkg_name,
	       "cause an immediat exit from a loop",
	       "break",
	       SQI_Control_Break
	       );
	       
   Map->AddFunc("continue",
	       Control_Pkg_name,
	       "skip the end of the block and process to the next iteration in a loop",
	       "continue",
	       SQI_Control_Continue
	       );
	
   Map->AddFunc("call",
	       Control_Pkg_name,
	       "call a command",
	       "call word input1 input2 input3 ...",
	       SQI_Control_Call
	       );
	       
	       
}

