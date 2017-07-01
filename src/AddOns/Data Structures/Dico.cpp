/*
 * Squirrel project
 *
 * file ?	Dico.cpp
 * what	?   Dico object
 * who	?	jlv
 * when	?	11/08/99
 * last	?	03/21/00
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-AddOn.h" 
#include "Dico.h"
#include <algorithm>

//DicoMemberDico DicoObjectMember;

MethodTable *DicoMethods = NULL;	// methods hash table

/*
 * function    : DicoObject
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
DicoObject::DicoObject(SQI_Squirrel *squirrel)
	:SQI_ThePointer(squirrel,SQI_DICO)
{
	SetClass(CLASS_DSTRUCTURE);	
	
	Dico = new map<string,SQI_Object *,less<string> >;	
	
	if(!DicoMethods)
		InitDicoObject();	
	
	//Members = &DicoObjectMember;
	Methods = DicoMethods;
}

/*
 * function    : DicoObject
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
DicoObject::DicoObject(SQI_Heap *target)
	:SQI_ThePointer(target,SQI_DICO)
{
	SetClass(CLASS_DSTRUCTURE);	
	
	Dico = new map<string,SQI_Object *,less<string> >;	
	
	//Members = &DicoObjectMember;
	
	if(!DicoMethods)
		InitDicoObject();
	
	Methods = DicoMethods;
}


/*
 * function    : DicoObject
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
DicoObject::~DicoObject()
{		
	delete Dico;
}

/*
 * function    : Export
 * purpose     : The object is aked to export himself to another heap
 * input       : 
 *
 * SQI_Heap *nheap, the target heap
 * char force, force the export even if the object is eternal
 *
 *
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
void DicoObject::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		//Locker.Lock();
		nheap->Import(this);	
		ObjDico::const_iterator i; 
   		     		    
	  	for(i=Dico->begin();i!=Dico->end();i++)
	  		(i->second)->Export(nheap,force);	
	  				  		
	  	//Locker.Unlock();
	}
}

/*
 * function    : Member
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, method name
 * deque<SQI_Object *> *inputs, arguments
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *DicoObject::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Methods)
	{
		OMeth MemberHook = NULL;		
			
	    MemberHook = Methods->Seek(member->c_str());		
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"DICO~","unknow member"));	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"DICO~","unsupported by the object"));	

	/*
	if(Members)
	{
		SQI_Object *(DicoObject::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
		MemberHook = (*Members)[*member];
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
		{
			Members->erase(*member);
			throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unknow member"));
		}	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unsupported by the object"));
	*/		
}

/*
 * function    : Set
 * purpose     : Set a value
 * input       : 
 *
 * long index, index of the value
 * SQI_Oject *value, value to store
 *
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Set(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  	SQI_Object *old=NULL;

	if(inputs->Length()==2)
	{
		SQI_Object *index = (*inputs)[0];
		SQI_Object *value = (*inputs)[1];	
						
		//Locker.Lock();

		string *key = index->Print();

	  	old = (*Dico)[*key];
	  	if(old)
	  	{
	  		old->Alone();
			old->RemRef();
	  	}			
   		 		  	
	  	(*Dico)[*key] = value;
	  	
	  	value->AddRef(); // we add a reference
	  	value->Contained(); // the object is contained
	  	
	  	if(value->heap!=heap)
	  		value->Export(heap);
	  
	  	//Locker.Unlock();
	  	delete key;	
	  	return NULL;			
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~set","need two arguments"));
	
}

/*
 * function    : Get
 * purpose     : Get a value
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * long index, index of the value
 *
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Get(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Object *index = (*inputs)[0];
		SQI_Object *value = NULL;
		string *key = index->Print();
	 		   
	  	//Locker.Lock(); 
	   
	  	value = (*Dico)[*key];
	  			  		
	  	//Locker.Unlock();
	  	
 		if(value)
 		{
 			delete key;
  			return value;
 		}	
  		else
  		{
  			Dico->erase(*key);
    		throw(new SQI_Exception(SQI_EX_OUTOFRANGE,key->c_str()));	
  		}
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~get","need one argument"));
}

/*
 * function    : Erase
 * purpose     : Erase the value as an specified index
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * long index, index of the value
 *
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Erase(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		string *key = (*inputs)[0]->Print();
		SQI_Object *value = NULL;
		ObjDico::const_iterator i; 
   		   
	  	//Locker.Lock(); 
	   	  	
	  	value = (*Dico)[*key];
	  			  			  	
 		if(value)
 		{
 			value->Alone();
	  		value->RemRef();
	  		Dico->erase(*key);
 		}	
  		else
  		{
  			Dico->erase(*key);
    		throw(new SQI_Exception(SQI_EX_OUTOFRANGE,key->c_str()));	
  		}
	  			  		
	  	//Locker.Unlock();
	  	delete key;
	  	
	  	return NULL;
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~erase","need one argument"));
}

/*
 * function    : Empty
 * purpose     : Empty the dico
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * long index, index of the value
 *
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Empty(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	EmptyIt();
	return NULL;
}

/*
 * function    : Keys
 * purpose     : Return a list of all the keys in the Dicotionary
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * long index, index of the value
 *
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Keys(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	ObjDico::const_iterator i;
	SQI_List *keys = new SQI_List(squirrel->LocalHeap);
		
	//Locker.Lock(); 
   
  	for(i=Dico->begin();i!=Dico->end();i++)
  		keys->Add2End(new SQI_String(squirrel->LocalHeap,i->first));
  	
  	//Locker.Unlock();		
		
	return keys;
}

/*
 * function    : Empty
 * purpose     : Erase all the value
 * input       : none
 * output      : none
 * side effect : none
 */
void DicoObject::EmptyIt()
{   	     	
	ObjDico::iterator i;  
	   
  	//Locker.Lock(); 
  	
  	if(Dico->size()!=0)
  	{ 
	  	for(i=Dico->begin();i!=Dico->end();i++)	
	  	{
	  		(i->second)->Alone();
	  		(i->second)->RemRef();	
	  	}	
	  	  	
	  	Dico->clear();
  	}	
	  	  	  	
  	//Locker.Unlock();
}

/*
 * function    : Min
 * purpose     : Return the minimum value in the array
 * input       : none
 * output      : SQI_Object *, the min
 * side effect : none
 */
SQI_Object *DicoObject::Min(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *min = NULL;
	ObjDico::iterator i;

	if(Dico->size())
	{
  		//Locker.Lock(); 
		i=Dico->begin();
	  	min = i->second;
	  		
		for(i++;i!=Dico->end();i++)	
			if(*min > i->second)
				min = i->second;
	 
  		//Locker.Unlock();
	}	
  	
  	if(!min)
  		min = new SQI_Number(squirrel->LocalHeap,0);
  	
  	return min;
}

/*
 * function    : Max
 * purpose     : Return the maximum value in the array
 * input       : none
 * output      : SQI_Object *, the max
 * side effect : none
 */
SQI_Object *DicoObject::Max(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *max = NULL;
	ObjDico::iterator i;

	if(Dico->size())
	{
  		//Locker.Lock(); 
		i=Dico->begin();
	  	max = i->second;
	  		
		for(i++;i!=Dico->end();i++)	
			if(*max < i->second)
				max = i->second;
	 
  		//Locker.Unlock();
	}	
  	
  	if(!max)
  		max = new SQI_Number(squirrel->LocalHeap,0);
  		  	
  	return max;
}

/*
 * function    : Avg
 * purpose     : Return the average value in the array
 * input       : none
 * output      : SQI_Object *, the max
 * side effect : none
 */
SQI_Object *DicoObject::Avg(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *avg = new SQI_Number(squirrel->LocalHeap,0);
	ObjDico::iterator i;
	
  	//Locker.Lock(); 
  	 	
	for(i=Dico->begin();i!=Dico->end();i++)	
		*avg += i->second; 
	  					
  	//Locker.Unlock();
  	
  	*avg /= (long)Dico->size();
  	
  	return avg;
}

/*
 * function    : Exist
 * purpose     : Check if a key exist in the dictionary
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::Exist(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Object *index = (*inputs)[0];
		SQI_Object *value = NULL;
		string *key = index->Print();
	 		   
	  	value = (*Dico)[*key];
	  			  		 	
 		if(value)
 		{
 			delete key;
  			return squirrel->interpreter->True;
 		}	
  		else
  		{
  			Dico->erase(*key);
    		return squirrel->interpreter->False;
  		}
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~exists","need one argument"));  
}

/*
 * function    : Size
 * purpose     : Return the number of element in the vector
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 *
 * output      : SQI_Object *, the size
 * side effect : none
 */
SQI_Object *DicoObject::Size(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	return new SQI_Number(squirrel->LocalHeap,(long)Dico->size());
}

/*
 * function    : IterateWithIndex
 * purpose     : Execute a function on all the element of the array (give the index in the array as first inputs
 *				 of the function
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::IterateWithIndex(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~iterate.i","need at least one argument"));
  else
    {
      	SQI_Keyword *name = IsKeyword((*inputs)[0]);
      	if(name)
      	{
      		SQI_Object *obj;
      		SQI_Object *ret = NULL;
      		SQI_Node *call = NULL;
      		SQI_Object *empty = new SQI_Number(squirrel->LocalHeap,0);
      		DicoObject *rarray = new DicoObject(squirrel);
      		list<SQI_Object *> *args = new list<SQI_Object *>;
      		
      		rarray->AddRef();
      		empty->AddRef();
      	
      		// we put all the other inputs in the inputs list now
      		      		
      		for(int i=1;i<inputs->Length();i++)
      		{
      			obj = (*inputs)[i];
      			obj->AddRef();
      			args->push_back(obj);
      		}
      		
      		call = dynamic_cast<SQI_Node *>(squirrel->interpreter->FindCall(name->Data()->c_str()));
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(args);
      		
      		try
      		{
      			if(Dico->size())
      			{
      				ObjDico::iterator i;
	      			for(i=Dico->begin();i!=Dico->end();i++)
	      			{		
	      				// each iteration, we change the 2 first elements of args
	      					      		
	      				obj = new SQI_String(squirrel->LocalHeap,i->first);	
	      					      					
	      				obj->AddRef();	
	      					      					
	      				args->push_front(i->second);	
	      				args->push_front(obj);
		      			
	      				ret = squirrel->HopOnLeave(squirrel->interpreter,call);
	      				if(ret)
	      				{
	      					(*(rarray->Dico))[i->first] = ret;
	      					ret->AddRef();
	      					ret->Contained();
	      				}	
	      			
	      				args->pop_front();
	      				args->pop_front();	
	      			
	      				obj->RemRef();	
	      			}
      			}
      		
      		} catch (SQI_Exception *ex) {
      			call->SetArgs(NULL);	
      			call->RemRef();
      			empty->RemRef();
      			rarray->REF--;
      			throw(ex);		
      		}	
      		
      		call->SetArgs(NULL);	
      		call->RemRef();
      		empty->RemRef();
      		rarray->REF--;
      		return rarray;
     		
      	}
      	else
      		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DICTIONARY~iterate.i","first input must be a word"));
    }
}

/*
 * function    : Iterate
 * purpose     : Execute a function on all the element of the array
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::Iterate(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~iterate","need at least one argument"));
  else
    {
      	SQI_Keyword *name = IsKeyword((*inputs)[0]);
      	if(name)
      	{
      		SQI_Object *obj;
      		SQI_Object *ret = NULL;
      		SQI_Node *call = NULL;
      		SQI_Object *empty = new SQI_Number(squirrel->LocalHeap,0);
      		DicoObject *rarray = new DicoObject(squirrel);
      		list<SQI_Object *> *args = new list<SQI_Object *>;
      		
      		rarray->AddRef();
      		empty->AddRef();
      	
      		// we put all the other inputs in the inputs list now
      		      		
      		for(int i=1;i<inputs->Length();i++)
      		{
      			obj = (*inputs)[i];
      			obj->AddRef();
      			args->push_back(obj);
      		}
      		     		
      		call = dynamic_cast<SQI_Node *>(squirrel->interpreter->FindCall(name->Data()->c_str()));
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(args);
      		
      		try
      		{
      			if(Dico->size())
      			{
      				ObjDico::iterator i;
      				
	      			for(i=Dico->begin();i!=Dico->end();i++)
	      			{		
	      				// each iteration, we change the 2 first elements of args
	      				
	      				args->push_front(i->second);
	   		
	      				ret = squirrel->HopOnLeave(squirrel->interpreter,call);
	      				if(ret)
	      				{
	      					(*(rarray->Dico))[i->first] = ret;
	      					ret->AddRef();
	      					ret->Contained();
	      				}	
	      			
	      				args->pop_front();	
	      			}
      			}
      		
      		} catch (SQI_Exception *ex) {
      			call->SetArgs(NULL);	
      			call->RemRef();
      			empty->RemRef();
      			rarray->REF--;
      			throw(ex);		
      		}	
      		
      		call->SetArgs(NULL);	
      		call->RemRef();
      		empty->RemRef();
      		rarray->REF--;
      		return rarray;
     		
      	}
      	else
      		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DICTIONARY~iterate","first input must be a word"));
    }
}

/*
 * function    : Find
 * purpose     : Find an element in the vector
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::Find(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	ObjDico::const_iterator i;
    	    	
    	for(i=Dico->begin();i!=Dico->end();i++)
    	{
    		if(*(i->second) == obj)
    			break;	
    	}	
    	  	   	
    	if(i==Dico->end())
    		return squirrel->interpreter->False;
    	else
    		return new SQI_String(squirrel->LocalHeap,i->first);
    }
}

/*
 * function    : FindAll
 * purpose     : Find all the occurence of an element in the vector
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::FindAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find.all","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	ObjDico::const_iterator i;
    	SQI_List *oc = new SQI_List(squirrel->LocalHeap);
    	
    	oc->AddRef();	
    	
    	for(i=Dico->begin();i!=Dico->end();i++)
    		if(*(i->second)==obj)
    			oc->Add2End(new SQI_String(squirrel->LocalHeap,i->first));	
    			
    	oc->REF--;
    	
  		return oc;
    }
}

/*
 * function    : FindLast
 * purpose     : Find the last occurence of an element in the vector
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::FindLast(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find.last","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	ObjDico::const_iterator i=Dico->end();
    	long j=Dico->size();
    	
    	if(!j)
	    	return squirrel->interpreter->False;	
    	
    	for(i--;i!=Dico->begin();i--,j--)
    		if(*(i->second)==obj)
    			break;	
    	
    	if(!j)
    		return squirrel->interpreter->False;
    	else
    		return new SQI_String(squirrel->LocalHeap,i->first);
    }
}

/*
 * function    : FindIf
 * purpose     : Find an element in the vector by executing a function
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::FindIf(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find.if","need at least 1 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	ObjDico::const_iterator i;
	    	SQI_Object *ret,*obj;
	    	SQI_Node *call = NULL;
	    	list<SQI_Object *> *args = new list<SQI_Object *>;
      		
      		// we put all the other inputs in the inputs list now
      		      				      		
      		for(int i=1;i<inputs->Length();i++)
      		{
      			obj = (*inputs)[i];
      			obj->AddRef();
      			args->push_back(obj);
      		}
      		
      		call = dynamic_cast<SQI_Node *>(squirrel->interpreter->FindCall(name->Data()->c_str()));
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(args);
	    	
	    	for(i=Dico->begin();i!=Dico->end();i++)
	    	{
      			args->push_front(i->second);
      					
      			ret = squirrel->HopOnLeave(squirrel->interpreter,call);

				if(ret)
				{
					SQI_Number *n = IsNumber(ret);
					if(n->IsTrue())
						break;
				}

      			args->pop_front();	
	    	}	
	    	
	    	if(i==Dico->end())
	    		return squirrel->interpreter->False;
	    	else
	    		return new SQI_String(squirrel->LocalHeap,i->first);
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DICTIONARY~find.if","second input must be a word"));
    }
}

/*
 * function    : FindIfAll
 * purpose     : Find an element in the vector by executing a function
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::FindIfAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find.if.all","need at least 2 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	ObjDico::const_iterator i;
	    	SQI_Object *ret,*obj;
	    	SQI_Node *call = NULL;
	    	SQI_List *ocu = new SQI_List(squirrel->LocalHeap);
	    	list<SQI_Object *> *args = new list<SQI_Object *>;
      		
      		// we put all the other inputs in the inputs list now
      		      		     		      		
      		for(int i=1;i<inputs->Length();i++)
      		{
      			obj = (*inputs)[i];
      			obj->AddRef();
      			args->push_back(obj);
      		}
      		
      		call = dynamic_cast<SQI_Node *>(squirrel->interpreter->FindCall(name->Data()->c_str()));
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(args);
	    	
	    	for(i=Dico->begin();i!=Dico->end();i++)
	    	{
      			args->push_front(i->second);
      					
      			ret = squirrel->HopOnLeave(squirrel->interpreter,call);

				if(ret)
				{
					SQI_Number *n = IsNumber(ret);
					if(n->IsTrue())
						ocu->Add2End(new SQI_String(squirrel->LocalHeap,i->first));
				}

      			args->pop_front();	
	    	}	
	    	
			return ocu;
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DICTIONARY~find.if.all","second input must be a word"));
    }
}

/*
 * function    : FindIfLast
 * purpose     : Find the last occurence of an element in the vector by executing a function
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *DicoObject::FindIfLast(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DICTIONARY~find.if.last","need at least 1 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	ObjDico::const_iterator i=Dico->end();
	    	long j=Dico->size();
	    	SQI_Object *ret,*obj;
	    	SQI_Node *call = NULL;
	    	list<SQI_Object *> *args = new list<SQI_Object *>;
	    	
	    	if(!j)
	    	{
	    		delete args;
	    		return squirrel->interpreter->False;	
	    	}
      		
      		// we put all the other inputs in the inputs list now     		
      		      		
      		for(int i=1;i<inputs->Length();i++)
      		{
      			obj = (*inputs)[i];
      			obj->AddRef();
      			args->push_back(obj);
      		}
      		
      		call = dynamic_cast<SQI_Node *>(squirrel->interpreter->FindCall(name->Data()->c_str()));
      		
      		squirrel->LocalHeap->Import(call);
      		
      		call->AddRef();
      		
      		call->SetArgs(args);
	    	
	    	for(i--;i!=Dico->begin();i--,j--)
	    	{
      			args->push_front(i->second);
      					
      			ret = squirrel->HopOnLeave(squirrel->interpreter,call);

				if(ret)
				{
					SQI_Number *n = IsNumber(ret);
					if(n->IsTrue())
						break;
				}

      			args->pop_front();	
	    	}	
	    	
	    	if(!j)
	    		return squirrel->interpreter->False;
	    	else
	    		return new SQI_String(squirrel->LocalHeap,i->first);
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DICTIONARY~find.if.last","second input must be a word"));
    }
}

// Legacy member functions

/*
 * function    : Suicidal
 * purpose     : The object will commit suicide if no reference to it and not eternal
 * input       : none
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
bool DicoObject::Suicidal(bool force=false)
{
    if(status || force)
      if(!REF)
      {
      		//cout << "DICO suicide from " << heap << "\n";
      		EmptyIt();
			delete this;
			return true;
      }
          
      return false;
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *DicoObject::Print(int prec = 3)
{	
	ObjDico::const_iterator i;
	ostrstream out;
  	string *str;
	   
  	//Locker.Lock(); 
  	
  	for(i=Dico->begin();i!=Dico->end();i++)	
  	{
  		str = (i->second)->Print(prec);
  		out << i->first << ":" << *str << "\n";
  		delete str;	
  	}	
    	  	
  	//Locker.Unlock();
  	
  	out << '\0';	 
  
  	string *ret = new string(out.str());
  	out.freeze(false);
  	return ret;
}	
	
/*
 * function    : Show
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *DicoObject::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Clone
 * purpose     : clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *DicoObject::Clone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
		
	ObjDico::const_iterator i;
	DicoObject *clone = new DicoObject(hp);

	for(i=Dico->begin();i!=Dico->end();i++)
	{
		(*(clone->Dico))[i->first] = i->second;
		(i->second)->AddRef();
		(i->second)->Contained();	
	}

	return clone;
}

/*
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *DicoObject::DeepClone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
	
	SQI_Object *obj;
	ObjDico::const_iterator i;
	DicoObject *clone = new DicoObject(hp);

	for(i=Dico->begin();i!=Dico->end();i++)
	{
		obj = (i->second)->DeepClone(hp);
		(*(clone->Dico))[i->first] = obj;
		obj->AddRef();
		obj->Contained();	
	}

	return clone;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *DicoObject::Dump()
{ 
	  //string *value = this->Print();
	  ostrstream out;
	 
	  out << "DICTIONARY[" << ID << "][" << REF << "][" << status << "]" << '\0'; // - " << *value << '\0';
	 
	  //delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

/*
 * function    : Neg
 * purpose     : Return a new object which is the negative value of the array
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Neg()
{ 
	ObjDico::const_iterator i;
	DicoObject *neg = new DicoObject(heap);
	SQI_Object *obj;

	for(i=Dico->begin();i!=Dico->end();i++)
	{
		obj = (i->second)->Neg();
		(*(neg->Dico))[i->first] = obj;
		obj->Export(heap,SQI_NORMAL);			
		obj->AddRef();
		obj->Contained();		
	}
	
	return neg;
}

/*
 * function    : Inv
 * purpose     : Return a new object which is the inverse value of the array
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *DicoObject::Inv()
{ 
	ObjDico::const_iterator i;
	DicoObject *neg = new DicoObject(heap);
	SQI_Object *obj;

	for(i=Dico->begin();i!=Dico->end();i++)
	{
		obj = (i->second)->Inv();
		(*(neg->Dico))[i->first] = obj;
		obj->Export(heap,SQI_NORMAL);			
		obj->AddRef();
		obj->Contained();		
	}
	
	return neg;
}


/*
 * function    : operator==
 * purpose     : test if the object == something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator==(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2=NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1!=obj2)
							return false;
					}
					else
						return false;	
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator>
 * purpose     : test if the object > something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator>(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2 = NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1<=obj2)
							return false;
					}
					else
						return false;	
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator>=
 * purpose     : test if the object >= something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator>=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2 = NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1<obj2)
							return false;
					}
					else
						return false;	
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator<
 * purpose     : test if the object < something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator<(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2 = NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1>=obj2)
							return false;
					}
					else
						return false;	
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator<=
 * purpose     : test if the object <= something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator<=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2 = NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1>obj2)
							return false;
					}
					else
						return false;	
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator!=
 * purpose     : test if the object != something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool DicoObject::operator!=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			ObjDico::const_iterator i;
			SQI_Object *obj1,*obj2 = NULL;	
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj1 = i->second;
				obj2 = (*(a->Dico))[i->first];
				if(!obj2)
				{
					a->Dico->erase(i->first);
					continue;		
				}
				
				if(obj1!=obj2)
				{
					if(obj1 && obj2)
					{
						if(*obj1!=obj2)
							return true;
					}
					else
						return true;	
				}
			}
			
			return false;
		}
		else
			return true;
	}
	else
		return true;	
}

/*
 * function    : operator+
 * purpose     : Add an object to the array
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::operator+(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			DicoObject *result = new DicoObject(heap);
			
			try
			{					
				SQI_Object *obj1,*obj2,*obj;
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj = *obj1 + obj2;
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"dictionary + object","unsupported by the array"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = *(i->second) + op2;
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}
		
}

/*
 * function    : operator-
 * purpose     : Sub an object to the array
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::operator-(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			DicoObject *result = new DicoObject(heap);
			
			try
			{			
				SQI_Object *obj,*obj1,*obj2 = NULL;		
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj = *obj1 - obj2;
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"dictionary - object","unsupported by the array"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = *(i->second) - op2;
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}
		
}

/*
 * function    : operator*
 * purpose     : Mult an object to the array
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::operator*(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);

			DicoObject *result = new DicoObject(heap);
			
			try
			{					
				SQI_Object *obj,*obj1,*obj2=NULL;		
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj = *obj1 * obj2;
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"dictionary * object","unsupported by the dictionary"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = *(i->second) * op2;
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}		
}

/*
 * function    : operator/
 * purpose     : Divide an object to the array
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::operator/(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			DicoObject *result = new DicoObject(heap);
			
			try
			{	
				SQI_Object *obj,*obj1,*obj2 = NULL;		
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj =  *obj1 / obj2;
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"dictionary / object","unsupported by the dictionary"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = *(i->second) / op2;
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}		
}

/*
 * function    : Div
 * purpose     : Divide (integer division) an object to the array
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::Div(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			DicoObject *result = new DicoObject(heap);
			
			try
			{			
				SQI_Object *obj,*obj1,*obj2 = NULL;		
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{		
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj = obj1->Div(obj2);
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"dictionary // object","unsupported by the dictionary"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = (i->second)->Div(op2);
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}		
}

/*
 * function    : InvDiv
 * purpose     : Divide (integer division) the array value by the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *DicoObject::InvDiv(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_DICO)
		{
			DicoObject *a = dynamic_cast<DicoObject *>(right);
			DicoObject *result = new DicoObject(heap);
			
			try
			{					
				SQI_Object *obj,*obj1,*obj2 = NULL;		
				ObjDico::const_iterator i;
				for(i=Dico->begin();i!=Dico->end();i++)
				{		
					obj1 = i->second;
					obj2 = (*(a->Dico))[i->first];
					if(!obj2)
					{
						a->Dico->erase(i->first);
						continue;		
					}
					else
					{
						obj = obj1->InvDiv(obj2);
						(*(result->Dico))[i->first] = obj;
						obj->AddRef();
						obj->Contained();
					}
				}
			} catch (SQI_Exception *ex) {
				result->Suicidal();
				throw(ex);
			}
				
			return result;
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"object // dictionary","unsupported by the dictionary"));
	}
	else
	{
		DicoObject *result = new DicoObject(heap);
		
		try
		{
			SQI_Object *obj;
			ObjDico::const_iterator i;
			for(i=Dico->begin();i!=Dico->end();i++)
			{
				obj = op2->InvDiv(i->second);
				(*(result->Dico))[i->first] = obj;
				obj->AddRef();
				obj->Contained();
			}
			
		} catch (SQI_Exception *ex) {
			result->Suicidal();
			throw(ex);
		}
			
		return result;	
	}		
}

/*
 * function    : Archive
 * purpose     : Store the list in an message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t DicoObject::Archive(BMessage *archive)
{	
	ObjDico::const_iterator i;
	// first we set the type if the object
	archive->AddInt8("SQI_Object",SQI_DICO);
	// we iterate on each element and store the archive of each element in the archive of the list
	for(i=Dico->begin();i!=Dico->end();i++)
  	{
 		BMessage element;
 		(i->second)->Archive(&element);
 		archive->AddString("index",i->first.c_str());
 		archive->AddMessage("element",&element);
  	}		
  	
  	// that's all
  	
  	return B_OK;
}

/*
 * function    : Instantiate
 * purpose     : Fill the list with the element in the archive message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t DicoObject::Instantiate(SQI_Heap *target,BMessage *archive)
{
	type_code type;
	int32 count;
	SQI_Object *elem;
	const char *index;	
	
	archive->GetInfo("element",&type,&count);
	
	for(int32 i=0;i<count;i++)
	{
		BMessage message;
		archive->FindMessage("element",i,&message);
		elem = InstantiateAnObject(target,&message);
		if(elem)
		{
			index = archive->FindString("index",i);
			elem->AddRef();
			elem->Contained();
			(*Dico)[string(index)] = elem;
		}
	}				
	
	return B_OK;
}

// creation functions and init function

SQI_ThePointer *NewDicoObject(SQI_Squirrel *squirrel)
{
	return new DicoObject(squirrel);
	//return new SQI_ThePointer(squirrel,SQI_DICO);
}

// Fill up the member map
void InitDicoObject()
{
	AddArchive(SQI_DICO,InstantiateADico);
			
	DicoMethods = new MethodTable();
	
	DicoMethods->Add("get",(OMeth)(&DicoObject::Get));
	DicoMethods->Add("set",(OMeth)(&DicoObject::Set));
	DicoMethods->Add("size",(OMeth)(&DicoObject::Size));
	DicoMethods->Add("min",(OMeth)(&DicoObject::Min));
	DicoMethods->Add("max",(OMeth)(&DicoObject::Max));
	DicoMethods->Add("av",(OMeth)(&DicoObject::Avg));
	DicoMethods->Add("exists",(OMeth)(&DicoObject::Exist));
	DicoMethods->Add("iterate",(OMeth)(&DicoObject::Iterate));
	DicoMethods->Add("iterate.i",(OMeth)(&DicoObject::IterateWithIndex));		
	DicoMethods->Add("find",(OMeth)(&DicoObject::Find));
	DicoMethods->Add("find.all",(OMeth)(&DicoObject::FindAll));
	DicoMethods->Add("find.last",(OMeth)(&DicoObject::FindLast));
	DicoMethods->Add("find.if",(OMeth)(&DicoObject::FindIf));
	DicoMethods->Add("find.if.all",(OMeth)(&DicoObject::FindIfAll));	
	DicoMethods->Add("find.if.last",(OMeth)(&DicoObject::FindIfLast));	
	DicoMethods->Add("erase",(OMeth)(&DicoObject::Erase));
	DicoMethods->Add("empty",(OMeth)(&DicoObject::Empty));	
	DicoMethods->Add("keys",(OMeth)(&DicoObject::Keys));		
		
	/*	
	DicoObjectMember[string("get")] = &DicoObject::Get;
	DicoObjectMember[string("set")] = &DicoObject::Set;
	DicoObjectMember[string("size")] = &DicoObject::Size;
	DicoObjectMember[string("min")] = &DicoObject::Min;
	DicoObjectMember[string("max")] = &DicoObject::Max;
	DicoObjectMember[string("av")] = &DicoObject::Avg;
	DicoObjectMember[string("exists")] = &DicoObject::Exist;
	DicoObjectMember[string("iterate")] = &DicoObject::Iterate;
	DicoObjectMember[string("iterate.i")] = &DicoObject::IterateWithIndex;		
	DicoObjectMember[string("find")] = &DicoObject::Find;
	DicoObjectMember[string("find.all")] = &DicoObject::FindAll;
	DicoObjectMember[string("find.last")] = &DicoObject::FindLast;
	DicoObjectMember[string("find.if")] = &DicoObject::FindIf;
	DicoObjectMember[string("find.if.all")] = &DicoObject::FindIfAll;	
	DicoObjectMember[string("find.if.last")] = &DicoObject::FindIfLast;	
	DicoObjectMember[string("erase")] = &DicoObject::Erase;
	DicoObjectMember[string("empty")] = &DicoObject::Empty;	
	DicoObjectMember[string("keys")] = &DicoObject::Keys;
	*/	
}

void UninitDicoObject()
{
	if(DicoMethods)
		delete DicoMethods; 	
}

/*
 * function    : InstantiateADico
 * purpose     : Instantiate a dico from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateADico(SQI_Heap *target,BMessage *archive)
{
	DicoObject *vector = new DicoObject(target);
	
	if(vector->Instantiate(target,archive)==B_OK)
		return vector;
	else
	{
		vector->Suicidal(true);
		return NULL;	
	}	
}
