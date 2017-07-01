/*
 * Squirrel project
 *
 * file ?	SQI-datah.cpp
 * what	?       Data Handling builtin predicate
 * who	?	jlv
 * when	?	08/02/99
 * last	?	12/09/00
 *
 *
 * (c) electron Technology 1999-2000
 */

// First the needed header

#include <iostream.h>
#include "SQI-AddOn.h"

// now some infos on the package

char *DataH_Pkg_name = "Data Processing";
char *DataH_Pkg_purpose = "Various builtin for working on data";
char *DataH_Pkg_author = "Kirilla";
char *DataH_Pkg_version = "0.10";
char *DataH_Pkg_date = "12/09/00";

/*
 * function    : SQI_DataH_Clone
 * purpose     : Builtin predicate "clone"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_Clone(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"clone","need one arg"));
    
  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  ret = obj->Clone(squirrel->LocalHeap);

  return ret;
}

/*
 * function    : SQI_DataH_DeepClone
 * purpose     : Builtin predicate "deepclone"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_DeepClone(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"deepclone","need one arg"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  ret = obj->DeepClone(squirrel->LocalHeap);

  return ret;
}


/*
 * function    : SQI_DataH_First
 * purpose     : Builtin predicate "first"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_First(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"first"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
		SQI_List *alist = dynamic_cast<SQI_List *>(obj);

		if(alist->Length())
	  	{
	    	list<SQI_Object *>::const_iterator *i = alist->Iterator();
	    	ret = *(*i);
	  	}
		else
	  		ret = new SQI_List(squirrel->LocalHeap);

		break;
      }
    case SQI_STRING:
      {
		SQI_String *astr = dynamic_cast<SQI_String *>(obj);
		char *str = (char *)malloc(sizeof(char)*2);
		str[0] = (*astr)[0];
		str[1] = '\0';

		ret = new SQI_String(squirrel->LocalHeap,str);

		free(str);
		break;
      }
    default:
    	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"first","argument must be a word/string/list"));
    }

  return ret;
}

/*
 * function    : SQI_DataH_Last
 * purpose     : Builtin predicate "last"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_Last(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"last"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
		SQI_List *alist = dynamic_cast<SQI_List *>(obj);

		if(alist->Length())
	  	{
	    	list<SQI_Object *>::const_iterator *i = alist->End();
	    	(*i)--;
	    	ret = *(*i);
	  	}
		else
	  	ret = new SQI_List(squirrel->LocalHeap);

		break;
      }
    case SQI_STRING:
      {
		SQI_String *astr = dynamic_cast<SQI_String *>(obj);
		char *str = (char *)malloc(sizeof(char)*2);
		//string *src = astr->Data();
		str[0] = (*astr)[astr->Length()-1];
		str[1] = '\0';

		ret = new SQI_String(squirrel->LocalHeap,str);

		free(str);
		break;
      }
    default:
    	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"last","argument must be a word/string/list"));
    }

  return ret;
}

/*
 * function    : SQI_DataH_ButFirst
 * purpose     : Builtin predicate "butfirst"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_ButFirst(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"butfirst"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
		SQI_List *alist = dynamic_cast<SQI_List *>(obj);
		SQI_List *resu = new SQI_List(squirrel->LocalHeap);

		if(alist->Length())
	  	{
	    	list<SQI_Object *>::const_iterator *i = alist->Iterator();
	    	list<SQI_Object *>::const_iterator *e = alist->End();
	    
	    	for((*i)++;(*i)!=(*e);(*i)++)
	      		resu->Add2End(*(*i));
	  	}	  

		ret = resu;

		break;
      }
    case SQI_STRING:
      {
		SQI_String *astr = dynamic_cast<SQI_String *>(obj);
		int size = astr->Length();
		char *str = (char *)malloc(sizeof(char)*size);
	
		for(int i=0;i<size;i++)
	  		str[i] = (*astr)[i+1];

		str[size-1]='\0';

		ret = new SQI_String(squirrel->LocalHeap,str);

		free(str);
		break;
      }
    default:
          throw(new SQI_Exception(SQI_EX_BADARGTYPE,"butfirst","argument must be a word/string/list"));
    }

  return ret;
}

/*
 * function    : SQI_DataH_ButLast
 * purpose     : Builtin predicate "butlast"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_ButLast(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"butlast"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
		SQI_List *alist = dynamic_cast<SQI_List *>(obj);
		SQI_List *resu = new SQI_List(squirrel->LocalHeap);

		if(alist->Length())
	  	{
	    	list<SQI_Object *>::const_iterator *i = alist->Iterator();
	    	list<SQI_Object *>::const_iterator *e = alist->End();
	    	(*e)--;

	    	for((*e)--;(*e)!=(*i);(*e)--)
	      		resu->Add2Begin(*(*e));

	    	resu->Add2Begin(*(*e));
	  	}	  

		ret = resu;

		break;
      }
    case SQI_STRING:
      {
		SQI_String *astr = dynamic_cast<SQI_String *>(obj);
		int size = astr->Length();
		char *str = (char *)malloc(sizeof(char)*size);
	
		for(int i=0;i<size-1;i++)
	  		str[i] = (*astr)[i];

		str[size-1]='\0';

		ret = new SQI_String(squirrel->LocalHeap,str);

		free(str);
		break;
      }
    default:
          throw(new SQI_Exception(SQI_EX_BADARGTYPE,"butfirst","argument must be a word/string/list"));
    }

  return ret;
}

/*
 * function    : SQI_DataH_Item
 * purpose     : Builtin predicate "item"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_Item(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"item","need 2 arguments"));

  SQI_Object *item = (*args)[0];
  SQI_Object *obj = (*args)[1];
  SQI_Object *ret = NULL;

  if(item->Is()!=SQI_NUMBER)
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"item","the index must be a number"));

  SQI_Number *index = dynamic_cast<SQI_Number *>(item);

  int i;

  switch(index->IsA())
    {
    case SQI_INT:
      {
		i = index->iData();
		break;
      }
    case SQI_LONG:
      {
		i = index->lData();
		break;
      }
    case SQI_FLOAT:
    case SQI_DOUBLE:
      throw(new SQI_Exception(SQI_EX_BADARGTYPE,"item","the index must be a int/long"));
    }
  
  switch(obj->Is())
    {
    case SQI_LIST:
      {
		SQI_List *alist = dynamic_cast<SQI_List *>(obj);

		if((i>0) && (alist->Length()>=i))
	  		ret = (*alist)[i];
		else
	  		throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"item",index->Print()));

		break;
      }
    case SQI_STRING:
      {
		SQI_String *astr = dynamic_cast<SQI_String *>(obj);

		if((i>0)&&(i<=astr->Length()))
	  	{
	    	char *str = (char *)malloc(sizeof(char)*2);
	    	str[0] = (*astr)[i-1];
	    	str[1] = '\0';
	    
	    	ret = new SQI_String(squirrel->LocalHeap,str);
	    
	    	free(str);
	  	}
		else
	  		throw(new SQI_Exception(SQI_EX_OUTOFRANGE,"item",index->Print()));
	
		break;
      }
    default:
          throw(new SQI_Exception(SQI_EX_BADARGTYPE,"first","argument must be a word/string/list"));
    }

  return ret;
}

/*
 * function    : SQI_DataH_Word
 * purpose     : Builtin predicate "word"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_Word(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  long i;
  SQI_Object *obj;
  SQI_Keyword *word;
  string *str = NULL;

  for(i=0;i<args->Length();i++)
    {
    	obj = (*args)[i];
      	if(obj)
			if(obj->Is()==SQI_KEYWORD)
	  		{
	    		word = dynamic_cast<SQI_Keyword *>(obj);
	    
	    		if(str)
					*str += *(word->Data());
	    		else
	      			str = new string(*(word->Data()));
	    
	  		}
    }

  if(str)
    {
      SQI_String * ret = new SQI_String(squirrel->LocalHeap,*str);
      delete str;
      return ret;
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"word","no word found in the argument"));
}

/*
 * function    : SQI_DataH_IsBool
 * purpose     : Builtin predicate "is.bool"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsBool(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.bool","need 1 argument"));

  if(IsBool((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsList
 * purpose     : Builtin predicate "is.list"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.list","need 1 argument"));

  if(IsList((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsWord
 * purpose     : Builtin predicate "is.word"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsWord(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.word","need 1 argument"));

  if(IsKeyword((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsString
 * purpose     : Builtin predicate "is.string"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsString(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.string","need 1 argument"));

  if(IsString((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsNumber
 * purpose     : Builtin predicate "is.number"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsNumber(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.Number","need 1 argument"));

  if(IsNumber((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsInteger
 * purpose     : Builtin predicate "is.integer"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsInteger(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.integer","need 1 argument"));

  if(IsInteger((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsFloat
 * purpose     : Builtin predicate "is.float"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsFloat(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.float","need 1 argument"));

  if(IsFloat((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsBlock
 * purpose     : Builtin predicate "is.block"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsBlock(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.block","need 1 argument"));

  if(IsBlock((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_IsObject
 * purpose     : Builtin predicate "is.object"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsObject(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.object","need 1 argument"));

  if(IsThePointer((*args)[0]))
    return interp->True;
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_Image
 * purpose     : Builtin predicate "Image"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_DataH_Image(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	color_space space = B_RGB_32_BIT;
	
	SQI_Object *init = NULL;
	
	if(args->Length()>=1)
	{
		SQI_String *path = IsString((*args)[0]);
		if(path)
		{
			string *p = path->Data();
			return NewImage(squirrel,p->c_str());	
		}	
		else
		{
			SQI_Number *w,*h;
			
			w = IsInteger((*args)[0]);
			h = IsInteger((*args)[1]);
			
			if(w && h)
				return NewImage(squirrel,Number2Int(w),Number2Int(h),space);
			else
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image","Size of the image is given in integer"));	
		}
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image","need an input at least."));			
}

/*
 * function    : SQI_DataH_IsImage
 * purpose     : Builtin predicate "is.image"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_IsImage(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.image","need 1 argument"));

  SQI_ThePointer *ptr = IsThePointer((*args)[0]);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_IMAGE)
    	return interp->True;
    else
    	return interp->False;
  } 
  else
    return interp->False;
}

/*
 * function    : SQI_DataH_Trans
 * purpose     : Builtin predicate "trans.name"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_TransName(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	TMTranslators::const_iterator i;
	SQI_List *lst = new SQI_List(squirrel->LocalHeap);
			
	for(i=Translators.begin();i!=Translators.end();i++)
		lst->Add2End(new SQI_String(squirrel->LocalHeap,i->first));	
				
	return lst;  
}

/*
 * function    : SQI_DataH_Trans
 * purpose     : Builtin predicate "trans.mime"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_DataH_TransMime(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	TMTranslators::const_iterator i;
	SQI_List *lst = new SQI_List(squirrel->LocalHeap);
			
	for(i=Translators.begin();i!=Translators.end();i++)
		lst->Add2End(new SQI_String(squirrel->LocalHeap,i->second.mime));	
				
	return lst;  
}

/*
 * function    : SQI_DataH_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_DataH_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("deepclone");
  Map->RemoveFunc("clone");
  Map->RemoveFunc("first");
  Map->RemoveFunc("last");
  Map->RemoveFunc("butfirst");
  Map->RemoveFunc("butlast");
  Map->RemoveFunc("item");
  Map->RemoveFunc("word");
  Map->RemoveFunc("is.bool");
  Map->RemoveFunc("is.list");
  Map->RemoveFunc("is.word");
  Map->RemoveFunc("is.string");
  Map->RemoveFunc("is.number");
  Map->RemoveFunc("is.float");
  Map->RemoveFunc("is.integer");
  Map->RemoveFunc("is.block"); 
  Map->RemoveFunc("is.object");
  Map->RemoveFunc("Image");
  Map->RemoveFunc("is.image");
  Map->RemoveFunc("trans.name");
  Map->RemoveFunc("trans.mime");
}

/*
 * function    : SQI_DataH_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
  Map->AddAddOn(addon_id,DataH_Pkg_name,DataH_Pkg_purpose,DataH_Pkg_author,DataH_Pkg_version,DataH_Pkg_date,SQI_DataH_Remove);

  Map->AddFunc("clone",
	       DataH_Pkg_name,
	       "Output a clone (copy) of the argument",
	       "clone thing",
	       SQI_DataH_Clone
	       );

   Map->AddFunc("deepclone",
	       DataH_Pkg_name,
	       "Output a deepclone (copy) of the argument",
	       "deepclone thing",
	       SQI_DataH_DeepClone
	       );

  Map->AddFunc("first",
	       DataH_Pkg_name,
	       "Return the first element of a list or the first character of a string",
	       "first thing",
	       SQI_DataH_First
	       );

  Map->AddFunc("last",
	       DataH_Pkg_name,
	       "Return the last element of a list or the first character of a string",
	       "last thing",
	       SQI_DataH_Last
	       );

  Map->AddFunc("butfirst",
	       DataH_Pkg_name,
	       "Return the all the element of a list or all characters of a string but the first",
	       "butfirst thing",
	       SQI_DataH_ButFirst
	       );

  Map->AddFunc("butlast",
	       DataH_Pkg_name,
	       "Return the all the element of a list or all characters of a string but the last",
	       "butlast thing",
	       SQI_DataH_ButLast
	       );

  Map->AddFunc("item",
	       DataH_Pkg_name,
	       "Return the nth element of a list or a string",
	       "item index thing",
	       SQI_DataH_Item
	       );

  Map->AddFunc("word",
	       DataH_Pkg_name,
	       "Output a word formed by the concatenation of the argument",
	       "word \"word1 \"word2 ...",
	       SQI_DataH_Word
	       );

  Map->AddFunc("is.bool",
	       DataH_Pkg_name,
	       "Return true if the argument is a boolean",
	       "is.bool thing",
	       SQI_DataH_IsBool
	       );

  Map->AddFunc("is.list",
	       DataH_Pkg_name,
	       "Return true if the argument is a list",
	       "is.list thing",
	       SQI_DataH_IsList
	       );

  Map->AddFunc("is.word",
	       DataH_Pkg_name,
	       "Return true if the argument is a word",
	       "is.word thing",
	       SQI_DataH_IsWord
	       );

  Map->AddFunc("is.string",
	       DataH_Pkg_name,
	       "Return true if the argument is a word",
	       "is.string thing",
	       SQI_DataH_IsString
	       );

  Map->AddFunc("is.number",
	       DataH_Pkg_name,
	       "Return true if the argument is a number",
	       "is.number thing",
	       SQI_DataH_IsNumber
	       );

  Map->AddFunc("is.float",
	       DataH_Pkg_name,
	       "Return true if the argument is a float",
	       "is.float thing",
	       SQI_DataH_IsFloat
	       );

  Map->AddFunc("is.integer",
	       DataH_Pkg_name,
	       "Return true if the argument is an integer",
	       "is.integer thing",
	       SQI_DataH_IsInteger
	       );

  Map->AddFunc("is.block",
	       DataH_Pkg_name,
	       "Return true if the argument is an block",
	       "is.block thing",
	       SQI_DataH_IsBlock
	       );
	       
	Map->AddFunc("is.object",
	       DataH_Pkg_name,
	       "Return true if the argument is an object",
	       "is.object thing",
	       SQI_DataH_IsObject
	       );
	       
  	Map->AddFunc("trans.name",
	       DataH_Pkg_name,
	       "Output the list of installed translators",
	       "trans.name",
	       SQI_DataH_TransName
	       );
	       
  	Map->AddFunc("trans.mime",
	       DataH_Pkg_name,
	       "Output the list of mime type that have an installed translators",
	       "trans.mime",
	       SQI_DataH_TransMime
	       );	             	     
      	       	       	       
  	Map->AddFunc("Image",
	       DataH_Pkg_name,
	       "Output a new image",
	       "Image",
	       SQI_DataH_Image
	       );
	       
  	Map->AddFunc("is.image",
	       DataH_Pkg_name,
	       "Output true if the input is an image",
	       "is.image thing",
	       SQI_DataH_IsImage
	       );	       
}

