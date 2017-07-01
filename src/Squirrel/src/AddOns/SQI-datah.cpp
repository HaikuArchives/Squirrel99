/*
 * Squirrel project
 *
 * file ?	SQI-datah.cpp
 * what	?       Data Handling builtin predicate
 * who	?	jlv
 * when	?	08/02/99
 * last	?	08/10/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include <iostream.h>
#include "SQI-AddOn.h"

// now some infos on the package

char *DataH_Pkg_name = "Data_Handling";
char *DataH_Pkg_purpose = "Various builtin for working on data";
char *DataH_Pkg_author = "[e-]";
char *DataH_Pkg_version = "0.5";
char *DataH_Pkg_date = "08/10/99";

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

SQI_Object *SQI_DataH_Clone(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()!=1)
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

SQI_Object *SQI_DataH_DeepClone(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()!=1)
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

SQI_Object *SQI_DataH_First(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()==0)
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
	  ret = squirrel->LocalHeap->List();

	break;
      }
    case SQI_STRING:
      {
	SQI_String *astr = dynamic_cast<SQI_String *>(obj);
	char *str = (char *)malloc(sizeof(char)*2);
	str[0] = (*astr)[0];
	str[1] = '\0';

	ret = squirrel->LocalHeap->String(str);

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

SQI_Object *SQI_DataH_Last(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()==0)
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
	  ret = squirrel->LocalHeap->List();

	break;
      }
    case SQI_STRING:
      {
	SQI_String *astr = dynamic_cast<SQI_String *>(obj);
	char *str = (char *)malloc(sizeof(char)*2);
	//string *src = astr->Data();
	str[0] = (*astr)[astr->Length()-1];
	str[1] = '\0';

	ret = squirrel->LocalHeap->String(str);

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

SQI_Object *SQI_DataH_ButFirst(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"butfirst"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
	SQI_List *alist = dynamic_cast<SQI_List *>(obj);
	SQI_List *resu = squirrel->LocalHeap->List();

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

	ret = squirrel->LocalHeap->String(str);

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

SQI_Object *SQI_DataH_ButLast(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()==0)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"butlast"));

  SQI_Object *obj = (*args)[0];
  SQI_Object *ret = NULL;

  switch(obj->Is())
    {
    case SQI_LIST:
      {
	SQI_List *alist = dynamic_cast<SQI_List *>(obj);
	SQI_List *resu = squirrel->LocalHeap->List();

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

	ret = squirrel->LocalHeap->String(str);

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

SQI_Object *SQI_DataH_Item(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->size()!=2)
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
	    
	    ret = squirrel->LocalHeap->String(str);
	    
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

SQI_Object *SQI_DataH_Word(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  for(i=args->begin();i!=args->end();i++)
    {
      if((*i))
	if((*i)->Is()==SQI_KEYWORD)
	  {
	    obj = dynamic_cast<SQI_Keyword *>(*i);
	    
	    if(str)
			*str += *(obj->Data());
	    else
	      str = new string(*(obj->Data()));
	    
	  }
    }

  if(str)
    {
      SQI_String * ret = squirrel->LocalHeap->String(*str);
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

SQI_Object *SQI_DataH_IsBool(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsList(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsWord(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsString(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsNumber(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsInteger(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsFloat(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
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

SQI_Object *SQI_DataH_IsBlock(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Keyword *obj;
  string *str = NULL;

  if(!args->size())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.Block","need 1 argument"));

  if(IsBlock((*args)[0]))
    return interp->True;
  else
    return interp->False;
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
  Map->RemoveFunc("copy");
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
}

/*
 * function    : SQI_DataH_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 *
 * output      : none
 * side effect : none
 */
void SQI_DataH_Init(SQI_BuiltInMap *Map)
{
  Map->AddAddOn(DataH_Pkg_name,DataH_Pkg_purpose,DataH_Pkg_author,DataH_Pkg_version,DataH_Pkg_date,SQI_DataH_Remove);

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
	       SQI_DataH_IsString
	       );

  Map->AddFunc("is.float",
	       DataH_Pkg_name,
	       "Return true if the argument is a float",
	       "is.float thing",
	       SQI_DataH_IsString
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
}

