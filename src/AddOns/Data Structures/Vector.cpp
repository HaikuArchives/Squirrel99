/*
 * Squirrel project
 *
 * file ?	Vector.cpp
 * what	?   Vector object
 * who	?	jlv
 * when	?	11/08/99
 * last	?	03/21/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-AddOn.h" 
#include "Vector.h"
#include <algorithm>

//VectorMemberMap VectorObjectMember;

MethodTable *VectorMethods = NULL;	// methods hash table

/*
 * function    : VectorObject
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
VectorObject::VectorObject(SQI_Squirrel *squirrel,long capacity,SQI_Object *init = NULL)
	:SQI_ThePointer(squirrel,SQI_VECTOR)
{
	SetClass(CLASS_DSTRUCTURE);	
	
	Vector = new vector<SQI_Object *>;	
	
	if(capacity)
	{
		SQI_Object *zero;
		if(!init)
			zero = new SQI_Number(squirrel->LocalHeap,0);
		else
			zero = init;
		
		Vector->resize(capacity);
		for(long i=0;i<capacity;i++)
		{
			zero->AddRef();
			zero->Contained();
			(*Vector)[i] = zero;
		}
	}

	//Members = &VectorObjectMember;
	
	if(!VectorMethods)
		InitVectorObject();	

	Methods = VectorMethods;
}

/*
 * function    : VectorObject
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
VectorObject::VectorObject(SQI_Heap *target,long capacity,SQI_Object *init = NULL)
	:SQI_ThePointer(target,SQI_VECTOR)
{
	SetClass(CLASS_DSTRUCTURE);	
	
	Vector = new vector<SQI_Object *>;	
	
	if(capacity)
	{
		
		SQI_Object *zero;
		if(!init)
			zero = new SQI_Number(target,0);
		else
			zero = init;
			
		Vector->resize(capacity);
		for(long i=0;i<capacity;i++)
		{
			zero->AddRef();
			zero->Contained();
			(*Vector)[i] = zero;
		}
	}
		
	//Members = &VectorObjectMember;
	
	if(!VectorMethods)
		InitVectorObject();	

	Methods = VectorMethods;	
}


/*
 * function    : VectorObject
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
VectorObject::VectorObject(SQI_Squirrel *squirrel,SQI_List *init)
	:SQI_ThePointer(squirrel,SQI_VECTOR)
{
	SetClass(CLASS_DSTRUCTURE);	
	
	long i;
	list<SQI_Object *>::const_iterator *j = init->Iterator();
	list<SQI_Object *>::const_iterator *e = init->End();
		
	Vector = new vector<SQI_Object *>;		
	Vector->resize(init->Length());

	for(i=0,*j;*j!=*e;(*j)++,i++)
	{
		(*Vector)[i] = *(*j);
		(*Vector)[i]->Contained();
		(*Vector)[i]->AddRef();	
	}
	
	delete j;
	delete e;
	
	//Members = &VectorObjectMember;
	
	if(!VectorMethods)
		InitVectorObject();	

	Methods = VectorMethods;	
}

/*
 * function    : VectorObject
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
VectorObject::~VectorObject()
{		
	delete Vector;
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
void VectorObject::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		vector<SQI_Object *>::iterator i;  
   		   
	  	//Lock(); 
	  	for(i=Vector->begin();i!=Vector->end();i++)
	  		(*i)->Export(nheap,force);
	  			  		
	  	//Unlock();
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
SQI_Object *VectorObject::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Methods)
	{
		OMeth MemberHook = NULL;		
			
	    MemberHook = Methods->Seek(member->c_str());		
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"VECTOR~","unknow member"));	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"VECTOR~","unsupported by the object"));	


/*
	if(Members)
	{
		SQI_Object *(VectorObject::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
 * function    : Append
 * purpose     : append a value to the end of the vector
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel running
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : none
 * side effect : none
 */
SQI_Object *VectorObject::Append(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{	
		SQI_Object *value = (*inputs)[0];	
	
		//Lock();
	   		   		 
	  	Vector->push_back(value);
	  	
	  	value->Contained(); // the
	  	value->AddRef(); // we add a reference
	  
	  	//Unlock();
	  	return NULL;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~append","need one input"));	
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
SQI_Object *VectorObject::Set(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  	SQI_Object *old=NULL;

	if(inputs->Length()==2)
	{
		SQI_Number *ind = IsInteger((*inputs)[0]);
		if(!ind)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~set","index must be an integer"));	
		else
		{
			SQI_Object *value = (*inputs)[1];	
			long index = Number2Long(ind)-1;	
						
			//Lock();
	
			if(index>Vector->capacity() || Vector->size()==0)
			{
				//Unlock();
				throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"VECTOR~set","index above capacity"));			
			}  	
   
			if(index<0) 
			{
				//Unlock();
				throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"VECTOR~set","index cannot be of negative value"));			
			}	
				
			if(index<Vector->size())
			{
			  	old = (*Vector)[index];
			  	if(old)
			  	{
			  		old->Alone();
					old->RemRef();
			  	}
			}
				
	   		 		  	
		  	(*Vector)[index] = value;
		  	
		  	value->AddRef(); // we add a reference
		  	value->Contained(); // the object is contained
		  	
		  	if(value->heap!=heap)
		  		value->Export(heap);
		  
		  	//Unlock();	
		  	return NULL;			
		}
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~set","need two arguments"));
	
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
SQI_Object *VectorObject::Get(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Number *ind = IsInteger((*inputs)[0]);
		if(!ind)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~get","index must be an integer"));
		else
		{
			SQI_Object *value = NULL;
	  		long index = Number2Long(ind)-1; 
	   
			if(index<0 || index>=Vector->size())
				throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"VECTOR~get"));	 
			   
		  	//Lock(); 
		   
		  	value = (*Vector)[index];
		  			  		
		  	//Unlock();
		  	
		  	return value;
		}
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~get","need one argument"));
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
SQI_Object *VectorObject::Erase(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Number *ind = IsInteger((*inputs)[0]);
		if(!ind)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~erase","index must be an integer"));
		else
		{
			SQI_Object *value = NULL;
			vector<SQI_Object *>::iterator i;
	  		long index = Number2Long(ind)-1;
	  		long j = 0; 
	   
			if(index<0 || index>=Vector->size())
				throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"VECTOR~erase"));	 
			   
		  	//Lock(); 
		   
		  	for(i=Vector->begin();i!=Vector->end();i++,j++)
		  	{
		  			if(j==index)
		  			{
		  				Vector->erase(i);
		  				break;
		  			}
		  	}
		  			  		
		  	//Unlock();
		  	
		  	return NULL;
		}
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~erase","need one argument"));
}

/*
 * function    : Empty
 * purpose     : Erase all the value
 * input       : none
 * output      : none
 * side effect : none
 */
void VectorObject::Empty()
{   	     	
	vector<SQI_Object *>::iterator i;  
	   
  	//Lock(); 
  	
  	if(Vector->size()!=0)
  	{ 
	  	for(i=Vector->begin();i!=Vector->end();i++)
	  		if(*i)	
	  		{
	  			(*i)->Alone();
	  			(*i)->RemRef();	
	  		}	
	  	  	
	  	Vector->clear();
  	}	
	  	  	  	
  	//Unlock();
}

/*
 * function    : Min
 * purpose     : Return the minimum value in the array
 * input       : none
 * output      : SQI_Object *, the min
 * side effect : none
 */
SQI_Object *VectorObject::Min(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *min = NULL;
	vector<SQI_Object *>::iterator i;

	if(Vector->size())
	{
  		//Lock(); 
		for(i=Vector->begin();i!=Vector->end();i++)
	  		if(*i)	
	  		{
	  			min = *i;
	  			break;	
	  		}
	  		
		for(i;i!=Vector->end();i++)
	  		if(*i)	
				if(*min > *i)
					min = *i;
	 
  		//Unlock();
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
SQI_Object *VectorObject::Max(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *max = NULL;
	vector<SQI_Object *>::iterator i;

	if(Vector->size())
	{
  		//Lock(); 
		for(i=Vector->begin();i!=Vector->end();i++)
	  		if(*i)	
	  		{
	  			max = *i;
	  			break;	
	  		}
	  		
		for(i;i!=Vector->end();i++)
	  		if(*i)	
				if(*max < *i)
					max = *i;
	 
  		//Unlock();
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
SQI_Object *VectorObject::Avg(SQI_Squirrel *squirrel,SQI_Args *inputs)
{   	   
	SQI_Object *avg = new SQI_Number(squirrel->LocalHeap,0);
	vector<SQI_Object *>::iterator i;
	
  	//Lock(); 
  	 	
	for(i=Vector->begin();i!=Vector->end();i++)
		if(*i)	
			*avg += *i; 
	  					
  	//Unlock();
  	
  	*avg /= (long)Vector->size();
  	
  	return avg;
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
SQI_Object *VectorObject::Size(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	return new SQI_Number(squirrel->LocalHeap,(long)Vector->size());
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
SQI_Object *VectorObject::IterateWithIndex(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~iterate.i","need at least one argument"));
  else
    {
      	SQI_Keyword *name = IsKeyword((*inputs)[0]);
      	if(name)
      	{
      		SQI_Object *obj;
      		SQI_Object *ret = NULL;
      		SQI_Node *call = NULL;
      		SQI_Object *empty = new SQI_Number(squirrel->LocalHeap,0);
      		VectorObject *rarray = new VectorObject(squirrel);
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
      			if(Vector->size())
      			{
      				vector<SQI_Object *>::iterator i;
      				long j;
	      			for(j=1,i=Vector->begin();i!=Vector->end();i++,j++)
	      			{		
	      				// each iteration, we change the 2 first elements of args
	      				
	      				obj = new SQI_Number(squirrel->LocalHeap,j);
	      				obj->AddRef();
	      				
	      				if(*i)
	      					args->push_front(*i);
	      				else
	      					args->push_front(empty);		
	      				
	      				args->push_front(obj);
		      			
	      				ret = squirrel->HopOnLeave(squirrel->interpreter,call);
	      				if(ret)
	      				{
	      					rarray->Vector->push_back(ret);
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
      		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~iterate.i","first input must be a word"));
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
SQI_Object *VectorObject::Iterate(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~iterate","need at least one argument"));
  else
    {
      	SQI_Keyword *name = IsKeyword((*inputs)[0]);
      	if(name)
      	{
      		SQI_Object *obj;
      		SQI_Object *ret = NULL;
      		SQI_Node *call = NULL;
      		SQI_Object *empty = new SQI_Number(squirrel->LocalHeap,0);
      		VectorObject *rarray = new VectorObject(squirrel);
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
      			if(Vector->size())
      			{
      				vector<SQI_Object *>::iterator i;
      				
	      			for(i=Vector->begin();i!=Vector->end();i++)
	      			{		
	      				// each iteration, we change the 2 first elements of args
	      				
	      				if(*i)
	      					args->push_front(*i);
	      				else
	      					args->push_front(empty);		
	      				
	      				ret = squirrel->HopOnLeave(squirrel->interpreter,call);
	      				if(ret)
	      				{
	      					rarray->Vector->push_back(ret);
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
      		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~iterate","first input must be a word"));
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
SQI_Object *VectorObject::Find(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	vector<SQI_Object *>::const_iterator i;
    	long j=1;
    	
    	for(i=Vector->begin();i!=Vector->end();i++,j++)
    		if(*i)
    			if(*(*i)==obj)
    				break;	
    	
    	if(i==Vector->end())
    		return squirrel->interpreter->False;
    	else
    		return new SQI_Number(squirrel->LocalHeap,j);
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
SQI_Object *VectorObject::FindAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find.all","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	vector<SQI_Object *>::const_iterator i;
    	long j=1;
    	SQI_List *oc = new SQI_List(squirrel->LocalHeap);
    	
    	oc->AddRef();	
    	
    	for(i=Vector->begin();i!=Vector->end();i++,j++)
    		if(*i)
    			if(*(*i)==obj)
    			{
    				oc->Add2End(new SQI_Number(squirrel->LocalHeap,j));	
    			}	
    			
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
SQI_Object *VectorObject::FindLast(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find.last","need one argument"));
  else
    {
    	SQI_Object *obj = (*inputs)[0];
    	vector<SQI_Object *>::const_iterator i=Vector->end();
    	long j=Vector->size();
    	
    	if(!j)
	    	return squirrel->interpreter->False;	
    	
    	for(i--;i!=Vector->begin();i--,j--)
    	{
    		if(*i)
    			if(*(*i)==obj)
    				break;	
    	}
    	
    	if(!j)
    		return squirrel->interpreter->False;
    	else
    		return new SQI_Number(squirrel->LocalHeap,j);
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
SQI_Object *VectorObject::FindIf(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find.if","need at least 1 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	vector<SQI_Object *>::const_iterator i;
	    	long j=1;
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
	    	
	    	for(i=Vector->begin();i!=Vector->end();i++,j++)
	    	{
      			args->push_front(*i);
      					
      			ret = squirrel->HopOnLeave(squirrel->interpreter,call);

				if(ret)
				{
					SQI_Number *n = IsNumber(ret);
					if(n->IsTrue())
						break;
				}

      			args->pop_front();	
	    	}	
	    	
	    	if(i==Vector->end())
	    		return squirrel->interpreter->False;
	    	else
	    		return new SQI_Number(squirrel->LocalHeap,j);
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~find.if","second input must be a word"));
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
SQI_Object *VectorObject::FindIfAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find.if.all","need at least 2 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	vector<SQI_Object *>::const_iterator i;
	    	long j=1;
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
	    	
	    	for(i=Vector->begin();i!=Vector->end();i++,j++)
	    	{
      			args->push_front(*i);
      					
      			ret = squirrel->HopOnLeave(squirrel->interpreter,call);

				if(ret)
				{
					SQI_Number *n = IsNumber(ret);
					if(n->IsTrue())
						ocu->Add2End(new SQI_Number(squirrel->LocalHeap,j));
				}

      			args->pop_front();	
	    	}	
	    	
			return ocu;
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~find.if.all","second input must be a word"));
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
SQI_Object *VectorObject::FindIfLast(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
  if(inputs->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VECTOR~find.if.last","need at least 1 arguments"));
  else
    {
    	SQI_Keyword *name = IsKeyword((*inputs)[0]);
    	
    	if(name)
    	{
	    	vector<SQI_Object *>::const_iterator i=Vector->end();
	    	long j=Vector->size();
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
	    	
	    	for(i--;i!=Vector->begin();i--,j--)
	    	{
      			args->push_front(*i);
      					
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
	    		return new SQI_Number(squirrel->LocalHeap,j);
    	} 
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~find.if.last","second input must be a word"));
    }
}

/*
 * function    : Swap
 * purpose     : Swap 2 elements of the vector
 * input       :
 *
 * long f, 
 * long t, 
 *
 * output      : none
 * side effect : none
 */
void VectorObject::Swap(long f,long t)
{
	SQI_Object *tmp = (*Vector)[f];
	(*Vector)[f] = (*Vector)[t];
	(*Vector)[t] = tmp;
}

/*
 * function    : AscSort
 * purpose     : Sort the array in ascendent order
 * input       :
 *
 * long f, from the index
 * long t, to the index 
 *
 * output      : none
 * side effect : none
 */
void VectorObject::AscSort(long f,long t)
{
	if(f<t)
	{
		long p = AscPartition(f,t);
		AscSort(f,p-1);
		AscSort(p+1,t);	
	}
}

/*
 * function    : DecSort
 * purpose     : Sort the array in descendent order
 * input       :
 *
 * long f, from the index
 * long t, to the index 
 *
 * output      : none
 * side effect : none
 */
void VectorObject::DecSort(long f,long t)
{
	if(f<t)
	{
		long p = DecPartition(f,t);
		DecSort(f,p-1);
		DecSort(p+1,t);	
	}	
}

/*
 * function    : AscPartition
 * purpose     : Sort a partition of the array in ascendant order
 * input       :
 *
 * long f, from the index
 * long t, to the index 
 *
 * output      : none
 * side effect : none
 */
long VectorObject::AscPartition(long p,long r)
{
	SQI_Object *x,*y;
	long i,j;
	i = p+1;
	j = i;
	Swap(p,(p+r)/2);
	x = (*Vector)[p];
	while (j<=r) 
	{
		y = (*Vector)[j];
		if (*y >= x)
			j++;
		else 
		{
			Swap(i,j);
			i++;
			j++;	
		}	
	}
	
	Swap(p,i-1);
	return i-1;
}

/*
 * function    : DecPartition
 * purpose     : Sort a partition of the array in ascendant order
 * input       :
 *
 * long f, from the index
 * long t, to the index 
 *
 * output      : none
 * side effect : none
 */
long VectorObject::DecPartition(long p,long r)
{
	SQI_Object *x,*y;
	long i,j;
	i = p+1;
	j = i;
	Swap(p,(p+r)/2);
	x = (*Vector)[p];
	while (j<=r) 
	{
		y = (*Vector)[j];
		if (*y <= x)
			j++;
		else 
		{
			Swap(i,j);
			i++;
			j++;	
		}	
	}
	
	Swap(p,i-1);
	return i-1;
}

/*
 * function    : Sort
 * purpose     : Sort the array (mutate the array)
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *VectorObject::Sort(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	char mode = 0;
	
	if(inputs->Length())
	{
		SQI_Keyword *key = IsKeyword((*inputs)[0]);
		if(key)
		{
			string *m = key->Data();
			
			if(*m == "asc")
				mode = 0;
			else
				if(*m == "des")
					mode = 1;
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"VECTOR~sort","order must be \"asc or \"des "));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VECTOR~sort","first input must be a word"));	
	}		
	
	//Lock();	
	
	if(!mode)
		AscSort(0,Vector->size()-1);	
	else		
		DecSort(0,Vector->size()-1);	
		
	//Unlock();	
		
	return NULL;	
}

/*
 * function    : SortNew
 * purpose     : Sort the array (create a new one)
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *VectorObject::SortNew(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	VectorObject *clone = dynamic_cast<VectorObject *>(Clone(heap));
	
	clone->Sort(squirrel,inputs);
	
	return clone;			
}

/*
 * function    : Reverse
 * purpose     : Reverse the array (mutate the array)
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *VectorObject::Reverse(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	reverse(Vector->begin(),Vector->end());		
}

/*
 * function    : ReverseNew
 * purpose     : Reverse the array (create a new one)
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel executing
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, an array wich contain the result of the function on all the element of the array
 * side effect : none
 */
SQI_Object *VectorObject::ReverseNew(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	VectorObject *clone = dynamic_cast<VectorObject *>(Clone(heap));
	
	clone->Reverse(squirrel,inputs);
	
	return clone;			
}



// Legacy member functions

/*
 * function    : Suicidal
 * purpose     : The object will commit suicide if no reference to it and not eternal
 * input       : none
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
bool VectorObject::Suicidal(bool force=false)
{
    if(status || force)
      if(!REF)
      {
      		Empty();
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
string *VectorObject::Print(int prec = 3)
{	
	vector<SQI_Object *>::const_iterator i;
	ostrstream out;
  	string *str;
	   
  	//Lock(); 
  	 
  	for(i=Vector->begin();i!=Vector->end();i++)	
  	{
  		if(*i)
  		{
  				str = (*i)->Print(prec);
  				out << *str << " ";
  				delete str;	
  		}
  	}	
    	  	
  	//Unlock();
  	
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
string *VectorObject::Show(int prec = 3)
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
SQI_Object *VectorObject::Clone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
		
	vector<SQI_Object *>::const_iterator i;
	VectorObject *clone = new VectorObject(hp,0);

	for(i=Vector->begin();i!=Vector->end();i++)
		if(*i)
		{
			clone->Vector->push_back(*i);
			(*i)->AddRef();
			(*i)->Contained();	
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
SQI_Object *VectorObject::DeepClone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
	
	SQI_Object *obj;
	vector<SQI_Object *>::const_iterator i;
	VectorObject *clone = new VectorObject(hp,0);

	for(i=Vector->begin();i!=Vector->end();i++)
		if(*i)
		{
			obj = (*i)->DeepClone(hp);
			clone->Vector->push_back(obj);
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
string *VectorObject::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "VECTOR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
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
SQI_Object *VectorObject::Neg()
{ 
	vector<SQI_Object *>::const_iterator i;
	VectorObject *neg = new VectorObject(heap,0);
	SQI_Object *obj;

	for(i=Vector->begin();i!=Vector->end();i++)
	{
		if(*i)
		{
			obj = (*i)->Neg();
			neg->Vector->push_back(obj);
			obj->Export(heap,SQI_NORMAL);			
			obj->AddRef();
			obj->Contained();	
		}	
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
SQI_Object *VectorObject::Inv()
{ 
	vector<SQI_Object *>::const_iterator i;
	VectorObject *neg = new VectorObject(heap,0);
	SQI_Object *obj;

	for(i=Vector->begin();i!=Vector->end();i++)
	{
		if(*i)
		{
			obj = (*i)->Inv();
			neg->Vector->push_back(obj);
			obj->Export(heap,SQI_NORMAL);			
			obj->AddRef();
			obj->Contained();	
		}	
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
bool VectorObject::operator==(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
bool VectorObject::operator>(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
bool VectorObject::operator>=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
bool VectorObject::operator<(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
bool VectorObject::operator<=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
bool VectorObject::operator!=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				vector<SQI_Object *>::const_iterator i,j;
				SQI_Object *obj1,*obj2;	
				for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
				{
					obj1 = *i;
					obj2 = *j;
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
SQI_Object *VectorObject::operator+(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap,0);
				
				try
				{					
					SQI_Object *obj;
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{
						if(*i && *j)
						{
							obj = *(*i) + *j;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();
						}
						else
						if(*i)
						{
							obj = *i;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();						
						}
						else
						if(*j)
						{
							obj = *j;
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector + vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"vector + object","unsupported by the array"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap,0);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = *(*i) + op2;
					result->Vector->push_back(obj);
					obj->AddRef();
					obj->Contained();
				}
				else
				{
					result->Vector->push_back(op2);
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
SQI_Object *VectorObject::operator-(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap,0);
				
				try
				{			
					SQI_Object *obj;		
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{
						if(*i && *j)
						{
							obj = *(*i) - *j;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();
						}
						else
						if(*i)
						{
							obj = *i;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();					
						}
						else
						if(*j)
						{
							obj = (*j)->Neg();
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector - vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"vector - vector","unsupported by the array"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap,0);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = *(*i) - op2;
					result->Vector->push_back(obj);
					obj->AddRef();
					obj->Contained();
				}
				else
				{
					obj = op2->Neg();
					result->Vector->push_back(obj);
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
SQI_Object *VectorObject::operator*(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap);
				
				try
				{					
					SQI_Object *obj;		
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{
						if(*i && *j)
						{
							obj = *(*i) * *j;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();
						}
						else
						{
							obj = new SQI_Number(heap,0);
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector * vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"vector * object","unsupported by the vector"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap,0);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = *(*i) * op2;
					result->Vector->push_back(obj);
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
SQI_Object *VectorObject::operator/(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap,0);
				
				try
				{	
					SQI_Object *obj;		
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{
						if(*i && *j)
						{
							obj =  *(*i) / *j;
							result->Vector->push_back(obj);
							obj->AddRef();
							obj->Contained();
						}
						else
						{
							obj = new SQI_Number(heap,0);
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector / vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"vector / object","unsupported by the vector"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap,0);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = *(*i) / op2;
					result->Vector->push_back(obj);
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
SQI_Object *VectorObject::Div(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap,0);
				
				try
				{			
					SQI_Object *obj;		
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{		
						if(*i && *j)
						{
							obj = (*i)->Div(*j);
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector // vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"vector // object","unsupported by the array"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap,0);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = (*i)->Div(op2);
					result->Vector->push_back(obj);
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
SQI_Object *VectorObject::InvDiv(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_VECTOR)
		{
			VectorObject *a = dynamic_cast<VectorObject *>(right);
			if(Vector->size()==a->Vector->size())
			{
				VectorObject *result = new VectorObject(heap,0);
				
				try
				{					
					SQI_Object *obj;		
					vector<SQI_Object *>::const_iterator i,j;
					for(i=Vector->begin(),j=a->Vector->begin();i!=Vector->end();i++,j++)
					{		
						if(*i && *j)
						{
							obj = (*i)->InvDiv(*j);
							result->Vector->push_back(obj);
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
				throw (new SQI_Exception(SQI_EX_INVALIDE,"vector // vector","not of the same size"));
		}
		else
			throw (new SQI_Exception(SQI_EX_INVALIDE,"object // array","unsupported by the array"));
	}
	else
	{
		VectorObject *result = new VectorObject(heap);
		
		try
		{
			SQI_Object *obj;
			vector<SQI_Object *>::const_iterator i;
			for(i=Vector->begin();i!=Vector->end();i++)
			{
				if(*i)
				{
					obj = op2->InvDiv(*i);
					result->Vector->push_back(obj);
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
}

// creation functions and init function

SQI_ThePointer *NewVectorObject(SQI_Squirrel *squirrel,long capacity,SQI_Object *init)
{
	VectorObject *vector = new VectorObject(squirrel,capacity,init);	
	return vector;
}

SQI_ThePointer *NewVectorObject(SQI_Squirrel *squirrel,SQI_List *init)
{
	VectorObject *vector = new VectorObject(squirrel,init);	
	return vector;
}

// Fill up the member map
void InitVectorObject()
{		
	VectorMethods = new MethodTable();

	VectorMethods->Add("get",(OMeth)(&VectorObject::Get));
	VectorMethods->Add("set",(OMeth)(&VectorObject::Set));
	VectorMethods->Add("append",(OMeth)(&VectorObject::Append));
	VectorMethods->Add("size",(OMeth)(&VectorObject::Size));
	VectorMethods->Add("min",(OMeth)(&VectorObject::Min));
	VectorMethods->Add("max",(OMeth)(&VectorObject::Max));
	VectorMethods->Add("av",(OMeth)(&VectorObject::Avg));
	VectorMethods->Add("iterate",(OMeth)(&VectorObject::Iterate));
	VectorMethods->Add("iterate.i",(OMeth)(&VectorObject::IterateWithIndex));
	VectorMethods->Add("sort",(OMeth)(&VectorObject::Sort));
	VectorMethods->Add("sort.new",(OMeth)(&VectorObject::SortNew));
	VectorMethods->Add("reverse",(OMeth)(&VectorObject::Reverse));
	VectorMethods->Add("reverse.new",(OMeth)(&VectorObject::ReverseNew));		
	VectorMethods->Add("find",(OMeth)(&VectorObject::Find));
	VectorMethods->Add("find.all",(OMeth)(&VectorObject::FindAll));
	VectorMethods->Add("find.last",(OMeth)(&VectorObject::FindLast));
	VectorMethods->Add("find.if",(OMeth)(&VectorObject::FindIf));
	VectorMethods->Add("find.if.all",(OMeth)(&VectorObject::FindIfAll));	
	VectorMethods->Add("find.if.last",(OMeth)(&VectorObject::FindIfLast));	
	VectorMethods->Add("erase",(OMeth)(&VectorObject::Erase));	
	
	/*
	VectorObjectMember[string("get")] = &VectorObject::Get;
	VectorObjectMember[string("set")] = &VectorObject::Set;
	VectorObjectMember[string("append")] = &VectorObject::Append;
	VectorObjectMember[string("size")] = &VectorObject::Size;
	VectorObjectMember[string("min")] = &VectorObject::Min;
	VectorObjectMember[string("max")] = &VectorObject::Max;
	VectorObjectMember[string("av")] = &VectorObject::Avg;
	VectorObjectMember[string("iterate")] = &VectorObject::Iterate;
	VectorObjectMember[string("iterate.i")] = &VectorObject::IterateWithIndex;
	VectorObjectMember[string("sort")] = &VectorObject::Sort;
	VectorObjectMember[string("sort.new")] = &VectorObject::SortNew;
	VectorObjectMember[string("reverse")] = &VectorObject::Reverse;
	VectorObjectMember[string("reverse.new")] = &VectorObject::ReverseNew;		
	VectorObjectMember[string("find")] = &VectorObject::Find;
	VectorObjectMember[string("find.all")] = &VectorObject::FindAll;
	VectorObjectMember[string("find.last")] = &VectorObject::FindLast;
	VectorObjectMember[string("find.if")] = &VectorObject::FindIf;
	VectorObjectMember[string("find.if.all")] = &VectorObject::FindIfAll;	
	VectorObjectMember[string("find.if.last")] = &VectorObject::FindIfLast;	
	VectorObjectMember[string("erase")] = &VectorObject::Erase;	
	*/	
}

void UninitVectorObject()
{
	if(VectorMethods)
		delete VectorMethods;	
}
