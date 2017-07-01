/*
 * Squirrel project
 *
 * file ?	SQI-nmember.cpp
 * what	?	Member Node class 
 * who	?	jlv
 * when	?	10/02/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_nMember class

/*
 * function    : SQI_nMember
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h,		heap to use
 * SQI_Keyword *, pointer to the object (string) 
 *
 * output      : none
 * side effect : none
 */
SQI_nMember::SQI_nMember(SQI_Heap *h,SQI_Keyword *obj)
  : SQI_Node(h,SQI_TNODE_MEMBER)
{
  ref = obj;
  obj->AddRef();
  obj->Contained();
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : the object  may be deleted
 */
bool SQI_nMember::Suicidal(bool force=false)
{
	if(status || force)
		if(!REF)
		{
			ref->Alone();
  			ref->RemRef(force);
  			return SQI_Node::Suicidal(force);
		}
		
	return false;
}

/*
 * function    : Print
 * purpose     : return a string* of the Node
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nMember::Print(int prec = 3)
{
  	string *out,*tmp,*cur;

  	out = new string("{member}[");
  
  	cur = ref->Print(prec);
  	
  	*out += *cur;  
 	*out += " ; ";
  	
  	delete cur;	
  	
  	list<SQI_Object *>::const_iterator i;
          
    for(i=args->begin();i!=args->end();i++)
	{
  		cur = (*i)->Print(prec);
		*out += *cur;
		delete cur;
		*out += " ";
	}
	
	*out += "]";	
  	return out;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nMember::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "MBR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;
  
  string *ret = new string(out.str());
  out.freeze(false);

  return ret;
}

/*
 * function    : Export
 * purpose     : Export the object from his heap to another heap
 * input       : none
 *
 * SQI_Heap *nheap, the target heap
 * char force, if 1 , the export is forced even on an eternal object
 *
 * output      : none
 * side effect : The object don't exist anymore in his heap
 */
void SQI_nMember::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
  if(status || force) // Eternal object cannot be Exported
    {      
      // when a node is exported , all the args of the node are exported to

		if(args)
		{
      		list<SQI_Object *>::const_iterator i;
      
     	 	for(i=args->begin();i!=args->end();i++)
				(*i)->Export(nheap,force);

      		nheap->Import(this); // we call the heap method
    	}
    	
    	ref->Export(nheap,force);
    }
}
