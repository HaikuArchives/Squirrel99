/*
 * Squirrel project
 *
 * file ?	SQI-listh.cpp
 * what	?	List handling builtin predicate
 * who	?	jlv
 * when	?	08/01/99
 * last	?	08/09/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include <iostream.h>
#include <algorithm>
#include "SQI-AddOn.h"

// now some infos on the package

char *ListH_Pkg_name = "List_Handling";
char *ListH_Pkg_purpose = "List Handling builtin";
char *ListH_Pkg_author = "[e-]";
char *ListH_Pkg_version = "0.3";
char *ListH_Pkg_date = "08/09/99";

/*
 * function    : SQI_ListH_List
 * purpose     : Builtin predicate "list"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_List(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_List *rlist = squirrel->LocalHeap->List();

  for(i=args->begin();i!=args->end();i++)
    {
      if((*i))
	rlist->Add2End((*i)->Clone(squirrel->LocalHeap));
    }

  return rlist;
}

/*
 * function    : SQI_ListH_Sentence
 * purpose     : Builtin predicate "sentence"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Sentence(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_List *rlist = squirrel->LocalHeap->List();

  for(i=args->begin();i!=args->end();i++)
    {
      if((*i))
	{
	  if((*i)->Is()==SQI_LIST) // we flatten the list in input
	    {
	      SQI_List *alist = dynamic_cast<SQI_List *>(*i);

	      list<SQI_Object *>::const_iterator *j = alist->Iterator();
	      list<SQI_Object *>::const_iterator *e = alist->End();

	      for(*j;*j!=*e;(*j)++)
		{
		  rlist->Add2End(*(*j));
		}
		  

	      delete j;
	      delete e;
	    }
	  else
	    rlist->Add2End((*i)->Clone(squirrel->LocalHeap));
	}
    }

  return rlist;
}

/*
 * function    : SQI_ListH_GSeq
 * purpose     : Builtin predicate "gseq"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_GSeq(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_List *rlist = squirrel->LocalHeap->List();
  
  switch(args->size())
  	{
	case 2:
	  {
	    SQI_Object *ag1 = (*args)[0];
	    SQI_Object *ag2 = (*args)[1];
	    
	    if((ag1->Is()==SQI_NUMBER)&&(ag2->Is()==SQI_NUMBER))
	      {
		SQI_Number *from = dynamic_cast<SQI_Number *>(ag1);
		SQI_Number *to = dynamic_cast<SQI_Number *>(ag2);
		SQI_Number *swap;
		
		if(*from>to)
		  {
		    swap = from;
		    from = to;
		    to = swap;	
		  }	
		
		SQI_Object *i = from->Clone(squirrel->LocalHeap);
		
		for(i;*i<=to;i = *i + 1)
		  rlist->Add2End(i);
		
		delete i;
	      }
	    else
	      throw(new SQI_Exception(SQI_EX_BADARGTYPE,"gseq","arguments must be number"));
	    
	    break;		
	    
	  }
	case 3:
	  {
	    SQI_Object *ag1 = (*args)[0];
	    SQI_Object *ag2 = (*args)[1];
	    SQI_Object *ag3 = (*args)[2];
	    
	    if((ag1->Is()==SQI_NUMBER)&&(ag2->Is()==SQI_NUMBER)&&(ag3->Is()==SQI_NUMBER))
	      {
		SQI_Number *from = dynamic_cast<SQI_Number *>(ag1);
		SQI_Number *to = dynamic_cast<SQI_Number *>(ag2);
		SQI_Number *count = dynamic_cast<SQI_Number *>(ag3);
		SQI_Number *swap;
		
		if(*from>to)
		  {
		    swap = from;
		    from = to;
		    to = swap;	
		  }	
		
		SQI_Object *i = from->Clone(squirrel->LocalHeap);
		SQI_Object *change = *to - from;
		SQI_Object *div = *count - 1;
		SQI_Object *step = *change / div;
		
		for(i;*i<=to;i = *i + step)
		  rlist->Add2End(i);
		
		rlist->Add2End(i);
		
		delete step;
		delete div;
		delete change;
	      }
	    else
	      throw(new SQI_Exception(SQI_EX_BADARGTYPE,"gseq","arguments must be number"));
	    
	    break;			
	  }
	default:
	  throw(new SQI_Exception(SQI_EX_BADARGSNUM,"gseq","need at least 2 arguments"));	
	  
  	}

  
  return rlist;
}

/*
 * function    : SQI_ListH_Fput
 * purpose     : Builtin predicate "fput"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Fput(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"fput","need at least 2 arguments"));

  SQI_Object *ag1 = (*args)[0];
  SQI_Object *ag2 = (*args)[1];

  if(ag2->Is()==SQI_LIST)
    {
      SQI_List *rlist = squirrel->LocalHeap->List();
     
      SQI_List *alist = dynamic_cast<SQI_List *>(ag2);
      
      list<SQI_Object *>::const_iterator *j = alist->Iterator();
      list<SQI_Object *>::const_iterator *e = alist->End();

      rlist->Add2End(ag1);
      
      for(j;*j!=*e;(*j)++)
		rlist->Add2End(*(*j));
       
      delete j;
      delete e;

      return rlist;      
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fput","second argument must be a list"));
}

/*
 * function    : SQI_ListH_Lput
 * purpose     : Builtin predicate "lput"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Lput(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"lput","need at least 2 arguments"));

  SQI_Object *ag1 = (*args)[0];
  SQI_Object *ag2 = (*args)[1];

  if(ag2->Is()==SQI_LIST)
    {
      SQI_List *rlist = squirrel->LocalHeap->List();
     
      SQI_List *alist = dynamic_cast<SQI_List *>(ag2);
      
      list<SQI_Object *>::const_iterator *j = alist->Iterator();
      list<SQI_Object *>::const_iterator *e = alist->End();

      for(j;*j!=*e;(*j)++)
	{
	  rlist->Add2End(*(*j));
	}

      rlist->Add2End(ag1);
       
      delete j;
      delete e;

      return rlist;
      
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"reverse","the argument must be a list"));
}

/*
 * function    : SQI_ListH_Reverse
 * purpose     : Builtin predicate "reverse"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Reverse(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"reverse","need one argument"));

  SQI_Object *ag = (*args)[0];

  if(ag->Is()==SQI_LIST)
    {
  
      SQI_List *rlist = squirrel->LocalHeap->List();
      SQI_List *alist = dynamic_cast<SQI_List *>(ag);
      
      list<SQI_Object *>::const_iterator *j = alist->Iterator();
      list<SQI_Object *>::const_iterator *e = alist->End();

      for(j;*j!=*e;(*j)++)
	rlist->Add2Begin(*(*j));
 
      delete j;
      delete e;

      return rlist;
      
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"reverse","second argument must be a list"));
}

/*
 * function    : SQI_ListH_Length
 * purpose     : Builtin predicate "length"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Length(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"llength","need one argument"));

  SQI_List *list = IsList((*args)[0]);

  if(list)
      return squirrel->LocalHeap->Number(list->Length());
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"llength","the argument must be a list"));
}

/*
 * function    : SQI_ListH_Scan
 * purpose     : Builtin predicate "scan"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Scan(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<3)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"scan","need at least 2 arguments"));

  SQI_List *alist = IsList((*args)[0]);

  if(alist)
    {
      if(alist->Length()>(args->size()-1))
	{
	  // more element than variable name

	  SQI_List *rlist = squirrel->LocalHeap->List();
	  SQI_Keyword *var;
	  
	  deque<SQI_Object *>::const_iterator i;
	  list<SQI_Object *>::const_iterator *j = alist->Iterator();
	  list<SQI_Object *>::const_iterator *e = alist->End();
	  
	  i=args->begin();

	  for(j,i++;i!=args->end();(*j)++,i++)
	    {
	      if(var=IsKeyword(*i))
		{
		  squirrel->SetVarValue(interp,var->Data(),*(*j));
		}
	      else
		{
		  // wrong args
		  delete rlist;
		  throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lscan","variable name must be word"));

		}
	    }

	  // and now we add the remainder to the new list

	  for(j;*j!=*e;(*j)++)
	    rlist->Add2End(*(*j));
	  
	  delete j;
	  delete e;
	  
	  return rlist;
	}
      else
	{
	  SQI_List *rlist = squirrel->LocalHeap->List();
	  SQI_Keyword *var;

	  deque<SQI_Object *>::const_iterator i;
	  list<SQI_Object *>::const_iterator *j = alist->Iterator();
	  list<SQI_Object *>::const_iterator *e = alist->End();
	  
	  i=args->begin();
	  for(j,i++;*j!=*e;(*j)++,i++)
	    {
	      if(var=IsKeyword(*i))
		{
		  squirrel->SetVarValue(interp,var->Data(),*(*j));
		}
	      else
		{
		  // wrong args
		  delete rlist;
		  throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lscan","variable name must be word"));

		}
	    }

	  // now all the variable are set to empty list

	  for(i;i!=args->end();i++)
	    {
	      if(var=IsKeyword(*i))
		  squirrel->SetVarValue(interp,var->Data(),rlist->Clone());
	      else
		{
		  // wrong args
		  delete rlist;
		  throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lscan","variable name must be word"));
		  
		}
	    }
	    
	  return rlist;
	}
      
    }
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lscan","first argument must be a list"));
}

/*
 * function    : SQI_ListH_Empty
 * purpose     : Builtin predicate "lempty"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Empty(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"lempty","need one argument"));

  SQI_List *list = IsList((*args)[0]);

  if(list)
    {
      if(list->Length())
	return interp->False;
      else
	return interp->True;
    }
  else
    return interp->True;
}

/*
 * function    : SQI_ListH_Index
 * purpose     : Builtin predicate "lindex"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Index(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"lindex","need 2 arguments"));

  SQI_List *alist = IsList((*args)[0]);
  SQI_Number *index = IsInteger((*args)[1]);
  
  if(!alist || !index)
  	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lindex","need a list as first argument and a number"));

  if(args->size()==3)
    {
      // modify the value
		
		if(!alist->IsEternal())
		{
				long ith;
				
				if(index->IsA()==SQI_INT)
					ith = index->iData();
				else
					ith = index->lData();
					
				if(ith>alist->Length() || ith < 1)
						throw(new SQI_Exception( SQI_EX_OUTOFRANGE,"lindex"));
				
				
				list<SQI_Object *>::iterator j;
				
				j = alist->Data()->begin();
			
				for(long i=1;i<ith;j++,i++);
				
				// we're on the object
				
				SQI_Object *previous = *j;

				previous->RemRef();
								
				(*args)[2]->AddRef();	
				
				alist->Data()->insert(j,(*args)[2]);
				alist->Data()->erase(j);	
				
				//delete j;
				
				return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"lindex","cannot mutate an Eternal object , make a clone first"));
    }
  else
    {
		// get the value
		
		long ith;
				
		if(index->IsA()==SQI_INT)
			ith = index->iData();
		else
			ith = index->lData();
			
		if(ith>alist->Length() || ith < 1)
				throw(new SQI_Exception( SQI_EX_OUTOFRANGE,"lindex"));
		
		
		list<SQI_Object *>::const_iterator *j = alist->Iterator();
		
		for(long i=1;i<ith;(*j)++,i++);
		
		// we're on the object
		
		SQI_Object *current = *(*j);
						
		delete j;
						
		return current;	
		
    }
}

/*
 * function    : SQI_ListH_Append
 * purpose     : Builtin predicate "lappend"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_ListH_Append(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  
  if(args->size()<2)
  	 throw(new SQI_Exception(SQI_EX_BADARGSNUM,"lappend","need at least 2 arguments"));
  
  SQI_List *thelist = IsList((*args)[0]);
  
  if(!thelist)
  	 throw(new SQI_Exception(SQI_EX_BADARGTYPE,"lappend","need a list as first argument"));
  
  if(thelist->IsEternal())
  	throw(new SQI_Exception(SQI_EX_INVALIDE,"lappend","cannot mutate an Eternal object , make a clone first"));

  for((i=args->begin())++;i!=args->end();i++)
	thelist->Add2End(*i);
  

  return thelist;
}


/*
 * function    : SQI_ListH_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_ListH_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("list");
  Map->RemoveFunc("sentence");
  Map->RemoveFunc("gseq");
  Map->RemoveFunc("fput");
  Map->RemoveFunc("lput");
  Map->RemoveFunc("reverse");
  Map->RemoveFunc("llength");
  Map->RemoveFunc("lget");
  Map->RemoveFunc("lscan");
  Map->RemoveFunc("lempty");
  Map->RemoveFunc("lindex");
  Map->RemoveFunc("lappend");
  
}


/*
 * function    : SQI_ListH_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 *
 * output      : none
 * side effect : none
 */
void SQI_ListH_Init(SQI_BuiltInMap *Map)
{
	Map->AddAddOn(ListH_Pkg_name,ListH_Pkg_purpose,ListH_Pkg_author,ListH_Pkg_version,ListH_Pkg_date,SQI_ListH_Remove);
  
	Map->AddFunc("list",
	       ListH_Pkg_name,
	       "Ouput a list whose member are in inputs",
	       "list a_thing another_thing ....",
	       SQI_ListH_List
	       );

	Map->AddFunc("sentence",
	       ListH_Pkg_name,
	       "Ouput a list whose member are in inputs (flatten the list in input)",
	       "sentence a_thing another_thing ....",
	       SQI_ListH_Sentence
	       );
	        
	Map->AddFunc("gseq",
	       ListH_Pkg_name,
	       "Ouput a list of number from FROM to TO. If COUNT specified number will be equally spaced.",
	       "gseq from to (count)",
	       SQI_ListH_GSeq
	       );

	Map->AddFunc("fput",
	       ListH_Pkg_name,
	       "Ouput a list formed by the concatenation of the first element and the second",
	       "fput thing list",
	       SQI_ListH_Fput
	       );

	Map->AddFunc("lput",
	       ListH_Pkg_name,
	       "Ouput a list formed by the concatenation of the second element and the first",
	       "lput thing list",
	       SQI_ListH_Lput
	       );

	Map->AddFunc("reverse",
	       ListH_Pkg_name,
	       "Ouput a list which is the reverse of the argument",
	       "reverse list",
	       SQI_ListH_Reverse
	       );

	Map->AddFunc("llength",
	       ListH_Pkg_name,
	       "Ouput the size of the list passed in argument",
	       "llength list",
	       SQI_ListH_Length
	       );

	Map->AddFunc("lscan",
	       ListH_Pkg_name,
	       "Store the element of the list in various variable (on for each element wanted). Output the rest of the list ",
	       "lscan list \"var1 \"var2 \"var3 ....",
	       SQI_ListH_Scan
	       );

	Map->AddFunc("lempty",
	       ListH_Pkg_name,
	       "Ouput true if the argument is an empty list or not a list",
	       "lempty thing",
	       SQI_ListH_Empty
	       );

	Map->AddFunc("lindex",
	       ListH_Pkg_name,
	       "Output or change the value of the ith element of a list",
	       "lindex list index (new value)",
	       SQI_ListH_Index
	       );
	       
	Map->AddFunc("lappend",
	       ListH_Pkg_name,
	       "Append elements at the end of a list (mutate the list)",
	       "lappend list thing1 thing2 thing3 ...",
	       SQI_ListH_Append
	       );
}

