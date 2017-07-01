/*
 * Squirrel project
 *
 * file ?	SQI-varmap.cpp
 * what	?	VarMap class implementation
 * who	?	jlv
 * when	?	07/19/99
 * last	?	12/05/00
 *
 *
 * (c) electron Technology 1999-2000
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-ufunc.h"
#include "SQI-interp.h"
#include "SQI-squirrel.h"
#include <string.h>

void DeleteBinding(TVariable *var)
{
	list<SQI_Object *>::const_iterator j;
	
	if(var->get)
    {
    		if(var->get->function)
    		{
    			var->get->function->RemRef();
    			if(var->get->args)
    			{
    				for(j=var->get->args->begin();j!=var->get->args->end();j++)
    					(*j)->RemRef();
    				var->get->args->clear();
    				delete var->get->args;
    			}
    		}
    		else
    		if(var->get->block)
    			var->get->block->RemRef();
    			
    		delete var->get;	
    }
    	
    if(var->set)
    {
    		if(var->set->function)
    		{
    			var->set->function->RemRef();
    			if(var->set->args)
    			{
	    			for(j=var->set->args->begin();j!=var->set->args->end();j++)
	    				(*j)->RemRef();
	    			var->set->args->clear();
	    			delete var->set->args;
    			}
    		}
    		else
    		if(var->set->block)
    			var->set->block->RemRef();
    			
    		delete var->set;	
    }
    	
    if(var->erase)
    {
    		if(var->erase->function)
    		{
    			var->erase->function->RemRef();
    			if(var->erase->args)
    			{
    				for(j=var->erase->args->begin();j!=var->erase->args->end();j++)
    					(*j)->RemRef();
    				var->erase->args->clear();
    				delete var->erase->args;
    			}	
    		}
    		else
    		if(var->erase->block)
    			var->erase->block->RemRef();
    			
    		delete var->erase;	
    }
    	
	delete var;	
}

/*
 * function    : SQI_VarMap
 * purpose     : Create a new VarMap
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_VarMap::SQI_VarMap()
{
	map = hash_create(HASHCOUNT_T_MAX, NULL, NULL); 
}

/*
 * function    : ~SQI_VarMap
 * purpose     : destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_VarMap::~SQI_VarMap()
{
	hscan_t hs;
    hnode_t *hn;
    TVariable *var;

	// in case we still have some object linked to the variable, we
	// remove a ref on them
	    
    hash_scan_begin(&hs, map);		
	while ((hn = hash_scan_next(&hs)))
	{
		var = (TVariable *)hnode_get(hn);
		//cout << "var = " << (char *)hnode_getkey(hn) << "\n";
    	var->object->RemRef();
    	free((char *)hnode_getkey(hn));
		DeleteBinding(var);			
	}
		
	hash_free(map);
}

/*
 * function    : Clean
 * purpose     : Clear the map of all it contents
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_VarMap::Clean()
{		
	hscan_t hs;
    hnode_t *hn;
    TVariable *var;
               
    hash_scan_begin(&hs, map);		
	while ((hn = hash_scan_next(&hs)))
	{
		var = (TVariable *)hnode_get(hn);
    	var->object->RemRef();
		DeleteBinding(var);		
    	free((char *)hnode_getkey(hn));		
	}
	
	hash_free_nodes(map);		
}

/*
 * function    : Print
 * purpose     : Print the content of the map
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_VarMap::Print()
{
	string *str = new string;
  	string *value;
  	SQI_Object *obj;
  	hscan_t hs;
  	hnode_t *hn;
  	TVariable *var;  
 
 	hash_scan_begin(&hs, map);
	while ((hn = hash_scan_next(&hs)))
  	{
  		var = (TVariable *)hnode_get(hn);
  		obj = var->object;
      	value = obj->Print();
      	*str += (char *)hnode_getkey(hn);
      	*str += " = ";
      	*str += *value;
      	*str += "\n";
      	delete value;  			
  	}
     
  return str;
}

/*
 * function    : Remove
 * purpose     : Delete a variable
 * input       : 
 *
 * string *key,       the key
 *
 * output      : none 
 * side effect : none
 */
void SQI_VarMap::Remove(SQI_Squirrel *squirrel,string *key,char flag = DO_SYSTEM_HOOK)
{
	hnode_t *hn;	
  	TVariable *value;
  	SQI_Object *ret = NULL;
    
  	Lock();
   
   	hn = hash_lookup(map, key->c_str());
   
	if(hn) 
  	{
  		value = (TVariable *)hnode_get(hn);	
  		
  		// we call the binding if any	
  		if(value->erase)
  		{
  			ret = ExecBinding(squirrel,value->erase,flag,value->object);
  			if(ret)
  			{
  				SQI_Number *n = IsBool(ret);
  				if(n)
  					if(n->IsTrue())
  					{
  						// and we erase it	
  						value->object->RemRef();
  						DeleteBinding(value);
  						free((char *)hnode_getkey(hn));
  						hash_scan_delfree(map, hn);  					
  					}	
  			}
  		}	
  		else	
  		{	
  			// and we erase it	
  			value->object->RemRef();
  			DeleteBinding(value);
  			free((char *)hnode_getkey(hn));
  			hash_scan_delfree(map, hn);
  		}
  	}
	  
  	Unlock();
}

/*
 * function    : Set
 * purpose     : Set a value to a variable
 * input       : 
 *
 * string *key,       the key
 * SQI_Object *obj, the value
 *
 * output      : none 
 * side effect : none
 */
void SQI_VarMap::Set(SQI_Squirrel *squirrel,string *key,SQI_Object *obj,char flag = DO_SYSTEM_HOOK)
{
  	TVariable *old = NULL;
  	SQI_Object *ret = NULL; 
  	hnode_t *hn;
    
  	Lock();
  
  	hn = hash_lookup(map,key->c_str());
  
  	obj->AddRef(); // we add a reference

  	if(hn) //we remplace a value by another
  	{  	
  		old = (TVariable *)hnode_get(hn);	
  	
 		if(old->set)
 		{
 			ret = ExecBinding(squirrel,old->set,flag,old->object,obj); 
 			if(ret)
 			{
 				obj->REF--;
  				old->object->RemRef();
  				ret->AddRef();
 				old->object = ret;  				
 			}
 			else
 			{
 				old->object->RemRef();
 				old->object = obj;  	
 			}
 		}
 		else
 		{ 
    		old->object->RemRef();
 			old->object = obj;  
 		} 
 	
 		WakeUp(old->update);
 		old->update = NULL;
  	}
  	else
  	{
  		old = new TVariable;
    	old->object = obj;
    	old->get = NULL;
    	old->set = NULL;
    	old->erase = NULL;
    	old->update = NULL; 
  		hash_alloc_insert(map,strdup(key->c_str()),old);	
  	}	 
  
  	Unlock();
}

/*
 * function    : Set
 * purpose     : Set a value to a variable
 * input       : 
 *
 * char *key,       the key
 * SQI_Object *obj, the value
 *
 * output      : none 
 * side effect : none
 */
void SQI_VarMap::Set(SQI_Squirrel *squirrel,char *key,SQI_Object *obj,char flag = DO_SYSTEM_HOOK)
{
  	TVariable *old = NULL;
  	SQI_Object *ret = NULL; 
  	hnode_t *hn;
    
  	Lock();
   
  	hn = hash_lookup(map,key);
  
  	obj->AddRef(); // we add a reference

  	if(hn) //we remplace a value by another
  	{  	
  		old = (TVariable *)hnode_get(hn);	
  	
 		if(old->set)
 		{
 			ret = ExecBinding(squirrel,old->set,flag,old->object,obj); 
 			if(ret)
 			{
 				obj->REF--;
  				old->object->RemRef();
  				ret->AddRef();
 				old->object = ret;  				
 			}
 			else
 			{
 				old->object->RemRef();
 				old->object = obj;  	
 			}
 		}
 		else
 		{ 
    		old->object->RemRef();
 			old->object = obj;  
 		} 
 	
 		WakeUp(old->update);
 		old->update = NULL;
  	}
  	else
  	{
  		old = new TVariable;
    	old->object = obj;
    	old->get = NULL;
    	old->set = NULL;
    	old->erase = NULL;
    	old->update = NULL;
  		hash_alloc_insert(map,strdup(key),old);	
  	}	 
  
  	Unlock(); 
}

/*
 * function    : Get
 * purpose     : Get the value of a variable
 * input       : 
 *
 * string *key,       the key
 *
 * output      : SQI_Object *, the value
 * side effect : none
 */
SQI_Object *SQI_VarMap::Get(SQI_Squirrel *squirrel,string *key,char flag = DO_SYSTEM_HOOK)
{
	hnode_t *hn;
  	TVariable *value = NULL;
  	SQI_Object *ret = NULL; 
   
  	Lock();
  
  	hn = hash_lookup(map,key->c_str());
  		
  	if(hn)
  	{
  		value = (TVariable *)hnode_get(hn);
  	 	Unlock();
  	 	if(value->get)
  	 	{
 			ret = ExecBinding(squirrel,value->get,flag,value->object);
 			if(ret)
 				return ret; 
  	 	} 
  	 	return value->object;	
  	}
  	else
  	{
  		Unlock();
    	throw(new SQI_Exception(SQI_EX_UNKNOWVAR,key->c_str()));
  	}
}

/*
 * function    : Get
 * purpose     : Get the value of a variable
 * input       : 
 *
 * char *key,       the key
 *
 * output      : SQI_Object *, the value
 * side effect : none
 */
SQI_Object *SQI_VarMap::Get(SQI_Squirrel *squirrel,char *key,char flag = DO_SYSTEM_HOOK)
{
	hnode_t *hn;
  	TVariable *value = NULL;
  	SQI_Object *ret = NULL; 
   
  	Lock();
  
  	hn = hash_lookup(map,key);
  		
  	if(hn)
  	{
  		value = (TVariable *)hnode_get(hn);
  	 	Unlock();
  	 	if(value->get)
  	 	{
 			ret = ExecBinding(squirrel,value->get,flag,value->object);
 			if(ret)
 				return ret; 
  	 	} 
  	 	return value->object;	
  	}
  	else
  	{
  		Unlock();
    	throw(new SQI_Exception(SQI_EX_UNKNOWVAR,key));
  	} 
}

/*
 * function    : GetBinding
 * purpose     : Get the binding of a variable
 * input       : 
 *
 * string *key,       the key
 *
 * output      : TVariable *, the value
 * side effect : none
 */
TVariable *SQI_VarMap::GetBinding(string *key)
{
	hnode_t *hn;
  	TVariable *value = NULL;
   
  	Lock();
    
  	hn = hash_lookup(map,key->c_str());
    		
  	if(hn)
  	{
  		value = (TVariable *)hnode_get(hn);
  	 	Unlock();
  	 	return value;	
  	}
  	else
  	{
  		Unlock();
    	return NULL;
  	}
}

/*
 * function    : Exist
 * purpose     : Return the true if the variable exist
 * input       : 
 *
 * string *key,       the key
 *
 * output      : bool
 * side effect : none
 */
bool SQI_VarMap::Exist(string *key)
{ 	 
  	Lock();
  
  	if(hash_lookup(map,key->c_str()))
  	{  
 	 	Unlock();
 	 	return true;
  	} 
  	else
  	{ 
  		Unlock();
   		return false; 	
  	}	 
}

/*
 * function    : ExecBinding
 * purpose     : Execute a binding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel to use
 * TBinding *binding, binding to execute
 * SQI_Object *oldobj, old value of the variable
 * SQI_Object *newobj, new value of the variable (if any)
 *
 * output      : SQI_Object *, return the result of the execution if any
 * side effect : none
 */
SQI_Object *SQI_VarMap::ExecBinding(SQI_Squirrel *squirrel,TBind *binding,char flag,SQI_Object *oldobj,SQI_Object *newobj = NULL)
{
	// we execute the block or the function	
	
	Unlock();	
	
	SQI_Object *ret = NULL;
	
	if(binding->block)
	{
		try
		{
			squirrel->HopOnABlock(squirrel->interpreter,binding->block);
		} catch (SQI_Exception *ex) {
			string *str = ex->Print();
			*(squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
			squirrel->interpreter->Stream->Flush();
			delete str;
			delete ex;		
		}	
	}	
	else
	if(binding->function)
	{
		// for a function, we need to add 1 or 2 news inputs to the function
				
		if(binding->args)
		{	
			list<SQI_Object *> args = *(binding->args);
		
			// we add the args to the args list
			
			if(newobj)
				args.push_front(newobj);
			args.push_front(oldobj);
				
			binding->function->SetArgs(&args);
			
			try
			{
				ret = squirrel->HopOnLeave(squirrel->interpreter,binding->function);
			} catch (SQI_Exception *ex) {
					string *str = ex->Print();
					*(squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
					squirrel->interpreter->Stream->Flush();
					delete str;
					delete ex;	
			}
			
			binding->function->EmptyArgs();
		}
		else
		{
			list<SQI_Object *> args;
			
						
			if(newobj)
				args.push_front(newobj);
			args.push_front(oldobj);
				
			binding->function->SetArgs(&args);	
				
			
			try
			{
				ret = squirrel->HopOnLeave(squirrel->interpreter,binding->function);
			} catch (SQI_Exception *ex) {
				string *str = ex->Print();
				*(squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
				squirrel->interpreter->Stream->Flush();
				delete str;
				delete ex;	
			}		
			
			binding->function->EmptyArgs();
		}
	}
	
	// and a system function if any (always executed if any)	
		
	if(binding->func && flag)
	{
		(*(binding->func))(newobj,binding->data,squirrel);
	}
	
	Lock();	
	
	return ret;
}

void SQI_VarMap::SetSystemFunc(string *variable,void *data,char bind,void (*func)(SQI_Object *,void *,SQI_Squirrel *))
{
	TVariable *var = GetBinding(variable);	
	
	if(var)
	{		
		switch(bind)
		{
			case BIND_GET:
			{
				if(var->get)
				{
					var->get->data = data;
					var->get->func = func;			
				}
				else
				{
					var->get = new TBind;
					var->get->block = NULL;
					var->get->function = NULL;
					var->get->args = NULL;
					var->get->data = data;
					var->get->func = func;	
				}
				break;	
			}
			case BIND_SET:
			{
				if(var->set)
				{
					var->set->data = data;
					var->set->func = func;			
				}
				else
				{
					var->set = new TBind;
					var->set->block = NULL;
					var->set->function = NULL;
					var->set->args = NULL;
					var->set->data = data;
					var->set->func = func;	
				}
				break;			
			}
			case BIND_ERASE:
			{
				if(var->erase)
				{
					var->erase->data = data;
					var->erase->func = func;			
				}
				else
				{
					var->erase = new TBind;
					var->erase->block = NULL;
					var->erase->function = NULL;
					var->erase->args = NULL;
					var->erase->data = data;
					var->erase->func = func;	
				}
				break;
			}	
		}
	}	
}

void SQI_VarMap::RemoveSystemFunc(string *variable,char bind)
{
	TVariable *var = GetBinding(variable);

	if(var)
	{
		switch(bind)
		{
			case BIND_GET:
			{
				if(var->get)
				{
					var->get->data = NULL;
					var->get->func = NULL;
				}	
				break;	
			}
			case BIND_SET:
			{
				if(var->set)
				{
					var->set->data = NULL;
					var->set->func = NULL;
				}	
				break;	
			}
			case BIND_ERASE:		
			{
				if(var->erase)
				{
					var->erase->data = NULL;
					var->erase->func = NULL;
				}	
				break;	
			}
		}		
	}
}

void SQI_VarMap::AddWaiting(string *key,thread_id id)
{
	TVariable *var = GetBinding(key);

	Lock();

	if(var)
	{
		if(!var->update)
			var->update = new TWait;	
		
		var->update->threads.push_back(id);
	}	

	Unlock();	
			
}

void SQI_VarMap::WakeUp(TWait *threads)
{
	if(threads)
	{
		Lock();
		list<thread_id>::const_iterator i;
		for(i=threads->threads.begin();i!=threads->threads.end();i++)
			resume_thread(*i);	
		
		delete threads;
		Unlock();		
	}	
}
