/*
 * Squirrel project
 *
 * file ?	SQI-string.cpp
 * what	?	String class 
 * who	?	jlv
 * when	?	06/22/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_String class

/*
 * function    : SQI_String
 * purpose     : initialise the string
 * input       :
 *
 * SQI_Heap *h,	heap to use	
 * char *value, the value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_String::SQI_String(SQI_Heap *h,char *value)
  : SQI_Object(h,SQI_STRING)
{
  	data = value;
}


/*
 * function    : SQI_String
 * purpose     : initialise the string
 * input       :
 *
 * SQI_Heap *h,	heap to use
 * string value, the value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_String::SQI_String(SQI_Heap *h,string value)
  : SQI_Object(h,SQI_STRING)
{
  data = value.c_str();
}

/*
 * function    : SQI_String
 * purpose     : initialise the string
 * input       :
 *
 * SQI_Heap *h,	heap to use
 * SQI_String *str, the object value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_String::SQI_String(SQI_Heap *h,SQI_String *str)
  : SQI_Object(h,SQI_STRING)
{
  data = str->data.c_str();
}

/*
 * function    : Archive
 * purpose     : Store the string in an message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_String::Archive(BMessage *archive)
{	
	// first we set the type if the object
	archive->AddInt8("SQI_Object",SQI_STRING);
	archive->AddString("data",data.c_str());	
  	
  	return B_OK;
}

/*
 * function    : Instantiate
 * purpose     : Fill the string with the value stored in the archived message
 * input       : 
 *
 * SQI_Heap *target, heap to be use
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_String::Instantiate(SQI_Heap *target,BMessage *archive)
{
	const char *str;
	
	archive->FindString("data",&str);
	
	data = str;		
	
	return B_OK;
}

/*
 * function    : Print
 * purpose     : return a string* of the String
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_String::Print(int prec=3)
{
  return new string(data);
}

/*
 * function    : Show
 * purpose     : return a char* of the String
 * input       : none
 * output      : char *
 * side effect : none
 */
string *SQI_String::Show(int prec=3)
{
  	string *str = new string;
  
	*str += "'";
	*str += data;
	*str += "'";	 
  
  	return str;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_String::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "STR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
  delete value;
  
  string *ret = new string(out.str());
  out.freeze(false);

  return ret;
}

/*
 * function    : operator=
 * purpose     : affect a string object to another
 * input       :
 *
 * SQI_Object *, pointer to the object
 *
 * output      : SQI_Object *, the object
 * side effect : none
 */
SQI_Object *SQI_String::operator=(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
  		data = (dynamic_cast<SQI_String *>(op2))->data;
    }
  else
    throw (new SQI_Exception(SQI_EX_BADTYPE,"String = Object"));

  return this;
}

/*
 * function    : operator+
 * purpose     : Create a string from the concatenation of 2 strings
 * input       :
 *
 * SQI_Object *, pointer to the object to use
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_String::operator+(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      
	  string str = data + obj->data;	
      
      return new SQI_String(heap,str);
    }
  else
    throw (new SQI_Exception(SQI_EX_BADTYPE,"String + Object")); 
}

/*
 * function    : operator-
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the object to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_String::operator-(SQI_Object *op2)
{
  throw (new SQI_Exception(SQI_EX_INVALIDE,"String - Object")); 
  return this;
}

/*
 * function    : operator*
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the object to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_String::operator*(SQI_Object *op2)
{
  throw (new SQI_Exception(SQI_EX_INVALIDE,"String * Object"));
  return this;
}

/*
 * function    : operator/
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the object to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_String::operator/(SQI_Object *op2)
{
  throw (new SQI_Exception(SQI_EX_INVALIDE,"String / Object"));
  return this;
}

/*
 * function    : operator%
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the object to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_String::operator%(SQI_Object *op2)
{
  throw (new SQI_Exception(SQI_EX_INVALIDE,"String % Object"));
  return this;
}


/*
 * function    : operator==
 * purpose     : return true if 2 string are equal
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator==(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      return (data == obj->data);
    }
  else
    return false;
}

/*
 * function    : operator!=
 * purpose     : return true if 2 string are non equal
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator!=(SQI_Object *op2)
{
   if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      return (data != obj->data);
    }
   else
     return true;
}

/*
 * function    : operator>
 * purpose     : return true if the string is > than the other
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator>(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      return (data>obj->data);
    }
  else
    return false;
}

/*
 * function    : operator<
 * purpose     : return true if the string is < than the othe
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator<(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      return (data<obj->data);
    }
  else
    return false;
}

/*
 * function    : operator>=
 * purpose     : return true if the string is >= than the other
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator>=(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
     return (data>=obj->data); 
    }
  else
    return false;
}

/*
 * function    : operator<=
 * purpose     : return true if the string is <= than the othe
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_String::operator<=(SQI_Object *op2)
{
  if(op2->TYPE==SQI_STRING||op2->TYPE==SQI_KEYWORD)
    {
      SQI_String *obj = dynamic_cast<SQI_String *>(op2);
      return (data<=obj->data);
    }
  else
    return false;
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
SQI_Object *SQI_String::Clone(SQI_Heap *target = NULL)
{
  if(!target)
    target = heap;

  return new SQI_String(target,data);
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
SQI_Object *SQI_String::DeepClone(SQI_Heap *target = NULL)
{
  return Clone(target);
}

/*
 * function    : InstantiateAString
 * purpose     : Instantiate a string from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateAString(SQI_Heap *target,BMessage *archive)
{
	SQI_String *str = new SQI_String(target,"");
	
	if(str->Instantiate(target,archive)==B_OK)
		return str;
	else
	{
		str->Suicidal(true);
		return NULL;	
	}	
}




