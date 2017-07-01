/*
 * Squirrel project
 *
 * file ?	SQI-list.cpp
 * what	?	List class 
 * who	?	jlv
 * when	?	07/16/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <math.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_List class

/*
 * function    : SQI_List
 * purpose     : initialise the list
 * input       :
 *
 * long id, the object ID
 *
 * output      : none
 * side effect : none
 */
SQI_List::SQI_List(SQI_Heap *h)
  : SQI_Object(h,SQI_LIST)
{
  data = new list<SQI_Object *>;
  complex = false;
}

/*
 * function    : ~SQI_List
 * purpose     : destroy the list
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_List::~SQI_List()
{
  delete data;
}

/*
 * function    : Archive
 * purpose     : Store the list in an message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_List::Archive(BMessage *archive)
{
	list<SQI_Object *>::const_iterator i;	
	
	// first we set the type if the object
	archive->AddInt8("SQI_Object",SQI_LIST);
	// the number of element
	archive->AddInt32("size",data->size());
	// we iterate on each element and store the archive of each element in the archive of the list
	for(i=data->begin();i!=data->end();i++)
  	{
 		BMessage element;
 		(*i)->Archive(&element);
 		archive->AddMessage("element",&element);
  	}		
  	
  	// that's all
  	
  	return B_OK;
}

/*
 * function    : Instantiate
 * purpose     : Fill the list with the element in the archive message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_List::Instantiate(SQI_Heap *target,BMessage *archive)
{
	type_code type;
	int32 count;
	SQI_Object *elem;	
	
	archive->GetInfo("element",&type,&count);
	
	for(int32 i=0;i<count;i++)
	{
		BMessage message;
		archive->FindMessage("element",i,&message);
		elem = InstantiateAnObject(target,&message);
		if(elem)
			Add2End(elem);
	}				
	
	return B_OK;
}	
	
/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_List::Suicidal(bool force=false)
{
  	list<SQI_Object *>::const_iterator i;
  	  
 	if(status || force)
 		if(!REF)
 		{
  			for(i=data->begin();i!=data->end();i++)
  			{
  				(*i)->Alone();
    			(*i)->RemRef(force);
  			}	
    	
			delete this;
			return true;
 		}			
 		
 	return false;
}


/*
 * function    : Show
 * purpose     : return a char* of the List
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_List::Show(int prec = 3)
{
  if(data->empty())
      return new string("[]");
  else
    {
      string *out = new string("["),*cur;
      list<SQI_Object *>::const_iterator i;
      
      for(i=data->begin();i!=data->end();i++)
		{
		  cur = (*i)->Show(prec);
		  *out += *cur;
		  delete cur;
		  *out += " ";
		}

	  (*out)[out->length()-1] = ']';
	
      return out;
    }
}

/*
 * function    : Print (without [])
 * purpose     : return a char* of the List
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_List::Print(int prec = 3)
{
  if(data->empty())
      return new string("");
  else
    {
      string *out = new string,*cur;
      list<SQI_Object *>::const_iterator i;
      
      for(i=data->begin();i!=data->end();i++)
		{
		  cur = (*i)->Print(prec);
		  *out += *cur;
		  delete cur;
		  *out += " ";
		}
	
	  out->erase(out->length()-1);
	
      return out;
    }
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_List::Dump()
{
  string *value = this->Print();
  ostrstream out;
  
  out << "LST[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
 
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
void SQI_List::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
  if(status || force) // Eternal object cannot be Exported
    {      
      // when a list is exported , all the element of the list are exported to

      list<SQI_Object *>::const_iterator i;
      
      for(i=data->begin();i!=data->end();i++)
		(*i)->Export(nheap,force);

      nheap->Import(this); // we call the heap method
    }
}

/*
 * function    : Add2Begin
 * purpose     : Insert a new object at the begin of the list
 * input       : 
 *
 * SQI_Object *, object to insert in the list
 *
 * output      : none
 * side effect : none
 */
void SQI_List::Add2Begin(SQI_Object *obj)
{
  data->push_front(obj);
  obj->AddRef();
  obj->Contained();
  obj->Export(heap);
}

/*
 * function    : Add2End
 * purpose     : Insert a new object at the end of the list
 * input       : 
 *
 * SQI_Object *, object to insert in the list
 *
 * output      : none
 * side effect : none
 */
void SQI_List::Add2End(SQI_Object *obj)
{
  data->push_back(obj);
  obj->AddRef();
  obj->Contained();
  obj->Export(heap);
}

/*
 * function    : Add2End
 * purpose     : Insert a new object at a position in the list
 * input       : 
 *
 * SQI_Object *, object to insert in the list
 * int32 index, position where to insert
 *
 * output      : none
 * side effect : none
 */
void SQI_List::AddAt(SQI_Object *obj,int32 index)
{
  list<SQI_Object *>::iterator i;
  
  if(index>=0 && index<data->size())
  {
  	int32 j = 0;
  	
  	obj->AddRef();
  	obj->Contained();
  	obj->Export(heap);
  	
  	for(i=data->begin();i!=data->end();i++,j++)
  		if(j==index)
  			break;
  			
  	data->insert(i,obj);			  	
  }			
}


/*
 * function    : operator[]
 * purpose     : Return the n-ieme element of the list
 * input       : 
 *
 * int i, indice of the element to return
 *
 * output      : SQI_Object *, the object
 * side effect : none
 */
SQI_Object *SQI_List::operator[](int i)
{
  if(i>=data->size())
    return NULL;
  else
    {
      list<SQI_Object *>::const_iterator j;
      j=data->begin();
      for(int k=0;k<i;k++)
		j++;
      return *j;
    }
}

/*
 * function    : operator=
 * purpose     : affect a list object to another (not yet implemented)
 * input       :
 *
 * SQI_Object *, pointer to the list object
 *
 * output      : SQI_Object *, the object
 * side effect : none
 */
SQI_Object *SQI_List::operator=(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List = Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }
}

/*
 * function    : operator+
 * purpose     : Create a list from the concatenation of 2 lists (not yet implemented)
 * input       :
 *
 * SQI_Object *, pointer to the list to use
 *
 * output      : SQI_Object *, the list created
 * side effect : none
 */
SQI_Object *SQI_List::operator+(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List + Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }    
}

/*
 * function    : operator-
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the list to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_List::operator-(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List - Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }
}

/*
 * function    : operator*
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the list to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_List::operator*(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List * Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }
}

/*
 * function    : operator/
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the list to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_List::operator/(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List / Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }
}

/*
 * function    : operator%
 * purpose     : no sens for the moment
 * input       :
 *
 * SQI_Object *, pointer to the list to use
 *
 * output      : SQI_Object *,
 * side effect : none
 */
SQI_Object *SQI_List::operator%(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    {
      throw (new SQI_Exception(SQI_EX_BADTYPE,"List % Object"));
    }
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      return this;
    }
}


/*
 * function    : operator==
 * purpose     : return true if 2 list are equal
 * input       :
 *
 * SQI_Object *, pointer to the list to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator==(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    return false;
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      if(Length()==obj->Length())
		{
		  bool test = true;
		  list<SQI_Object *>::const_iterator a,b;
	
		  for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
		    test = test && (*a == *b);
	
		  return test;
		}
      else
		return false;
    }
}

/*
 * function    : operator!=
 * purpose     : return true if 2 list are non equal
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator!=(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    return true;
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      if(Length()==obj->Length())
	{
	  bool test = true;
	  list<SQI_Object *>::const_iterator a,b;
	  
	  for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
	    {  
	      test = test && (*a == *b);
	      if(!test)
		break;
	    }

	  return !test;
	}
      else
	return true;
    }
}

/*
 * function    : operator>
 * purpose     : return true if the list is > than the other
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator>(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    return false;
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      if(Length()==obj->Length())
	{
	  bool test = true;
	  list<SQI_Object *>::const_iterator a,b;
	  
	  for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
	    {  
	      test = test && (*a > *b);
	      if(!test)
		break;
	    }
	  
	  return test; 
	}
    }
}

/*
 * function    : operator<
 * purpose     : return true if the list is < than the othe
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator<(SQI_Object *op2)
  {
    if(op2->TYPE!=SQI_LIST)
      return false;
    else
      {
	SQI_List *obj = dynamic_cast<SQI_List *>(op2);
	if(Length()==obj->Length())
	  {
	    bool test = true;
	    list<SQI_Object *>::const_iterator a,b;
	    
	    for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
	      {  
	      test = test && (*a < *b);
	      if(!test)
		break;
	    }
	  
	  return test;
	}
      }
  }

/*
 * function    : operator>=
 * purpose     : return true if the list is >= than the other
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator>=(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    return false;
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      if(Length()==obj->Length())
	{
	  bool test = true;
	  list<SQI_Object *>::const_iterator a,b;
	  
	  for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
	    {  
	      test = test && (*a >= *b);
	      if(!test)
		break;
	    }
	  
	  return test;
	}
    }
}

/*
 * function    : operator<=
 * purpose     : return true if the list is <= than the other
 * input       :
 *
 * SQI_Object *, pointer to the object to test
 *
 * output      : bool
 * side effect : none
 */
bool SQI_List::operator<=(SQI_Object *op2)
{
  if(op2->TYPE!=SQI_LIST)
    return false;
  else
    {
      SQI_List *obj = dynamic_cast<SQI_List *>(op2);
      if(Length()==obj->Length())
	{
	  bool test = true;
	  list<SQI_Object *>::const_iterator a,b;
	  
	  for(a=data->begin(),b=obj->data->begin();a!=data->end();a++,b++)
	    {  
	      test = test && (*a <= *b);
	      if(!test)
		break;
	    }
	  
	  return test;
	}
    }
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
SQI_Object *SQI_List::Clone(SQI_Heap *target = NULL)
{
  if(!target)
    target = heap;

  SQI_List *obj = new SQI_List(target);
  SQI_Object *elem;
  list<SQI_Object *>::const_iterator i;
  for(i=data->begin();i!=data->end();i++)
    {
      elem = *i;
      obj->Add2End(elem);
    }

  return obj;
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
SQI_Object *SQI_List::DeepClone(SQI_Heap *target = NULL)
{
   if(!target)
    target = heap;

  SQI_List *obj = new SQI_List(target);
  SQI_Object *elem;
  list<SQI_Object *>::const_iterator i;
  for(i=data->begin();i!=data->end();i++)
    {
      elem = (*i)->DeepClone(target);
      obj->Add2End(elem);
    }

  return obj;
}

/*
 * function    : Max
 * purpose     : Return the maximum value of all the number element in the list
 * input       : none
 * output      : SQI_Object *, the maximum or NULL if no number found
 * side effect : none
 */
SQI_Number *SQI_List::Max()
{
  list<SQI_Object *>::const_iterator i;
  SQI_Number *max;
  SQI_Object *obj;

  for(i=data->begin();i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  max = dynamic_cast<SQI_Number *>(obj);
	  break;
	}
    }
  
  for(i++;i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  if(*max<obj)
	    max = dynamic_cast<SQI_Number *>(obj);
	}
    }

  return max;
}

/*
 * function    : Min
 * purpose     : Return the minimum value of all the number element in the list
 * input       : none
 * output      : SQI_Object *, the maximum or NULL if no number found
 * side effect : none
 */
SQI_Number *SQI_List::Min()
{
  list<SQI_Object *>::const_iterator i;
  SQI_Number *min;
  SQI_Object *obj;

  for(i=data->begin();i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  min = dynamic_cast<SQI_Number *>(obj);
	  break;
	}
    }
  
  for(i++;i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  if(*min>obj)
	    min = dynamic_cast<SQI_Number *>(obj);
	}
    }

  return min;
}

/*
 * function    : Sum
 * purpose     : Return the sum of all the number element in the list
 * input       : 
 *
 * int *num, set to the number of number in the list
 *
 * output      : SQI_Object *, the maximum or NULL if no number found
 * side effect : none
 */
SQI_Number *SQI_List::Sum(long *num)
{
  list<SQI_Object *>::const_iterator i;
  SQI_Number *sum;
  SQI_Object *obj;
  *num =0;

  for(i=data->begin();i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  sum = dynamic_cast<SQI_Number *>(obj->Clone());
	  *num = 1;
	  break;
	}
    }
  
  for(i++;i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  (*num)++;
	  *sum+=dynamic_cast<SQI_Number *>(obj);
	}
    }

  return sum;
}

/*
 * function    : Product
 * purpose     : Return the product of all the number element in the list
 * input       : 
 * output      : SQI_Object *, the product or NULL if no number found
 * side effect : none
 */
SQI_Number *SQI_List::Product()
{
  list<SQI_Object *>::const_iterator i;
  SQI_Number *prod = NULL;
  SQI_Object *obj;

  for(i=data->begin();i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
	{
	  prod = dynamic_cast<SQI_Number *>(obj->Clone());
	  break;
	}
    }
  
  for(i++;i!=data->end();i++)
    {
      obj = *i;
      if(obj->Is()==SQI_NUMBER)
		*prod *= dynamic_cast<SQI_Number *>(obj);
    }

  return prod;
}

/*
 * function    : Find
 * purpose     : Return true if an object is present in the list
 * input       :
 *
 * SQI_Object *, the object to find
 * 
 * output      : bool
 * side effect : none
 */
bool SQI_List::Find(SQI_Object *object,int *position = NULL)
{
	list<SQI_Object *>::const_iterator location;
	int i = 0;
	
	for(location = data->begin();location!=data->end();location++,i++)
		if(*(*location) == object)
			break;
				
	if(location!=data->end())
	{
		if(position)
			*position = i;
		return true;
	}	
	else
	{
		if(position)
			*position = -1;
		return false;	
	}
}

/*
 * function    : InstantiateAList
 * purpose     : Instantiate a list from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateAList(SQI_Heap *target,BMessage *archive)
{
	SQI_List *lst = new SQI_List(target);
	
	if(lst->Instantiate(target,archive)==B_OK)
		return lst;
	else
	{
		lst->Suicidal(true);
		return NULL;	
	}	
}

