/*
 * Squirrel project
 *
 * file ?	SQI-heap.cpp
 * what	?	heap class implementation
 * who	?	jlv
 * when	?	07/02/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <stdio.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// we create a memory exception
SQI_Exception HeapEx(SQI_EX_HEAP,"Heap allocation","Failure");

/*
 * function    : SQI_Heap
 * purpose     : initialise a heap
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Heap::SQI_Heap(char otype = SQI_NORMAL)
{
  size = 0;
  counter = 0;
  status = otype;
  top = NULL;
  bottom = NULL;
}

/*
 * function    : ~SQI_Heap
 * purpose     : destroy a heap
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Heap::~SQI_Heap()
{   	 	
	while(top)
		delete top;	
}

/*
 * function    : GC
 * purpose     : Performe a garbage collection
 * input       : none
 * output      : long, number of object deleted
 * side effect : none
 */
long SQI_Heap::GC()
{
	Lock();
	SQI_Object *current = top;
	SQI_Object *previous = top;
	long number = size;	
	
	// first we delete all the object which are not contained

	current = top;
	previous = top;
	
	while(current)
	{
		if(current->IsAlone())
		{			
		  if(current==top)
		    {
		      if(current->Suicidal(true))
				current = top;	
		      else
				{
			  		previous = current;
			  		current = current->next;
				}
		    }
		  else
		    {
		      if(current->Suicidal(true))
				current = previous->next;	
		      else
				{
			  		previous = current;
			  		current = current->next;
				}
		    }
		}
		else
			current = current->next;			
	}	
				
	// now we delete all the rest	
		
	current = top;
	previous = top;
	
	while(current)
	{
	  if(current==top)
	    {
	      if(current->Suicidal(true))
			current = top;	
	      else
			{
		  		previous = current;
		  		current = current->next;
			}
	    }
	  else
	    {
	      if(current->Suicidal(true))
			current = previous->next;	
	      else
			{
		  		previous = current;
		  		current = current->next;
			}
	    }
	}
			
	Unlock();
	
	return number - size;	
} 
 
/*
 * function    : AddObject
 * purpose     : add a new object to the list
 * input       :
 *
 * SQI_Object *last,    the object to add
 *
 * output      : none
 * side effect : none
 */
long SQI_Heap::AddObject(SQI_Object *last)
{
  if(!last)
    throw(&HeapEx);
      
  last->heap = this;
  last->status = status; // set the status of the object
  
  Lock();

  if(!bottom)
    {
      bottom = last;
      top = last;
    }
  else
    {
      bottom->next = last;
      last->previous = bottom;
      bottom = last;
    }
  
  size++;
  
  Unlock();
  
  return ++counter;
}

/*
 * function    : Number
 * purpose     : allocate a new bool object
 * input       :
 *
 * bool value,    initial value
 *
 * output      : SQI_Number * , pointer to the object
 * side effect : none
 *
SQI_Number *SQI_Heap::Number(bool value)
{
  SQI_Number *last = new SQI_Number(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Number
 * purpose     : allocate a new integer object
 * input       :
 *
 * int value,    initial value
 *
 * output      : SQI_Number * , pointer to the object
 * side effect : none
 *
SQI_Number *SQI_Heap::Number(int value)
{
  SQI_Number *last = new SQI_Number(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Number
 * purpose     : allocate a new long object
 * input       :
 *
 * long value,    initial value
 *
 * output      : SQI_Number * , pointer to the object
 * side effect : none
 *
SQI_Number *SQI_Heap::Number(long value)
{
  SQI_Number *last = new SQI_Number(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Number
 * purpose     : allocate a new float object
 * input       :
 *
 * float value,    initial value
 *
 * output      : SQI_Number * , pointer to the object
 * side effect : none
 *
SQI_Number *SQI_Heap::Number(float value)
{
  SQI_Number *last = new SQI_Number(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Number
 * purpose     : allocate a new double object
 * input       :
 *
 * double value,    initial value
 *
 * output      : SQI_Number * , pointer to the object
 * side effect : none
 *
SQI_Number *SQI_Heap::Number(double value)
{
  SQI_Number *last = new SQI_Number(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : String
 * purpose     : allocate a new string object
 * input       :
 *
 * char *value,    initial value
 *
 * output      : SQI_String * , pointer to the object
 * side effect : none
 *
SQI_String *SQI_Heap::String(char *value)
{
  SQI_String *last = new SQI_String(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : String
 * purpose     : allocate a new string object
 * input       :
 *
 * string value,    initial value
 *
 * output      : SQI_String * , pointer to the object
 * side effect : none
 *
SQI_String *SQI_Heap::String(string value)
{
  SQI_String *last = new SQI_String(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : String
 * purpose     : allocate a new string object
 * input       :
 *
 * SQI_String *value,    initial value
 *
 * output      : SQI_String * , pointer to the object
 * side effect : none
 *
SQI_String *SQI_Heap::String(SQI_String *value)
{
  SQI_String *last = new SQI_String(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Keyword
 * purpose     : allocate a new keyword object
 * input       :
 *
 * char *value,    initial value
 *
 * output      : SQI_Keyword * , pointer to the object
 * side effect : none
 *
SQI_Keyword *SQI_Heap::Keyword(char *value)
{
  SQI_Keyword *last = new SQI_Keyword(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Keyword
 * purpose     : allocate a new string object
 * input       :
 *
 * string *value,    initial value
 *
 * output      : SQI_String * , pointer to the object
 * side effect : none
 *
SQI_Keyword *SQI_Heap::Keyword(string *value)
{
  SQI_Keyword *last = new SQI_Keyword(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : Keyword
 * purpose     : allocate a new keyword object
 * input       :
 *
 * SQI_Keyword *value,    initial value
 *
 * output      : SQI_Keyword * , pointer to the object
 * side effect : none
 *
SQI_Keyword *SQI_Heap::Keyword(SQI_Keyword *value)
{
  SQI_Keyword *last = new SQI_Keyword(++counter,value);

  AddObject(last);

  return last;
}

/*
 * function    : List
 * purpose     : allocate a new list object
 * input       : none
 * output      : SQI_List * , pointer to the object
 * side effect : none
 *
SQI_List *SQI_Heap::List()
{
  SQI_List *last = new SQI_List(++counter);

  AddObject(last);

  return last;
}

/*
 * function    : Block
 * purpose     : allocate a new block node object
 * input       : none
 * output      : SQI_nBlock * , pointer to the object
 * side effect : none
 *
SQI_nBlock *SQI_Heap::Block()
{
  SQI_nBlock *last = new SQI_nBlock(++counter);

  AddObject(last);

  return last;
}

/*
 * function    : Builtin
 * purpose     : allocate a new builtin node object
 * input       : none
 * output      : SQI_nBuiltin * , pointer to the object
 * side effect : none
 *
SQI_nBuiltin *SQI_Heap::Builtin(char *str)
{
  SQI_nBuiltin *last = new SQI_nBuiltin(++counter,str);

  AddObject(last);

  return last;
}

/*
 * function    : Expression
 * purpose     : allocate a new expr node object
 * input       : 
 *
 * char type, type of the expression
 * SQI_Object *obj, first argument
 *
 * output      : SQI_nExpr * , pointer to the object
 * side effect : none
 *
SQI_nExpr *SQI_Heap::Expression(char type,SQI_Object *obj)
{
  SQI_nExpr *last = new SQI_nExpr(++counter,type,obj);

  AddObject(last);

  return last;
}

/*
 * function    : Unknow
 * purpose     : allocate a new unknow node object
 * input       : 
 *
 * string *name, name of the function
 *
 * output      : SQI_nUfunc * , pointer to the object
 * side effect : none
 *
SQI_nUnknow *SQI_Heap::Unknow(string *name)
{
  SQI_nUnknow *last = new SQI_nUnknow(++counter,name);

  AddObject(last);

  return last;
}

/*
 * function    : Ufunc
 * purpose     : allocate a new ufunc node object
 * input       : 
 *
 * string *name, name of the function
 *
 * output      : SQI_nUfunc * , pointer to the object
 * side effect : none
 *
SQI_nUfunc *SQI_Heap::Ufunc(string *name)
{
  SQI_nUfunc *last = new SQI_nUfunc(++counter,name);

  AddObject(last);

  return last;
}

/*
 * function    : Ufunc
 * purpose     : allocate a new ufunc node object
 * input       : 
 *
 * string *name, name of the function
 * SQI_Defunc *ptr, ptr of the function
 *
 * output      : SQI_nUfunc * , pointer to the object
 * side effect : none
 *
SQI_nUfunc *SQI_Heap::Ufunc(string *name,SQI_Defunc *ptr)
{
  SQI_nUfunc *last = new SQI_nUfunc(++counter,name,ptr);

  AddObject(last);

  return last;
}

/*
 * function    : Control
 * purpose     : allocate a new control node object
 * input       :
 *
 * char type, type of the control
 *
 * output      : SQI_nControl * , pointer to the object
 * side effect : none
 *
SQI_nControl *SQI_Heap::Control(char type)
{
  SQI_nControl *last = new SQI_nControl(++counter,type);

  AddObject(last);

  return last;
}

/*
 * function    : Varref
 * purpose     : allocate a new varref node object
 * input       : none
 * output      : SQI_nVarref * , pointer to the object
 * side effect : none
 *
SQI_nVarref *SQI_Heap::Varref(SQI_Object *obj)
{
  SQI_nVarref *last = new SQI_nVarref(++counter,dynamic_cast<SQI_Keyword *>(obj));

  AddObject(last);

  return last;
}

/*
 * function    : Tag
 * purpose     : allocate a new tag node object
 * input       :
 *
 * SQI_String *obj, the tag
 *
 * output      : SQI_nTag * , pointer to the object
 * side effect : none
 *
SQI_nTag *SQI_Heap::Tag(SQI_String *obj)
{
  SQI_nTag *last = new SQI_nTag(++counter,obj);

  AddObject(last);

  return last;
}

/*
 * function    : Goto
 * purpose     : allocate a new goto node object
 * input       :
 *
 * SQI_Object *obj, the tap
 *
 * output      : SQI_nGoto * , pointer to the object
 * side effect : none
 *
SQI_nGoto *SQI_Heap::Goto(SQI_Object *obj)
{
  SQI_nGoto *last = new SQI_nGoto(++counter,obj);

  AddObject(last);

  return last;
}

/*
 * function    : Switch
 * purpose     : allocate a new switch node object
 * input       :
 *
 * SQI_Object *obj, the tap
 *
 * output      : SQI_nSwitch * , pointer to the object
 * side effect : none
 *
SQI_nSwitch *SQI_Heap::Switch(SQI_Object *obj)
{
  SQI_nSwitch *last = new SQI_nSwitch(++counter,obj);

  AddObject(last);

  return last;
}

/*
 * function    : ThePointer
 * purpose     : add a new pointer object to the heap
 * input       :
 *
 * SQI_ThePointer *ptr, object created to be added
 *
 * output      : none
 * side effect : none
 *
void SQI_Heap::ThePointer(SQI_ThePointer *ptr)
{ 
  ptr->ID = ++counter;
  ptr->heap = this;
  AddObject(ptr);
}

/*
 * function    : Member
 * purpose     : allocate a new member node object
 * input       : none
 * output      : SQI_nMember * , pointer to the object
 * side effect : none
 *
SQI_nMember *SQI_Heap::Member(SQI_Object *obj,char *member)
{
  	SQI_nMember *last = new SQI_nMember(++counter,dynamic_cast<SQI_Keyword *>(obj));

	last->SetMember(member);	

  	AddObject(last);

  	return last;
}

*/

/*
 * function    : Free
 * purpose     : Remove the object from the list
 * input       :
 *
 * SQI_Object *, the object to remove
 *
 * output      : none
 * side effect : none
 */
void SQI_Heap::Free(SQI_Object *obj)
{
	Lock();
	size--;
	    
	if(obj==top)
	{
		if(top==bottom)
		{
	 	  	top = NULL;
	  		bottom = NULL;
		}
		 else
		{
 	  		top = top->next;
 	  		top->previous = NULL;	  
		}
	}
	else
	if(obj==bottom)
	{
		bottom = obj->previous;
		bottom->next = NULL;
	}
	else
	{
		obj->previous->next = obj->next;
		obj->next->previous = obj->previous;
	}

	obj->previous = NULL;
	obj->next = NULL;	

	Unlock();
}

/*
 * function    : Stats
 * purpose     : Return a string of statistical infos on the heap
 * input       :
 * output      : string *, string of infos
 * side effect : none
 */
string *SQI_Heap::Stats()
{
  ostrstream out;	
	
  out << "Size = " << size << " Last ID = "<< counter << '\0';

  string *ret = new string(out.str());

  out.freeze(false);

  return ret;
}

/*
 * function    : Display
 * purpose     : Print the content of the heap on the stdout
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Heap::Display()
{
  SQI_Object *current = top;
  cout << "\nHeap content : \n";
  string *str;

  while(current)
    {
      str = current->Dump();
      cout << *str << "\n";
      delete str;
      current = current->next;
    }
}

/*
 * function    : Dump
 * purpose     : Dump all the object in the heap
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_Heap::Dump()
{
  SQI_Object *current = top;
  string *out = new string;
  string *str;

  while(current)
    {
      str = current->Dump();
      //cout << *str << "\n";
      *out += *str;
      delete str;
      *out += "\n";	
      current = current->next;
    }

  return out;
}

/*
 * function    : Import
 * purpose     : Import in the heap an object from another heap
 * input       : 
 *
 * SQI_Object *obj, the object to import
 *
 * output      : none
 * side effect : none
 */
void SQI_Heap::Import(SQI_Object *obj)
{
  // first we remove the object from his heap
  obj->heap->Free(obj);
  // and now we put it in this one
  AddObject(obj);
  obj->ID = ++counter;
}

/*
 * function    : FootPrint
 * purpose     : Return the memory used by the Heap (and the content of the heap also)
 * input       : none
 * output      : long
 * side effect : none
 */
long SQI_Heap::FootPrint()
{
  long s = sizeof(SQI_Heap);

  SQI_Object *current = top;
 
  while(current)
    {
      s += current->FootPrint();
      current = current->next;
    }

  return s;
}
