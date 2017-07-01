/*
 * Squirrel project
 *
 * file ?	SQI-nufunc.cpp
 * what	?	Ufunc Node class 
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

// SQI_nUfunc class

/*
 * function    : SQI_nUfunc
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h,	heap to use 
 * string *n, 	function name
 *
 * output      : none
 * side effect : none
 */
SQI_nUfunc::SQI_nUfunc(SQI_Heap *h,string *n)
  : SQI_Node(h,SQI_TNODE_USERFUN)
{
  name = new string(n->c_str());
  func = NULL;
}

/*
 * function    : SQI_nUfunc
 * purpose     : initialise the Node of a certain type
 * input       :
 *
 * SQI_Heap *h,	heap to use
 * string *n, function name
 * SQI_Defunc *f, function ptr
 *
 * output      : none
 * side effect : none
 */
SQI_nUfunc::SQI_nUfunc(SQI_Heap *h,string *n,SQI_Defunc *f)
  : SQI_Node(h,SQI_TNODE_USERFUN)
{
  name = new string(*n);
  func = f;
}

/*
 * function    : ~SQI_nUfunc
 * purpose     : destroy the Node
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_nUfunc::~SQI_nUfunc()
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
string *SQI_nUfunc::Print(int prec = 3)
{
  string *out,*tmp,*cur;

  if(!args)
    out = new string("{ufunc}[]");
  else
    {
      out = new string("{ufunc}[");
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
string *SQI_nUfunc::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "UFC[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;
  
  string *ret = new string(out.str());
  out.freeze(false);

  return ret;
}
