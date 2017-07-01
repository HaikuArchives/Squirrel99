/*
 * Squirrel project
 *
 * file ?	SQI-threading.cpp
 * what	?	Multi-Threading builtin predicates
 * who	?	jlv
 * when	?	02/09/00
 * last	?	02/14/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
 // First the needed header

#include "SQI-AddOn.h"
#include <Application.h>

#include <image.h>
#include <OS.h>
#include <Locker.h>
#include "sqi-locker.h"
#include "Threading.h"

// now some infos on the package

char *Threading_Pkg_name = "Threading";
char *Threading_Pkg_purpose = "Multi-Threading primitives";
char *Threading_Pkg_author = "Kirilla";
char *Threading_Pkg_version = "0.5";
char *Threading_Pkg_date = "02/14/01";

BLocker MapLock;	  // locker for the map

/*
 * function    : SQI_Threading_Spawn
 * purpose     : Builtin predicate "Thread"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Spawn(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread","need at least two inputs"));
  else
    {
    	SQI_Keyword *priority = IsKeyword((*args)[0]);
    	
    	if(priority)
    	{
	      	SQI_Keyword *name = IsKeyword((*args)[1]);
	      	string *p = priority->Data();	
	      	int32 prio;	
	      		
	      	if(*p == "low")
	      		prio = B_LOW_PRIORITY;
	      	else
	      	if(*p == "normal")
	      		prio = B_NORMAL_PRIORITY;
	      	else
	      	if(*p == "display")
	      		prio = B_DISPLAY_PRIORITY;
	      	else	      			
	      	if(*p == "urgent_display")
	      		prio = B_URGENT_DISPLAY_PRIORITY;
	      	else	      		
	      	if(*p == "realtime_display")
	      		prio = B_REAL_TIME_DISPLAY_PRIORITY;
	      	else
	      	if(*p == "urgent")
	      		prio = B_URGENT_PRIORITY;
	      	else
	      	if(*p == "realtime")
	      		prio = B_REAL_TIME_PRIORITY;
	      	else
	      		throw(new SQI_Exception(SQI_EX_INVALIDE,"Thread","thread priority must be a valid word"));
	      		      	
	      	if(name)
	      	{
	      		SQI_Object *obj;
	      		SQI_Object *ret = NULL;
	      		SQI_Node *call = NULL;
	      		list<SQI_Object *> *inputs = new list<SQI_Object *>;
	      		
	      		// we put all the other inputs in the inputs list now
	      		
	      		for(int i=2;i<args->Length();i++)
	      		{
	      			obj = (*args)[i];
	      			obj->AddRef();
	      			obj->Export(squirrel->interpreter->GlobalHeap);
	      			inputs->push_back(obj);
	      		}
	      		 		
	      		call = dynamic_cast<SQI_Node *>(interp->FindCall(name->Data()->c_str()));	
	      		
	      		call->Export(squirrel->interpreter->GlobalHeap);
	      		
	      		call->AddRef();
	      		
	      		//call->SetArgs(inputs);
	      		
	      		return new SQI_Number(squirrel->LocalHeap,SpawnThread(name->Data()->c_str(),call,squirrel,prio,inputs));
	      	}
	      	else
	      	{
	      		SQI_nBlock *block = IsBlock((*args)[1]);	
	      		if(block)
	      		{
	      			block->Export(squirrel->interpreter->GlobalHeap);
	      		
	      			block->AddRef();
	      			      		
	      			return new SQI_Number(squirrel->LocalHeap,SpawnThread("block",block,squirrel,prio));	
	      		}
	      		else
	      			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Thread","second input must be a word or a block"));
	      	}	
    	}
    	else
    		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Thread","first input must be a word"));	
    }	
}

/*
 * function    : SQI_Threading_Resume
 * purpose     : Builtin predicate "Thread.hop"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Resume(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.hop","need at least one input"));
  
  	SQI_Number *id;
    
  	for(int i=0;i<args->Length();i++)
  	{
  		id = IsInteger((*args)[i]);
  		if(id)
  			resume_thread(Number2Long(id));
  	}  
  	
  	return NULL;
}

/*
 * function    : SQI_Threading_Snooze
 * purpose     : Builtin predicate "snooze"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Snooze(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"snooze","need at least one input"));
      
  	SQI_Number *time = IsInteger((*args)[0]);
 	if(time)
 		snooze(Number2Long(time));
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"snooze","input must be an integer"));		 
 
  	return NULL;
}

/*
 * function    : SQI_Threading_Suspend
 * purpose     : Builtin predicate "Thread.suspend"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Suspend(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.suspend","need at least one input"));
  
  	SQI_Number *id;
    
  	for(int i=0;i<args->Length();i++)
  	{
  		id = IsInteger((*args)[i]);
  		if(id)
  			suspend_thread(Number2Long(id));
  	}  
  	
  	return NULL;
}

/*
 * function    : SQI_Threading_Kill
 * purpose     : Builtin predicate "Thread.kill"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Kill(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.kill","need at least one input"));
  
  	SQI_Number *id;
    
  	for(int i=0;i<args->Length();i++)
  	{
  		id = IsInteger((*args)[i]);
  		if(id)
  		{
  			SQI_Thread_Data *thread = NULL;
  			long n = Number2Long(id);
  				
  			if(n > B_ERROR)
  			{	
	  			thread = TheThreads[n];
	  					
	  			if(thread)
	  			{		
	  				kill_thread(n);
	  				if(!thread->waited)
	  				{
	  					delete thread->squirrel;
	  					delete thread;
	  					TheThreads.erase(n);
	  				}
	  				else
	  					thread->terminated = true;
	  			}
  			}	
  		}	
  	}  
  	
  	return NULL;
}

/*
 * function    : SQI_Threading_WaitFor
 * purpose     : Builtin predicate "Thread.waitfor"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_WaitFor(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.waitfor","need one input at least"));
  
  	if(args->Length()==1)
  	{
	  	status_t vexit;
	  	SQI_Object *output = NULL;
	  	SQI_Number *id = IsInteger((*args)[0]);
	  	if(id)
	  	{
	  		long n = Number2Long(id);
	  		SQI_Thread_Data *thread = NULL;
	  		
	  		thread = TheThreads[n];
	  		if(thread)
	  		{
	  			thread->waited = true;
	  			wait_for_thread(n,&vexit);
	  			if(thread->output)
	  			{
	  				output = thread->output;
	  				output->Export(squirrel->LocalHeap);
	  				delete thread->squirrel;
	  				delete thread;
	  				TheThreads.erase(n);	
	  			}
	  		}
	  		else
	  			TheThreads.erase(n);	
	  	}
	   	
	  	return output;
  	}
  	else
  	{
  		SQI_Keyword *mode = IsKeyword((*args)[0]);
  		if(mode)
  		{
  			string *str = mode->Data();
  			
  			if(*str == "first")
  			{	
		  		SQI_Object *output;
		  		thread_id *ids,over_id;
		  		SQI_Number *id;	
		  		int32 priority;	
		  		thread_info info;
		  		bool over = false;
		  		int i,number;	
		  		
		  		// first we put all the thread id to wait in an array
		  		// and we inform the thread that they are waited
		  		
		  		number = args->Length() - 1;	
		  		
		  		ids = new thread_id[number];
		  		
		  		for(i=0;i<number;i++)
		  		{
		  			id = IsInteger((*args)[i+1]);
		  			if(id)
		  			{
		  				ids[i] = Number2Long(id);
		  				TheThreads[ids[i]]->waited = true;	
		  			}	
		  		}	
		  		  		
		  		// now we save the priority of the thread and we switch it to low	
		  		get_thread_info(squirrel->thread,&info);
		  		priority = info.priority;	
		  		set_thread_priority(squirrel->thread,B_LOW_PRIORITY);
		  		
		  		while(!over)
		  		{
		  			for(i=0;i<number;i++)
		  				if(TheThreads[ids[i]]->terminated)
		  					{
		  						over_id = ids[i];
		  						over = true;
		  						break;	
		  					}
		  			if(!over)
		  				snooze(700);
		  		}	
		  		  		
		  		// the wait is over now
		  		// we put back the priority
		  		set_thread_priority(squirrel->thread,priority);
		  		// we set back the flag of all the thread we waited 
		  		for(i=0;i<number;i++)
		  			TheThreads[ids[i]]->waited = false;
		  						
		  		// and we get the output if any
		  		SQI_Thread_Data *thread = NULL;
			  	thread = TheThreads[over_id];
			  	if(thread)
			  		if(thread->output)
			  		{
		  				output = thread->output;
		  				output->Export(squirrel->LocalHeap);
		  				delete thread->squirrel;
		  				delete thread;
		  				TheThreads.erase(over_id);	
			  		}	
			  	delete [] ids;
			  	return output;
  			}
  			else
  			if(*str == "all")
  			{
		  		SQI_Object *output;
		  		SQI_List *lst;
		  		thread_id *ids,over_id;
		  		SQI_Number *id;	
		  		int32 priority;	
		  		thread_info info;
		  		int over = 0;
		  		int i,number;	
		  		
		  		// first we put all the thread id to wait in an array
		  		// and we inform the thread that they are waited
		  		
		  		lst = new SQI_List(squirrel->LocalHeap);	
		  		
		  		number = args->Length() - 1;	
		  		
		  		ids = new thread_id[number];
		  		
		  		for(i=0;i<number;i++)
		  		{
		  			id = IsInteger((*args)[i+1]);
		  			if(id)
		  			{
		  				ids[i] = Number2Long(id);
		  				TheThreads[ids[i]]->waited = true;	
		  			}	
		  		}
		  				  		  		
		  		// now we save the priority of the thread and we switch it to low	
		  		get_thread_info(squirrel->thread,&info);
		  		priority = info.priority;	
		  		set_thread_priority(squirrel->thread,B_LOW_PRIORITY);
		  		
		  		while(over<number)
		  		{
		  			for(i=0;i<number;i++)
		  				if(TheThreads[ids[i]]->terminated)
		  					{
		  						TheThreads[ids[i]]->terminated = false;
		  						over++;
		  						break;	
		  					}
		  			if(over<number)
		  				snooze(700);
		  		}	
		  		  		
		  		// the wait is over now
		  		// we put back the priority
		  		set_thread_priority(squirrel->thread,priority);
		  		// we set back the flag of all the thread we waited 
		  		for(i=0;i<number;i++)
		  			TheThreads[ids[i]]->waited = false;
		  							  						
		  		// and we get the output if any
		  		SQI_Thread_Data *thread = NULL;
		  		for(i=0;i<number;i++)
		  		{
			  		thread = TheThreads[ids[i]];
			  		if(thread)
			  			if(thread->output)
			  			{
		  					output = thread->output;
		  					output->Export(squirrel->LocalHeap);
		  					lst->Add2End(output);
		  					delete thread->squirrel;
		  					delete thread;
		  					TheThreads.erase(ids[i]);	
			  			}
		  		}	
			  	delete [] ids;
			  	return lst;  				
  			}
  			else
  				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Thread.waitfor","first input must be a valid word"));	
  		}
  		else
  			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Thread.waitfor","first input must be a word"));
  	}
}

/*
 * function    : SQI_Threading_Hoping
 * purpose     : Builtin predicate "Thread.hoping"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Hoping(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.hoping","need no input"));
  
 	ThreadMap::const_iterator i;
 	SQI_List *lst = new SQI_List(squirrel->LocalHeap);	
 	
 	
 	MapLock.Lock();
 	for(i=TheThreads.begin();i!=TheThreads.end();i++)
 		lst->Add2End(new SQI_Number(squirrel->LocalHeap,(*i).first));
 	MapLock.Unlock();
   	
   	return lst;
}

/*
 * function    : SQI_Threading_Priority
 * purpose     : Builtin predicate "Thread.Priority"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Priority(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(!args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.suspend","need at least one input"));
    
  	SQI_Number *id = IsInteger((*args)[0]);
	if(id)
	{
		if(args->Length()==2)
		{
			SQI_Keyword *priority = IsKeyword((*args)[1]);	
			if(priority)
			{
				string *p = priority->Data();	
		      	int32 prio;	
		      		
		      	if(*p == "low")
		      		prio = B_LOW_PRIORITY;
		      	else
		      	if(*p == "normal")
		      		prio = B_NORMAL_PRIORITY;
		      	else
		      	if(*p == "display")
		      		prio = B_DISPLAY_PRIORITY;
		      	else	      			
		      	if(*p == "urgent_display")
		      		prio = B_URGENT_DISPLAY_PRIORITY;
		      	else	      		
		      	if(*p == "realtime_display")
		      		prio = B_REAL_TIME_DISPLAY_PRIORITY;
		      	else
		      	if(*p == "urgent")
		      		prio = B_URGENT_PRIORITY;
		      	else
		      	if(*p == "realtime")
		      		prio = B_REAL_TIME_PRIORITY;
		      	else
		      		throw(new SQI_Exception(SQI_EX_INVALIDE,"Thread","thread priority must be a valid word"));
		      		
		      	set_thread_priority(Number2Long(id),prio);
		      	return NULL;
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.priority","second input must be a word"));	
		}
		else
		{
			thread_info info;
			if(get_thread_info(Number2Long(id),&info) == B_NO_ERROR)
			{
				switch(info.priority)
				{
					case B_LOW_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"low");
					case B_NORMAL_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"normal");
					case B_DISPLAY_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"display");
					case B_URGENT_DISPLAY_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"urgent_display");
					case B_REAL_TIME_DISPLAY_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"realtime_display");
					case B_URGENT_PRIORITY:
						return new SQI_String(squirrel->LocalHeap,"urgent");
					case B_REAL_TIME_PRIORITY:	
						return new SQI_String(squirrel->LocalHeap,"realtime");
				}	
			}
			else
				return interp->False;		
		}	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.priority","first input must be an integer"));	
}

/*
 * function    : SQI_Threading_TPriority
 * purpose     : Builtin predicate "Priority"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_TPriority(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{
		SQI_Keyword *priority = IsKeyword((*args)[0]);	
		if(priority)
		{
			string *p = priority->Data();	
	      	int32 prio;	
	      		
	      	if(*p == "low")
	      		prio = B_LOW_PRIORITY;
	      	else
	      	if(*p == "normal")
	      		prio = B_NORMAL_PRIORITY;
	      	else
	      	if(*p == "display")
	      		prio = B_DISPLAY_PRIORITY;
	      	else	      			
	      	if(*p == "urgent_display")
	      		prio = B_URGENT_DISPLAY_PRIORITY;
	      	else	      		
	      	if(*p == "realtime_display")
	      		prio = B_REAL_TIME_DISPLAY_PRIORITY;
	      	else
	      	if(*p == "urgent")
	      		prio = B_URGENT_PRIORITY;
	      	else
	      	if(*p == "realtime")
	      		prio = B_REAL_TIME_PRIORITY;
	      	else
	      		throw(new SQI_Exception(SQI_EX_INVALIDE,"Thread","thread priority must be a valid word"));
	      		
	      	set_thread_priority(squirrel->thread,prio);
	      	return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Thread.priority","second input must be a word"));	
	}
	else
	{
		thread_info info;
		if(get_thread_info(squirrel->thread,&info) == B_NO_ERROR)
		{
			switch(info.priority)
			{
				case B_LOW_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"low");
				case B_NORMAL_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"normal");
				case B_DISPLAY_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"display");
				case B_URGENT_DISPLAY_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"urgent_display");
				case B_REAL_TIME_DISPLAY_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"realtime_display");
				case B_URGENT_PRIORITY:
					return new SQI_String(squirrel->LocalHeap,"urgent");
				case B_REAL_TIME_PRIORITY:	
					return new SQI_String(squirrel->LocalHeap,"realtime");
			}	
		}
		else
			return interp->False;		
	}		
}

/*
 * function    : SQI_Threading_ID
 * purpose     : Builtin predicate "ThreadID"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_ID(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(args->Length())
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ThreadID","need no input"));
  
 	return new SQI_Number(squirrel->LocalHeap,squirrel->thread);
}

/*
 * function    : SQI_Threading_Locker
 * purpose     : Builtin predicate "Locker"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Locker(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	return new SQI_Locker(squirrel);
}

/*
 * function    : SQI_Threading_Wait
 * purpose     : Builtin predicate "Wait"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Threading_Wait(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{
		SQI_Keyword *var = IsKeyword((*args)[0]);	
		if(var)
		{
			if(squirrel->WaitingVar(interp,var->Data(),squirrel->thread))
			{
				suspend_thread(squirrel->thread);
				return interp->True;
			}
			else
				return interp->False;	
		}	
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Wait","need a word as input"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Wait","need one input"));	
}

/*
 * function    : SQI_Threading_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Threading_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("Thread");
  Map->RemoveFunc("Thread.hop");
  Map->RemoveFunc("Thread.suspend");
  Map->RemoveFunc("Thread.kill");
  Map->RemoveFunc("Thread.waitfor");
  Map->RemoveFunc("Thread.priority");
  Map->RemoveFunc("snooze");
  Map->RemoveFunc("Priority");
  Map->RemoveFunc("ThreadID");
  Map->RemoveFunc("Locker");
  Map->RemoveFunc("Wait");
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
	InitLockerMember();	
	
  	Map->AddAddOn(addon_id,Threading_Pkg_name,Threading_Pkg_purpose,Threading_Pkg_author,Threading_Pkg_version,Threading_Pkg_date,SQI_Threading_Remove);
  
  	Map->AddFunc("Thread",
	       Threading_Pkg_name,
	       "Create a new thread and output it thread id",
	       "Thread priority block | (word things ...)",
	       SQI_Threading_Spawn
	       ); 
	       	       
	Map->AddFunc("Thread.hop",
	       Threading_Pkg_name,
	       "Run (resume) one or severals created thread",
	       "Thread.hop (number)+",
	       SQI_Threading_Resume
	       ); 
	       	       	       
	Map->AddFunc("Thread.suspend",
	       Threading_Pkg_name,
	       "Suspend one or severals hoping thread",
	       "Thread.suspend (number)+",
	       SQI_Threading_Suspend
	       ); 
	       
	Map->AddFunc("Thread.kill",
	       Threading_Pkg_name,
	       "Kill one or severals hoping thread",
	       "Thread.kill (number)+",
	       SQI_Threading_Kill
	       );
	       
	Map->AddFunc("Thread.waitfor",
	       Threading_Pkg_name,
	       "Wait for the end of a thread",
	       "Thread.waitfor number",
	       SQI_Threading_WaitFor
	       );
	       	       
	Map->AddFunc("Thread.hoping",
	       Threading_Pkg_name,
	       "Output a list of id for all the running thread",
	       "Thread.hoping",
	       SQI_Threading_Hoping
	       );
	      
	 Map->AddFunc("Thread.priority",
	       Threading_Pkg_name,
	       "Output or change the priority of a thread",
	       "Thread.priority (word)",
	       SQI_Threading_Priority
	       );	       

	Map->AddFunc("snooze",
	       Threading_Pkg_name,
	       "Pause the calling thread for a given number of microseconds",
	       "snooze number",
	       SQI_Threading_Snooze
	       );
	       
	Map->AddFunc("Priority",
	       Threading_Pkg_name,
	       "Output or change the priority of the calling thread",
	       "Priority (word)",
	       SQI_Threading_TPriority
	       );  

	Map->AddFunc("ThreadID",
	       Threading_Pkg_name,
	       "Output the ID of the calling thread",
	       "ThreadID",
	       SQI_Threading_ID
	       );
	       
	Map->AddFunc("Locker",
	       Threading_Pkg_name,
	       "Create a new locker",
	       "Locker",
	       SQI_Threading_Locker
	       );
	       
	Map->AddFunc("Wait",
	       Threading_Pkg_name,
	       "The calling thread wait for a variable to be updated",
	       "Waitword",
	       SQI_Threading_Wait
	       ); 
}

