/*
 * Squirrel project
 *
 * file ?	SQI-nblock.cpp
 * what	?	Block Node class 
 * who	?	jlv
 * when	?	07/23/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <math.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_nBlock class

/*
 * function    : SQI_nBlock
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h, heap to use
 *
 * output      : none
 * side effect : none
 */
SQI_nBlock::SQI_nBlock(SQI_Heap *h)
  : SQI_Node(h,SQI_TNODE_BLOCK)
{
  instructions = NULL;
}

/*
 * function    : ~SQI_nBlock
 * purpose     : destroy the Node
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_nBlock::~SQI_nBlock()
{
	if(instructions)
	    delete instructions;
}

/*
 * function    : Suicidal
 * purpose     : The object may not be anymore used , so we may delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_nBlock::Suicidal(bool force=false)
{
	if(status || force)
 		if(!REF)
 		{	
			if(instructions)
			{
				list<SQI_Object *>::const_iterator i;
	   			for(i=instructions->begin();i!=instructions->end();i++)
	   			{
	   				(*i)->Alone();
					(*i)->RemRef(force);	
	   			}	
			}
	
			SQI_Node::Suicidal(force);	
			return true;
 		}
 		
 	return false;		
}

/*
 * function    : Print
 * purpose     : return a sting * of the Node
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nBlock::Print(int prec = 3)
{
  string *out,*cur;

  if(!instructions)
    out = new string("\n{block}[]\n");
  else
    {
      if(!instructions->size())
		return new string("\n{block}[]\n");

      out = new string("\n{block}[\n");
      list<SQI_Object *>::const_iterator i;
	          
      for(i=instructions->begin();i!=instructions->end();i++)
		{
		  cur = (*i)->Print(prec);
		  *out += *cur;
		  *out += "\n";
		  delete cur;
		}
    }
 
  return out;
}

/*
 * function    : Show
 * purpose     : return a sting * of the Node
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nBlock::Show(int prec = 3)
{
	return Print(prec);		
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nBlock::Dump()
{  
  string *value = this->Print();
  ostrstream out;
  
  out << "BLK[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;

  string *ret = new string(out.str());
  out.freeze(false);
  
  return ret;
}

/*
 * function    : Add2Inst
 * purpose     : Add an object to the list of instructions
 * input       :
 *
 * SQI_Object *obj, obj to add
 *
 * output      : none
 * side effect : none
 */
void SQI_nBlock::Add2Inst(SQI_Object *obj)
{
	if(!instructions)
		instructions = new list<SQI_Object *>;	
			
  	instructions->push_back(obj);
  	obj->AddRef();
  	obj->Contained();
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
void SQI_nBlock::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
  if(status || force) // Eternal object cannot be Exported
    {      
      // when a node is exported , all the args of the node are exported to

		if(args)
		{
      		list<SQI_Object *>::const_iterator i;
      
     	 	for(i=args->begin();i!=args->end();i++)
				(*i)->Export(nheap,force);
    	}
    	
    	if(instructions)
    	{
      		list<SQI_Object *>::const_iterator i;
      
     	 	for(i=instructions->begin();i!=instructions->end();i++)
				(*i)->Export(nheap,force);   			
    	}
    	
    	nheap->Import(this); // we call the heap method
    }
}


