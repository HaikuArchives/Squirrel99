/*
 * Squirrel project
 *
 * file ?	SQI-node.cpp
 * what	?	Node class 
 * who	?	jlv
 * when	?	07/20/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_Node class

/*
 * function    : SQI_Node
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h, heap to use
 * char t, node type
 *
 * output      : none
 * side effect : none
 */
SQI_Node::SQI_Node(SQI_Heap *h,char t)
  : SQI_Object(h,SQI_NODE)
{
  args = NULL;
  stype = t;
}

/*
 * function    : ~SQI_Node
 * purpose     : destroy the Node
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Node::~SQI_Node()
{	
  	if(args)
      delete args; 
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : the object  may be deleted
 */
bool SQI_Node::Suicidal(bool force=false)
{
    list<SQI_Object *>::const_iterator i;
    SQI_Object *obj;
   
   	if(status || force)
   	{ 
   		if(!REF)
   		{
    		if(args)
    			for(i=args->begin();i!=args->end();i++)
    			{
					(*i)->Alone();
					(*i)->RemRef(force);
    			}
    			
		delete this;
		return true;
   		}
   		else
   			return false;
   	}
   	
   	return false;
}

/*
 * function    : Add2Args
 * purpose     : Add an object to the args list
 * input       :
 *
 * SQI_Object *, object to add
 *
 * output      : none
 * side effect : none
 */
void SQI_Node::Add2Args(SQI_Object *obj)
{
  if(!args)
    args = new list<SQI_Object *>;

  args->push_back(obj);
  obj->AddRef();
  obj->Contained();
}

/*
 * function    : SetArgs
 * purpose     : Set the arg list
 * input       :
 *
 * list<SQI_Object *> *alist, the arg list
 *
 * output      : none
 * side effect : none
 */
void SQI_Node::SetArgs(list<SQI_Object *> *alist)
{
	// code bellow not used anymore, it was causing problem with
	// widget hook running on thread and for the extact same
	// function. Deletion of the args is done where it need to be	
	
	/*
  if(args)
  {
 	list<SQI_Object *>::const_iterator i;
	for(i=args->begin();i!=args->end();i++)
	{
		(*i)->Alone();
		(*i)->RemRef();	
	} 	
    //delete args;
  }
  */

  args = alist;
  
  if(args)
  {
	  list<SQI_Object *>::const_iterator i;
	  for(i=args->begin();i!=args->end();i++)
	  {
		//(*i)->Contained();
		//(*i)->AddRef();
	  } 
  }
}

/*
 * function    : DelArgs
 * purpose     : Delete the arg list
 * input       :
 *
 * list<SQI_Object *> *alist, the arg list
 *
 * output      : none
 * side effect : none
 */
void SQI_Node::DelArgs()
{
  if(args)
  {
 	list<SQI_Object *>::const_iterator i;
	for(i=args->begin();i!=args->end();i++)
		(*i)->RemRef();	 
  }

  args = NULL;
}

/*
 * function    : EmptyArgs
 * purpose     : Set args to NULL
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Node::EmptyArgs()
{
  args = NULL;
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
void SQI_Node::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
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
    }
}
