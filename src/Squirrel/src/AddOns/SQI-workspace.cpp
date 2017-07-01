/*
 * Squirrel project
 *
 * file ?	SQI-workspace.cpp
 * what	?	variable management builtin predicates
 * who	?	jlv
 * when	?	07/28/99
 * last	?	08/09/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include <iostream.h>
#include "SQI-AddOn.h"

// now some infos on the package

char *WorkS_Pkg_name = "Workspace";
char *WorkS_Pkg_purpose = "Variable creation/destruction ...";
char *WorkS_Pkg_author = "[e-]";
char *WorkS_Pkg_version = "0.8";
char *WorkS_Pkg_date = "08/10/99";

/*
 * function    : SQI_WorkS_Make
 * purpose     : Builtin predicate "make"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Make(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"make"));
  else
    {
      SQI_Object *name = (*args)[0];
      SQI_Object *value = (*args)[1];

      if(name->Is()==SQI_KEYWORD)
	{
	  // first we search in the local stack

	  string *var = (dynamic_cast<SQI_Keyword *>(name))->Data();

	  if(squirrel->LocalVar->Exist(var)) // exist, so we store it
	      squirrel->LocalVar->Set(var,value);
	  else // now we check the global var map (but if the govalvar is global it's ok)
	    if(interp->GlobalVar->Exist(var)||squirrel->gvar)
	      {
		// if we store a local value in a global var , the value must be exported to
		// the global var (if not allready in)

		if(value->heap!=interp->GlobalHeap)
		  value->Export(interp->GlobalHeap);
		
		interp->GlobalVar->Set(var,value);
	      }
	    else // unknow var so ....
	      throw(new SQI_Exception(SQI_EX_UNKNOWVAR,"make",var));
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"make","First argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Name
 * purpose     : Builtin predicate "name"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Name(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"name"));
  else
    {
      SQI_Object *name = (*args)[1];
      SQI_Object *value = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  // first we search in the local stack

	  string *var = (dynamic_cast<SQI_Keyword *>(name))->Data();

	  if(squirrel->LocalVar->Exist(var)) // exist, so we store it
	      squirrel->LocalVar->Set(var,value);
	  else // now we check the global var map (but if the govalvar is global it's ok)
	    if(interp->GlobalVar->Exist(var)||squirrel->gvar)
	      {
		// if we store a local value in a global var , the value must be exported to
		// the global var (if not allready in)

		if(value->heap!=interp->GlobalHeap)
		  value->Export(interp->GlobalHeap);
		
		interp->GlobalVar->Set(var,value);
	      }
	    else // unknow var so ....
	      throw(new SQI_Exception(SQI_EX_UNKNOWVAR,"name",var));
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"name","second argument must be a word"));

      return NULL;
    }
}

/*
 * function    : Create_Var
 * purpose     : Create a Variable in the local map and initialize it to zero
 * input       : 
 *
 * SQI_String *var, name of the variable
 * SQI_Squirrel *squirrel, Squirrel executing the code
 *
 * output      : none
 * side effect : none
 */
void Create_Var(SQI_String *var,SQI_Squirrel *squirrel)
{
  string *name = var->Data();

  if(!squirrel->LocalVar->Exist(name))
    {
      SQI_Number *zero = squirrel->LocalHeap->Number(0);
      squirrel->LocalVar->Set(name,zero);
    }
}

/*
 * function    : SQI_WorkS_Local
 * purpose     : Builtin predicate "local"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_WorkS_Local(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  deque<SQI_Object *>::const_iterator i;
  SQI_Object *obj;

  if(!args->size())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"local"));
  
  
  for(i=args->begin();i!=args->end();i++)
    {
      if(*i)
	{
	  obj = *i;
	  switch(obj->Is()) // we do nothing with the argument who aren't string or list
	    {
	    case SQI_KEYWORD:
	      {
		Create_Var(dynamic_cast<SQI_Keyword *>(obj),squirrel);
		break;
	      }
	    case SQI_LIST:
	      {
		SQI_List *l = dynamic_cast<SQI_List *>(obj);
		list<SQI_Object *>::const_iterator *j = l->Iterator();
		list<SQI_Object *>::const_iterator *e = l->End();
		
		while((*j)!=(*e))
		  {
		    if((*(*j))->Is()==SQI_KEYWORD)
		      Create_Var(dynamic_cast<SQI_Keyword *>(*(*j)),squirrel);
		    (*j)++;
		  }
		
		delete j;
		delete e;
	      }
	    }
	}
    }

  return NULL;
}

/*
 * function    : SQI_WorkS_Help
 * purpose     : Builtin predicate "help"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Help(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"help"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);

	  string *str = interp->BuiltInMap->GetFuncInfo(s->Data());

	  if(!str)
	    {
	      // maybe a userfunc
	      if(!interp->UFuncMap->GetFunc(s->Data()))
		throw(new SQI_Exception(SQI_EX_UNKNOWFUNC,"help",s->Data()));
	      else
		cout << s->Data() << " is not a primitive. No help available\n";
	      
	    }
	  else
	  	{
	    	cout << str;
	    	delete str;	
	  	}
	    
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"help","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Thing
 * purpose     : Builtin predicate "thing"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Thing(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"thing"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);
	  SQI_Object *ret = NULL;

	  if(!(ret=squirrel->GetVarValue(interp,s->Data())))
	    throw(new SQI_Exception(SQI_EX_UNKNOWVAR,"thing",s->Data()));
	  else
	    return ret;
	  
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"thing","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Isprim
 * purpose     : Builtin predicate "isprim"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Isprim(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"isprim","need one argument"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);

	  if(!interp->BuiltInMap->GetFunc(s->Data()))		
	    return interp->False;
	  else
	    return interp->True;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"isprim","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Isproc
 * purpose     : Builtin predicate "isproc"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Isproc(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"isproc","need one argument"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);

	  if(!interp->UFuncMap->GetFunc(s->Data()))		
	    return interp->False;
	  else
	    return interp->True;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"isproc","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Ispred
 * purpose     : Builtin predicate "ispred"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Ispred(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ispred","need one argument"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);

	  if(!interp->UFuncMap->GetFunc(s->Data()))
	    {
	      if(!interp->BuiltInMap->GetFunc(s->Data()))
		return interp->False;
	      else
		return interp->True;
	    }
	  else
	    return interp->True;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ispred","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Isvar
 * purpose     : Builtin predicate "isvar"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Isvar(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"isvar"));
  else
    {
      SQI_Object *name = (*args)[0];

      if(name->Is()==SQI_KEYWORD)
	{
	  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);
	  SQI_Object *ret = NULL;

	  if(!squirrel->GetVarValue(interp,s->Data()))
	    return interp->False;
	  else
	    return interp->True;
	  
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"isvar","argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_Erase
 * purpose     : Builtin predicate "erase"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Erase(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"erase","need at least one argument"));
  else
    {
      deque<SQI_Object *>::const_iterator i;
      SQI_Object *name;

      for(i=args->begin();i!=args->end();i++)
	{
	  if((*i))
	    {
	      name = *i;
	      if(name->Is()==SQI_KEYWORD)
		{
		  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);

		  // is it a procedure
		  if(interp->UFuncMap->GetFunc(s->Data()))
		    interp->RemoveFunc(s->Data());
		  else // a var ?
		    if(squirrel->GetVarValue(interp,s->Data()))
		      squirrel->RemoveVar(interp,s->Data());
		    else
		      throw(new SQI_Exception(SQI_EX_DONTKNOW,"erase",s->Data()));
		}
	      else
		if(name->Is()==SQI_LIST)
		  {

		  }
	      // else we do nothing with it
	    }
	}

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_GC
 * purpose     : Builtin predicate "gc"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_GC(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
 	return squirrel->LocalHeap->Number(squirrel->LocalHeap->GC());
}

/*
 * function    : SQI_WorkS_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_WorkS_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("make");
  Map->RemoveFunc("local");
  Map->RemoveFunc("help");
  Map->RemoveFunc("name");
  Map->RemoveFunc("thing");
  Map->RemoveFunc("isproc");
  Map->RemoveFunc("isprim");
  Map->RemoveFunc("ispred");
  Map->RemoveFunc("isvar");
  Map->RemoveFunc("erase");
  Map->RemoveFunc("gc");
}


/*
 * function    : SQI_WorkS_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 *
 * output      : none
 * side effect : none
 */
void SQI_WorkS_Init(SQI_BuiltInMap *Map)
{
  Map->AddAddOn(WorkS_Pkg_name,WorkS_Pkg_purpose,WorkS_Pkg_author,WorkS_Pkg_version,WorkS_Pkg_date,SQI_WorkS_Remove);

  Map->AddFunc("make",
	       WorkS_Pkg_name,
	       "Affect a value to a variable",
	       "make \"myvar myvalue",
	       SQI_WorkS_Make
	       );

  Map->AddFunc("local",
	       WorkS_Pkg_name,
	       "Create a local variable",
	       "local \"myvar \"another ....",
	       SQI_WorkS_Local
	       );

  Map->AddFunc("help",
	       WorkS_Pkg_name,
	       "Display an help on the predicate",
	       "help",
	       SQI_WorkS_Help
	       );

  Map->AddFunc("name",
	       WorkS_Pkg_name,
	       "Affect a value to a variable",
	       "name myvalue \"myvar",
	       SQI_WorkS_Name
	       );

  Map->AddFunc("thing",
	       WorkS_Pkg_name,
	       "Output the value of a variable",
	       "thing \"myvar",
	       SQI_WorkS_Thing
	       );

  Map->AddFunc("is.prim",
	       WorkS_Pkg_name,
	       "Return true if the argument is the name of a primitive",
	       "is.prim \"name",
	       SQI_WorkS_Isprim
	       );

  Map->AddFunc("is.proc",
	       WorkS_Pkg_name,
	       "Return true if the argument is the name of a procedure",
	       "is.proc \"name",
	       SQI_WorkS_Isproc
	       );

  Map->AddFunc("is.pred",
	       WorkS_Pkg_name,
	       "Return true if the argument is the name of a procedure or a primitive",
	       "is.pred \"name",
	       SQI_WorkS_Ispred
	       );

  Map->AddFunc("is.var",
	       WorkS_Pkg_name,
	       "Return true if the argument is the name of a variable",
	       "is.var \"name",
	       SQI_WorkS_Isvar
	       );
  
  Map->AddFunc("erase",
	       WorkS_Pkg_name,
	       "Erase from the workspace the argument (variable/procedure)",
	       "erase name1 name2 ...",
	       SQI_WorkS_Erase
	       );
	       
  Map->AddFunc("gc",
	       WorkS_Pkg_name,
	       "Performe a garbage collection. Output the number of object deleted",
	       "gc",
	       SQI_WorkS_GC
	       );
}

