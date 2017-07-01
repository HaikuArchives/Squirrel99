/*
 * Squirrel project
 *
 * file ?	SQI-nindex.cpp
 * what	?	Member Node class 
 * who	?	jlv
 * when	?	10/18/99
 * last	?	10/18/99
 *
 *
 * (c) electron Technology 1999
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_nIndex class

/*
 * function    : SQI_nIndex
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * long id, id of the object
 * char *ref, name of the object
 * list<SQI_Object *> *obj, list of index object
 *
 * output      : none
 * side effect : none
 */
SQI_nIndex::SQI_nIndex(long id,char *ref,list<SQI_Object *> *obj)
  : SQI_Node(id,SQI_TNODE_INDEX)
{
	lindex = NULL;
	index = NULL;	
	
	if(obj->length()>1)
		lindex = obj;		
	else
	{
		index = *(obj->begin())
  		delete obj;
	}
	
  	Name = ref;
}

/*
 * function    : ~SQI_nIndex
 * purpose     : destroy the Node
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_nIndex::~SQI_nIndex()
{
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : the object  may be deleted
 */
bool SQI_nIndex::Suicidal(bool force=false)
{
	if(status || force)
		if(!REF)
		{
  			index->RemRef(force);
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
string *SQI_nIndex::Print(int prec = 3)
{
  	string *out,*tmp,*cur;

  	out = new string("{index}[");
  
  	cur = index->Print(prec);

	*out += *cur;
	*out += "]";

	delete cur;	
  	return out;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nIndex::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "IDX[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
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
void SQI_nIndex::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
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
    	
    	index->Export(nheap,force);
    }
}

/*
 * function    : GetIndex
 * purpose     : Get the indexe
 * input       : none
 *
 * SQI_Object **index, index if the index is an object
 * char force, if 1 , the export is forced even on an eternal object
 *
 * output      : none
 * side effect : The object don't exist anymore in his heap
 */
void SQI_nIndex::GetIndex(SQI_Object **pindex,list<SQI_Object *> **plindex)
{
	if(lindex)
		*plindex = lindex;
	else
		*pindex = index;
}


