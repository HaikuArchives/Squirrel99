/*
 * Squirrel project
 *
 * file ?	SQI-pointer.cpp
 * what	?	Pointer class 
 * who	?	jlv
 * when	?	09/05/99
 * last	?	09/09/99
 *
 *
 * (c) electron Technology 1999
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_Pointer members functions

/*
 * function    : SQI_Pointer
 * purpose     : initialise the pointer
 * input       :
 *
 * long id, object id
 * char itype, internal type
 *
 * output      : none
 * side effect : none
 */
SQI_Pointer::SQI_Pointer(long id,char itype)
  : SQI_Object(id,SQI_POINTER)
{
	stype = itype;
  	data = NULL;
  	DeleteHook = NULL;
}

/*
 * function    : ~SQI_Pointer
 * purpose     : destroy a pointer
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Pointer::~SQI_Pointer()
{
	// we call the DeleteHook if we have one
	if(DeleteHook)
		(*DeleteHook)(data);
			
  	heap->Free(this);
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Pointer::Print(int prec = 3)
{
  	ostrstream out;
  
	out << data << '\0';
	
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
string *SQI_Pointer::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Pointer::Dump()
{ 
  string *value = this->Print();
  ostrstream out;
 
  out << "PTR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;
  
  string *ret = new string(out.str());
  out.freeze(false);

  return ret;
}

/*
 * function    : operator=
 * purpose     : Store an object in another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_Pointer::operator=(SQI_Object *op2)
{
  if(TYPE!=op2->TYPE)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"Pointer = Object"));
    }
  else
    {
      SQI_Pointer *obj = dynamic_cast<SQI_Pointer *>(op2);
      if(stype==obj->stype)
		data = obj->data;
    }

  return this;
}

/*
 * function    : operator==
 * purpose     : Return true if the values are equal
 * input       : none
 *
 * SQI_Object *op2, the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_Pointer::operator==(SQI_Object *op2)
{
  if(TYPE!=op2->TYPE)
      return false;
  else
    {
      SQI_Pointer *obj = dynamic_cast<SQI_Pointer *>(op2);
      
		return (data == obj->data);
    }
}

/*
 * function    : operator!=
 * purpose     : Return true if the values are non equal
 * input       : none
 *
 * SQI_Object *op2, the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_Pointer::operator!=(SQI_Object *op2)
{
  if(TYPE!=op2->TYPE)
      return false;
  else
    {
      SQI_Pointer *obj = dynamic_cast<SQI_Pointer *>(op2);
      
		return (data != obj->data);
    }
}



