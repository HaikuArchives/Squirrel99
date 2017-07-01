/*
 * Squirrel project
 *
 * file ?	SQI-keyword.cpp
 * what	?	Keyword class 
 * who	?	jlv
 * when	?	07/16/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"

// SQI_Keyword class

/*
 * function    : SQI_Keyword
 * purpose     : initialise the keyword
 * input       :
 *
 * SQI_Heap *h,	heap to use
 * char *value, the value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_Keyword::SQI_Keyword(SQI_Heap *h,char *value)
  : SQI_String(h,value)
{
  TYPE = SQI_KEYWORD;
}

/*
 * function    : SQI_Keyword
 * purpose     : initialise the keyword
 * input       :
 *
 * SQI_Heap *h, 	heap to use	
 * string *value, 	the value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_Keyword::SQI_Keyword(SQI_Heap *h,string *value)
  : SQI_String(h,*value)
{
  TYPE = SQI_KEYWORD;
}

/*
 * function    : SQI_Keyword
 * purpose     : initialise the keyword
 * input       :
 *
 * SQI_Heap *h,		heap to use
 * SQI_Keyword *str, the object value to store in the object
 *
 * output      : none
 * side effect : none
 */
SQI_Keyword::SQI_Keyword(SQI_Heap *h,SQI_Keyword *str)
  : SQI_String(h,str)
{
  TYPE = SQI_KEYWORD;
}

/*
 * function    : Archive
 * purpose     : Store the keyword in an message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_Keyword::Archive(BMessage *archive)
{	
	// first we set the type if the object
	archive->AddInt8("SQI_Object",SQI_KEYWORD);
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
status_t SQI_Keyword::Instantiate(SQI_Heap *target,BMessage *archive)
{
	const char *str;
	
	archive->FindString("data",&str);
	
	data = str;		
	
	return B_OK;
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_Keyword::Dump()
{
  ostrstream out;
  string *value = this->Print();

  out << "KEYWORD[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';

  delete value;

  string *ret = new string(out.str());

  out.freeze(false);

  return ret;
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
SQI_Object *SQI_Keyword::Clone(SQI_Heap *target = NULL)
{
  if(!target)
    target = heap;

  SQI_Keyword *obj = new SQI_Keyword(target,&data);

  return obj;
}

/*
 * function    : Show
 * purpose     : return a char* of the Keyword
 * input       : none
 * output      : char *
 * side effect : none
 */
string *SQI_Keyword::Show(int prec=3)
{
  	string *str = new string;
  
	*str += "\"";
	*str += data;	 
  
  	return str;
}

/*
 * function    : InstantiateAKeyword
 * purpose     : Instantiate a keyword from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateAKeyword(SQI_Heap *target,BMessage *archive)
{
	SQI_Keyword *str = new SQI_Keyword(target,"");
	
	if(str->Instantiate(target,archive)==B_OK)
		return str;
	else
	{
		str->Suicidal(true);
		return NULL;	
	}	
}



