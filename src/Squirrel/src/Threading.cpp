/*
 * Squirrel project
 *
 * file ?	Threading.cpp
 * what	?   functions for multi-threading
 *			moved from the Threading Add-On here
 * who	?	jlv
 * when	?	02/14/01
 * last	?	02/19/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "SQI-AddOn.h"
#include "Threading.h"
#include <Locker.h>

ThreadMap TheThreads; // map of the created thread

/*
 * function    : Squirrel_Thread
 * purpose     : Thread function of a squirrel thread
 * input       : 
 *
 * void *data, data given to the thread
 *
 * output      : int32, an exit status
 * side effect : none
 */
int32 Squirrel_Thread(void *data)
{
	SQI_Thread_Data *thread = static_cast<SQI_Thread_Data *>(data);	
	SQI_Object *ret;
	SQI_nUfunc *func;
	
	try
	{
		SQI_nBlock *block = IsBlock(thread->node);
		if(block) // block
			thread->squirrel->HopOnABlock(thread->squirrel->interpreter,block);
		else	// function or something else	
		{
			func = 	IsUFunc(thread->node);
			if(func)
			{
				if(thread->args)
					func->SetArgs(thread->args);
				ret = thread->squirrel->HopOnAUfunc(thread->squirrel->interpreter,func,true);
				if(thread->args)
				{
					if(func->GetArgs() == thread->args)
						func->DelArgs();
					else	
					{
						list<SQI_Object *>::const_iterator i;
						for(i=thread->args->begin();i!=thread->args->end();i++)
							(*i)->RemRef();	
					}	
				}	
						
				delete thread->args;		
			}
			else
				ret = thread->squirrel->HopOnLeave(thread->squirrel->interpreter,thread->node);			
		}	
	} catch (SQI_Exception *ex) {
		string *str = ex->Print();
		*(thread->squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
		thread->squirrel->interpreter->Stream->Flush();
		if(func && thread->args)
		{
			func->DelArgs();
			delete thread->args;
		}		
		delete str;
		delete ex;		
	}
		
	thread->node->RemRef();	
		
	if(!thread->waited)	
	{	
		thread->squirrel->interpreter->Pool->Put(thread->squirrel);
		delete thread;
	}
	else
	{
		thread->terminated = true;
		thread->output = ret;
	}
	
	return 0;
}

/*
 * function    : SpawnThread
 * purpose     : Create a thread with a block to execute
 * input       : 
 *
 * char *name, name of the thread
 * SQI_Node *node, node to execute
 * SQI_Squirrel *parent, parent squirrel
 * int32 priority, thread priority
 *
 * output      : thread_id, id of the thread
 * side effect : none
 */
thread_id SpawnThread(const char *name,SQI_Node *node,SQI_Squirrel *squirrel,int32 priority,list<SQI_Object *> *args = NULL)
{
	SQI_Thread_Data *thread = new SQI_Thread_Data;
	thread_id id;
		
	thread->args = args;	
	thread->output = NULL;
	thread->terminated = false;
	thread->waited = false;
	thread->node = node;
	thread->squirrel =  squirrel->interpreter->Pool->Get(squirrel->interpreter);	
	thread->squirrel->SameGCConfig(squirrel);
	thread->squirrel->SamePrecision(squirrel);
	    		
	id = spawn_thread(Squirrel_Thread,name,priority,thread);
	
	thread->squirrel->thread = id;	
		
	if(id > B_ERROR)
		TheThreads[id] = thread;
	else
	{
		thread->squirrel->interpreter->Pool->Put(thread->squirrel);
		delete thread;	
	}	
	
	return id;
} 
