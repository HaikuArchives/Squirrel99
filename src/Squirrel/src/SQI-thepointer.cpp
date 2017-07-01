/*
 * Squirrel project
 *
 * file ?	SQI-thepointer.cpp
 * what	?	ThePointer class 
 * who	?	jlv
 * when	?	10/01/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-squirrel.h"

/*
 * function    : SQI_ThePointer
 * purpose     : initialise the pointer
 * input       :
 *
 * long id, object id
 * char itype, internal type
 *
 * output      : none
 * side effect : none
 */
SQI_ThePointer::SQI_ThePointer(SQI_Squirrel *squirrel,char itype)
  : SQI_Object(squirrel->LocalHeap,SQI_THEPOINTER)
{
	stype = itype;
	Methods = NULL;
}

/*
 * function    : SQI_ThePointer
 * purpose     : initialise the pointer
 * input       :
 *
 * long id, object id
 * char itype, internal type
 *
 * output      : none
 * side effect : none
 */
SQI_ThePointer::SQI_ThePointer(SQI_Heap *target,char itype)
  : SQI_Object(target,SQI_THEPOINTER)
{
	stype = itype;
}

/*
 * function    : Suicidal
 * purpose     : The object will commit suicide if no reference to it and not eternal
 * input       : none
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
bool SQI_ThePointer::Suicidal(bool force=false)
{
    if(status || force)
      if(!REF)
      {
			delete this;
			return true;
      }
      
      return false;
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
void SQI_ThePointer::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
		nheap->Import(this);	
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_ThePointer::Print(int prec = 3)
{
  	ostrstream out;
  
	out << this << '\0';

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
string *SQI_ThePointer::Show(int prec = 3)
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
string *SQI_ThePointer::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "OBJECT[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

/*
 * function    : Inv
 * purpose     : Inverse the value of the object
 * input       : none
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *SQI_ThePointer::Inv()
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"1 / Object","unsupported by the object"));
}

/*
 * function    : Div
 * purpose     : integer division of a thing by the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::InvDiv(SQI_Object *right)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Thing // Object","unsupported by the object"));
}

/*
 * function    : Neg
 * purpose     : Set the object as negative
 * input       : none
 * output      : SQI_Object *, the result
 * side effect : none
 */
SQI_Object *SQI_ThePointer::Neg()
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"-Object","unsupported by the object"));
}

/*
 * function    : Div
 * purpose     : integer division of the object by something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::Div(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object // Thing","unsupported by the object"));
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
SQI_Object *SQI_ThePointer::operator=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object = Thing","unsupported by the object"));
}

/*
 * function    : operator+
 * purpose     : Add an object to another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator+(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object + Thing","unsupported by the object"));
}

/*
 * function    : operator-
 * purpose     : sub an object to another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator-(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object - Thing","unsupported by the object"));
}

/*
 * function    : operator*
 * purpose     : Multiply an object to another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator*(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object * Thing","unsupported by the object"));
}

/*
 * function    : operator/
 * purpose     : Divide an object to another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator/(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object / Thing","unsupported by the object"));
}

/*
 * function    : operator%
 * purpose     : modulo an object to another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator%(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object % Thing","unsupported by the object"));
}

/*
 * function    : Pow
 * purpose     : pow of an object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::Pow(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object ** Thing","unsupported by the object"));
}

/*
 * function    : operator^
 * purpose     : bitwise exclusive OR of an object with another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator^(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object ^ Thing","unsupported by the object"));
}

/*
 * function    : operator&
 * purpose     : bitwise AND of an object with another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator&(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object & Thing","unsupported by the object"));
}

/*
 * function    : operator|
 * purpose     : bitwise inclusive OR of an object with another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator|(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object | Thing","unsupported by the object"));
}

/*
 * function    : operator<<
 * purpose     : bitwise left shift of an object with another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator<<(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object << Thing","unsupported by the object"));
}

/*
 * function    : operator>>
 * purpose     : bitwise right shift of an object with another
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator>>(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object >> Thing","unsupported by the object"));
}

/*
 * function    : operator+=
 * purpose     : add an object to the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator+=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object += Thing","unsupported by the object"));
}

/*
 * function    : operator-=
 * purpose     : sub an object to the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator-=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object -= Thing","unsupported by the object"));
}

/*
 * function    : operator*=
 * purpose     : mult an object to the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator*=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object *= Thing","unsupported by the object"));
}

/*
 * function    : operator/=
 * purpose     : divide an object to the object
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
SQI_Object *SQI_ThePointer::operator/=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object /= Thing","unsupported by the object"));
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
bool SQI_ThePointer::operator==(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object == Thing","unsupported by the object"));
}

/*
 * function    : operator!=
 * purpose     : test if the object == something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool SQI_ThePointer::operator!=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object != Thing","unsupported by the object"));
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
bool SQI_ThePointer::operator>(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object > Thing","unsupported by the object"));
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
bool SQI_ThePointer::operator<(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object < Thing","unsupported by the object"));
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
bool SQI_ThePointer::operator>=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object >= Thing","unsupported by the object"));
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
bool SQI_ThePointer::operator<=(SQI_Object *op2)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"Object >= Thing","unsupported by the object"));
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
SQI_Object *SQI_ThePointer::Clone(SQI_Heap *target = NULL)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"clone object","unsupported by the object"));	
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
SQI_Object *SQI_ThePointer::DeepClone(SQI_Heap *target = NULL)
{
	throw (new SQI_Exception(SQI_EX_INVALIDE,"deepclone object","unsupported by the object"));
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
SQI_Object *SQI_ThePointer::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{		
	throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unsupported by the object"));
}
