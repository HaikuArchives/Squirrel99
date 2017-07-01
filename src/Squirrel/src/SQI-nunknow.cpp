/*
 * Squirrel project
 *
 * file ?	SQI-nunknow.cpp
 * what	?	Unknow Node class 
 * who	?	jlv
 * when	?	09/28/00
 * last	?	03/29/00
 *
 *
 * (c) Kirilla 2000-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_nUnknow class

/*
 * function    : SQI_nUnknow
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h,	heap to use	
 * string *n, 	function name
 *
 * output      : none
 * side effect : none
 */
SQI_nUnknow::SQI_nUnknow(SQI_Heap *h,string *n)
  : SQI_Node(h,SQI_TNODE_UNKNOW)
{
  name = new string(n->c_str());
  func = NULL;
  function = NULL;
  
}

/*
 * function    : ~SQI_nUnknow
 * purpose     : destroy the Node
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_nUnknow::~SQI_nUnknow()
{
	delete name;
}

/*
 * function    : Print
 * purpose     : return a string* of the Node
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_nUnknow::Print(int prec = 3)
{
  string *out,*tmp,*cur;

  if(!args)
    out = new string("{unknow}[]");
  else
    {
      out = new string("{unknow}[");
      list<SQI_Object *>::const_iterator i;
	          
      for(i=args->begin();i!=args->end();i++)
		{
		  cur = (*i)->Print(prec);
	      *out += *cur;
	      *out += " ";
		  delete cur;
		}

	(*out)[out->length()-1] = ']';
    }
 
  return out;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : sting *
 * side effect : none
 */
string *SQI_nUnknow::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "UNK[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;
  
  string *ret = new string(out.str());
  out.freeze(false);

  return ret;
}
