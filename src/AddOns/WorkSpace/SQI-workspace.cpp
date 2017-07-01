/*
 * Squirrel project
 *
 * file ?	SQI-workspace.cpp
 * what	?	variable management builtin predicates
 * who	?	jlv
 * when	?	07/28/99
 * last	?	11/15/00
 *
 *
 * (c) Kirilla 1999-2000
 */

// First the needed header

#include <Entry.h>
#include <Directory.h>
#include <Path.h>
#include <be/kernel/image.h>

#include <iostream.h>
#include "SQI-AddOn.h"
#include <stdlib.h>

#define STD_ADDON_DIR      "/boot/home/config/add-ons/Squirrel"
#define STD_ADDON_INITFUNC "SQI_AddOn_Init__FP14SQI_BuiltInMapl"

// now some infos on the package

char *WorkS_Pkg_name = "Workspace";
char *WorkS_Pkg_purpose = "Variable creation/destruction ...";
char *WorkS_Pkg_author = "Kirilla";
char *WorkS_Pkg_version = "0.21";
char *WorkS_Pkg_date = "11/15/00";

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
void Create_Var(SQI_Keyword *var,SQI_Squirrel *squirrel)
{
  string *name = var->Data();

  if(!squirrel->LocalVar->Exist(name))
    {
      SQI_Number *zero = new SQI_Number(squirrel->LocalHeap,0);
      squirrel->LocalVar->Set(squirrel,name,zero);
    }
}

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

SQI_Object *SQI_WorkS_Make(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=2)
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
		      squirrel->LocalVar->Set(squirrel,var,value);
		  else // now we check the global var map (but if the govalvar is global it's ok)
		    if(interp->GlobalVar->Exist(var)||squirrel->gvar)
		      {
				// if we store a local value in a global var , the value must be exported to
				// the global var (if not allready in)
	
				if(value->heap==squirrel->LocalHeap  && !squirrel->gvar)
			  		value->Export(interp->GlobalHeap);
			
				interp->GlobalVar->Set(squirrel,var,value);
		      }
		    else // unknow var so we create a global variable
		    	{
		    		if(value->heap==squirrel->LocalHeap && !squirrel->gvar)
			  			value->Export(interp->GlobalHeap);
		    		interp->GlobalVar->Set(squirrel,var,value);		
		    	}
		    	//  throw(new SQI_Exception(SQI_EX_UNKNOWVAR,"make",var));
		}
      else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"make","First argument must be a word"));

      return NULL;
    }
}

/*
 * function    : SQI_WorkS_MakeLocal
 * purpose     : Builtin predicate "make.local"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_MakeLocal(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"make.local"));
  else
    {
      SQI_Object *name = (*args)[0];
      SQI_Object *value = (*args)[1];

      if(name->Is()==SQI_KEYWORD)
		{  
		  	string *var = (dynamic_cast<SQI_Keyword *>(name))->Data();
			Create_Var(dynamic_cast<SQI_Keyword *>(name),squirrel);
			squirrel->LocalVar->Set(squirrel,var,value);
		}
      else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"make.local","First argument must be a word"));

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

SQI_Object *SQI_WorkS_Name(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=2)
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
	      squirrel->LocalVar->Set(squirrel,var,value);
	  else // now we check the global var map (but if the govalvar is global it's ok)
	    if(interp->GlobalVar->Exist(var)||squirrel->gvar)
	      {
		// if we store a local value in a global var , the value must be exported to
		// the global var (if not allready in)

		if(value->heap!=interp->GlobalHeap)
		  value->Export(interp->GlobalHeap);
		
		interp->GlobalVar->Set(squirrel,var,value);
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
SQI_Object *SQI_WorkS_Local(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  long i;
  SQI_Object *obj;

  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"local"));
  
  
  for(i=0;i<args->Length();i++)
    {
    	obj = (*args)[i];
      	if(obj)
		{
		  switch(obj->Is()) // we do nothing with the argument who aren't string or list
		    {
		    case SQI_KEYWORD:
		      {
				Create_Var(dynamic_cast<SQI_Keyword *>(obj),squirrel);
				//cout << *(squirrel->LocalVar->Print()) << "\n";
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

SQI_Object *SQI_WorkS_Help(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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
		      {
				*(interp->Stream->out) << s->Data() << " is not a primitive. No help available\n" << '\0';
		       interp->Stream->Flush();
		      }
		    }
		  else
		  	{
		    	*(interp->Stream->out) << *str << '\0';;
		    	delete str;	
		    	interp->Stream->Flush();
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

SQI_Object *SQI_WorkS_Thing(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_WorkS_Isprim(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_WorkS_Isproc(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_WorkS_Ispred(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_WorkS_Isvar(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()!=1)
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

SQI_Object *SQI_WorkS_Erase(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"erase","need at least one argument"));
  else
    {
      long i;
      SQI_Object *name;

      for(i=0;i<args->Length();i++)
		{
			name = (*args)[i];	
			if(name)
			{
				if(name->Is()==SQI_KEYWORD)
				{
				  SQI_String *s = dynamic_cast<SQI_Keyword *>(name);
		
				  // is it a procedure
				  if(interp->UFuncMap->GetFunc(s->Data()))
				    interp->RemoveFunc(s->Data());
				  else // a var ?
				    if(squirrel->ExistVar(interp,s->Data()))
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

SQI_Object *SQI_WorkS_GC(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
 	return new SQI_Number(squirrel->LocalHeap,interp->Eternal->GC()+
 		interp->Tree->GC() + squirrel->LocalHeap->GC());
}

/*
 * function    : SQI_WorkS_Bind
 * purpose     : Builtin predicate "bind"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Bind(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=3)
	{
		SQI_Keyword *variable = IsKeyword((*args)[0]);
		SQI_Keyword *mode = IsKeyword((*args)[1]);
		
		if(variable && mode)
		{
			TVariable *binding = squirrel->GetVarBinding(interp,variable->Data());
				
			SQI_nBlock *block = IsBlock((*args)[2]);
			if(block) // it's a block to execute
			{
				string *m = mode->Data();
				
				if(*m == "get")
				{
					if(binding->get)
					{
						if(binding->get->block)
							binding->get->block->RemRef();				
						else
						if(binding->get->function)
						{
							list<SQI_Object *>::const_iterator j;
							binding->get->function->RemRef();
							binding->get->function = NULL;
							for(j=binding->get->args->begin();j!=binding->get->args->end();j++)
    							(*j)->RemRef();
    						binding->get->args->clear();
    						binding->get->args = NULL;
						}
						
						block->AddRef();
						binding->get->block = block;
					}		
					else
					{
						// new binding
						TBind *bind = new TBind;
						block->AddRef();
						bind->block = block;
						bind->function = NULL;
						bind->args = NULL;
						bind->data = NULL;
						bind->func = NULL;
						binding->get = bind;	
					}	
				}
				else
				if(*m == "set")
				{
					if(binding->set)
					{
						if(binding->set->block)
							binding->set->block->RemRef();			
						else
						if(binding->set->function)
						{
							list<SQI_Object *>::const_iterator j;
							binding->set->function->RemRef();
							binding->set->function = NULL;
							for(j=binding->set->args->begin();j!=binding->set->args->end();j++)
    							(*j)->RemRef();
    						binding->set->args->clear();
    						binding->set->args = NULL;
						}
						
						block->AddRef();
						binding->set->block = block;
					}		
					else
					{
						// new binding
						TBind *bind = new TBind;
						block->AddRef();
						bind->block = block;
						bind->function = NULL;
						bind->args = NULL;
						bind->data = NULL;
						bind->func = NULL;
						binding->set = bind;	
					}					
				}
				else
				if(*m == "erase")
				{
					if(binding->erase)
					{
						if(binding->erase->block)
							binding->erase->block->RemRef();			
						else
						if(binding->erase->function)
						{
							list<SQI_Object *>::const_iterator j;
							binding->erase->function->RemRef();
							binding->erase->function = NULL;
							for(j=binding->erase->args->begin();j!=binding->erase->args->end();j++)
    							(*j)->RemRef();
    						binding->erase->args->clear();
    						binding->erase->args = NULL;
						}
						
						block->AddRef();
						binding->erase->block = block;
					}		
					else
					{
						// new binding
						TBind *bind = new TBind;
						block->AddRef();
						bind->block = block;
						bind->function = NULL;
						bind->args = NULL;
						bind->data = NULL;
						bind->func = NULL;
						binding->get = bind;	
					}					
				}
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"bind","binding mode must be \"get \"set or \"erase"));	
			}
			else
			{
				SQI_Keyword *name = IsKeyword((*args)[2]);		
					
				// if we have a word, it's a function name	
						
				if(name)
				{				
					list<SQI_Object *> *extra = NULL;
					SQI_Node *call = dynamic_cast<SQI_Node *>(interp->FindCall(name->Data()->c_str()));
					
					call->Export(interp->GlobalHeap);
      		
      				call->AddRef();
										
					// if we have more args , it's extra inputs for the binding
					
					if(args->Length()>4)
					{
						SQI_Object *obj;
						extra = new list<SQI_Object *>;
						for(int i=3;i<args->Length();i++)
						{
							obj = (*args)[i];
							obj->AddRef();
							extra->push_back(obj);	
						}	
					}
					
					// now we set-up the binding
					
					string *m = mode->Data();
					
					if(*m == "get")
					{
						if(binding->get)
						{
							if(binding->get->block)
							{
								binding->get->block->RemRef();			
								binding->get->block = NULL;	
							}	
							else
							if(binding->get->function)
							{
								list<SQI_Object *>::const_iterator j;
								binding->get->function->RemRef();
								binding->get->function = NULL;
								if(binding->get->args)
								{
									for(j=binding->get->args->begin();j!=binding->get->args->end();j++)
		    							(*j)->RemRef();
		    						binding->get->args->clear();
		    						binding->get->args = NULL;
								}
							}
							
							binding->get->function = call;
							binding->get->args = extra;
						}		
						else
						{
							// new binding
							TBind *bind = new TBind;
							call->AddRef();
							bind->block = NULL;
							bind->function = call;
							bind->args = extra;
							bind->data = NULL;
							bind->func = NULL;
							binding->get = bind;	
						}	
					}
					else
					if(*m == "set")
					{
						if(binding->set)
						{
							if(binding->set->block)
								binding->set->block->RemRef();			
							else
							if(binding->set->function)
							{
								list<SQI_Object *>::const_iterator j;
								binding->set->function->RemRef();
								binding->set->function = NULL;
								if(binding->set->args)
								{
									for(j=binding->set->args->begin();j!=binding->set->args->end();j++)
	    								(*j)->RemRef();
	    							binding->set->args->clear();
	    							binding->set->args = NULL;
								}	
							}
	
							binding->set->function = call;
							binding->set->args = extra;
						}		
						else
						{
							// new binding
							TBind *bind = new TBind;
							call->AddRef();
							bind->block = NULL;
							bind->function = call;
							bind->args = extra;
							bind->data = NULL;
							bind->func = NULL;
							binding->set = bind;	
						}					
					}
					else
					if(*m == "erase")
					{
						if(binding->erase)
						{
							if(binding->erase->block)
								binding->erase->block->RemRef();			
							else
							if(binding->erase->function)
							{
								list<SQI_Object *>::const_iterator j;
								binding->erase->function->RemRef();
								binding->erase->function = NULL;
								if(binding->erase->args)
								{
									for(j=binding->erase->args->begin();j!=binding->erase->args->end();j++)
	    								(*j)->RemRef();
	    							binding->erase->args->clear();
	    							binding->erase->args = NULL;
								}	
							}
							
							binding->erase->function = call;
							binding->erase->args = extra;
						}		
						else
						{
							// new binding
							TBind *bind = new TBind;
							call->AddRef();
							bind->block = NULL;
							bind->function = call;
							bind->args = extra;
							bind->data = NULL;
							bind->func = NULL;
							binding->erase = bind;	
						}					
					}
					else
						throw(new SQI_Exception(SQI_EX_INVALIDE,"bind","binding mode must be \"get \"set or \"erase"));
					
					
					// and that's all
				
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"bind","third input must be a word or a block"));
				
			}			
			
			return NULL;
								
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"bind","need 2 words as firsts inputs")); 		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"bind","need at least 3 arguments")); 
}

/*
 * function    : SQI_WorkS_UnBind
 * purpose     : Builtin predicate "unbind"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_UnBind(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==2)
	{
		SQI_Keyword *variable = IsKeyword((*args)[0]);
		SQI_Keyword *mode = IsKeyword((*args)[1]);
		
		if(variable && mode)
		{
			TVariable *binding = squirrel->GetVarBinding(interp,variable->Data());
					
			string *m = mode->Data();
			
			if(*m == "get")
			{
				if(binding->get)
				{
					if(binding->get->block)
					{
						binding->get->block->RemRef();
						binding->get->block = NULL;			
					}	
					else
					if(binding->get->function)
					{
						list<SQI_Object *>::const_iterator j;
						binding->get->function->RemRef();
						binding->get->function = NULL;
						for(j=binding->get->args->begin();j!=binding->get->args->end();j++)
    							(*j)->RemRef();
    					binding->get->args->clear();
    					binding->get->args = NULL;
					}
					
					if(!binding->get->func)
					{
						delete binding->get;
						binding->get = NULL;
					}	
				}			
			}
			else
			if(*m == "set")
			{
				if(binding->set->block)
				{
					binding->set->block->RemRef();
					binding->set->block = NULL;			
				}	
				else
				if(binding->set->function)
				{
					list<SQI_Object *>::const_iterator j;
					binding->set->function->RemRef();
					binding->set->function = NULL;
					for(j=binding->set->args->begin();j!=binding->set->args->end();j++)
    							(*j)->RemRef();
    					binding->set->args->clear();
    					binding->set->args = NULL;
				}
				
				if(!binding->set->func)
				{
					delete binding->set;
					binding->set = NULL;
				}					
			}
			else
			if(*m == "erase")
			{
				if(binding->erase->block)
				{
					binding->erase->block->RemRef();
					binding->erase->block = NULL;			
				}	
				else
				if(binding->erase->function)
				{
					list<SQI_Object *>::const_iterator j;
					binding->erase->function->RemRef();
					binding->erase->function = NULL;
					for(j=binding->erase->args->begin();j!=binding->erase->args->end();j++)
    							(*j)->RemRef();
    					binding->erase->args->clear();
    					binding->erase->args = NULL;
				}
				
				if(!binding->erase->func)
				{
					delete binding->erase;
					binding->erase = NULL;
				}				
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"bind","binding mode must be \"get \"set or \"erase"));	
				
			return NULL;								
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"bind","need 2 words as firsts inputs")); 		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"bind","need two arguments")); 
}

bool LoadAddOn(SQI_Interp *interp,BEntry *entry)
{
	if(entry->InitCheck()==B_NO_ERROR)
	{
		if(!entry->Exists())
			return false;
			
		string name;	
		char leaf[B_FILE_NAME_LENGTH];
		BPath path;
		
		entry->GetName(leaf);	
		entry->GetPath(&path);	
		
		name = leaf;
		
		name.erase(name.find(".so"));	
				
		image_id addon_image;
		void (*InitFunc)(SQI_BuiltInMap *Map,image_id addon_id);
			
		// we check if we don't have it allready
		
		if(!interp->BuiltInMap->ExistAddOn(&name))
		{			
			if((addon_image = load_add_on(path.Path()))>0)
			{
				if(get_image_symbol(addon_image,STD_ADDON_INITFUNC,B_SYMBOL_TYPE_TEXT,(void **)&InitFunc)==B_NO_ERROR)	
					(*InitFunc)(interp->BuiltInMap,addon_image);
				else
					return false;	
			}		
			else
				return false;
		}
		else
			return false;
			
		return true;				
	}
	else
		return false;		
}

bool LoadAddOn(SQI_Interp *interp,const string *name)
{
	BEntry *entry;
	string path(STD_ADDON_DIR);
	
	path += "/";
	path += *name;
	path += ".so";
				
	entry = new BEntry(path.c_str());
	if(entry->InitCheck()==B_NO_ERROR)
	{
		if(!entry->Exists())
		{
			fprintf(stderr,"Failed loading addon : %s\n",path.c_str());
			delete entry;
			return false;		
		}
		
		image_id addon_image;
		void (*InitFunc)(SQI_BuiltInMap *Map,image_id addon_id);
	
		delete entry;	
		
		// we check if we don't have it allready
		
		if(!interp->BuiltInMap->ExistAddOn(name))
		{			
			if((addon_image = load_add_on(path.c_str()))>0)
			{
				if(get_image_symbol(addon_image,STD_ADDON_INITFUNC,B_SYMBOL_TYPE_TEXT,(void **)&InitFunc)==B_NO_ERROR)	
				{
					(*InitFunc)(interp->BuiltInMap,addon_image);
					fprintf(stderr,"succed loading addon : %s\n",path.c_str());
				}
				else
				{
					fprintf(stderr,"Failed loading addon : %s (STD_ADDON_INITFUNC missing)\n",path.c_str());
					return false;	
				}	
			}		
			else
			{
				fprintf(stderr,"Failed loading addon : %s\n",path.c_str());
				return false;
			}
		}
		else
		{
			fprintf(stderr,"already loaded addon : %s\n",path.c_str());
			return false;
		}	
			
		return true;				
	}
	else
	{
		fprintf(stderr,"Failed loading addon : %s\n",path.c_str());
		delete entry;
		return false;
	}		
}

/*
 * function    : SQI_WorkS_Use
 * purpose     : Builtin predicate "use"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_Use(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"use","need at least one argument"));
  else
    {
    	SQI_Keyword *word = IsKeyword((*args)[0]);
    	if(word)
    	{
    		bool ret = true;
    		string *str = word->Data();
    		
    		if(*str == "all") // we load them all
    		{
    			BDirectory dir(STD_ADDON_DIR);
	
				if(dir.InitCheck()==B_OK)
				{
					BEntry entry;
					BPath path;
					struct stat st;
					string str;
					
					while(dir.GetNextEntry(&entry,true)!=B_ENTRY_NOT_FOUND)
					{
						entry.GetStat(&st);
						if(!S_ISDIR(st.st_mode)) // we take only the regular (or link) files
							ret = ret & LoadAddOn(interp,&entry);		
					}			
				}	
				else
					ret = false;			
    		}
    		else
    			throw(new SQI_Exception(SQI_EX_INVALIDE,"use","input must be the word \"all"));	
    		
    		if(ret)
    			return interp->True;
    		else
    			return interp->False;		
    	}
    	else
    	{
    		bool ret = true,tm;
    		int i,max;
    		SQI_String *str;
    		SQI_Object *obj;
    		
    		max = args->Length();
    		    			
    		for(i=0;i<max;i++)
			{
		    	obj = (*args)[i];
		        if(obj)
		      	{
		      		str = IsString(obj);
		      		if(str)
		      		{
		      			tm = LoadAddOn(interp,str->Data());	
		      			ret = ret && tm;	
		      		} 
		        }
			}
						
    		if(ret)
    			return interp->True;
    		else
    			return interp->False;				
				    		
    	}	
    }		
}

/*
 * function    : SQI_WorkS_EnvList
 * purpose     : Builtin predicate "env.list"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_EnvList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	SQI_List *lst = new SQI_List(squirrel->LocalHeap);
	int i=0;	
	string str;
	
	while(environ[i])
	{
		str = environ[i++];
		str.erase(str.find("="));
		lst->Add2End(new SQI_Keyword(squirrel->LocalHeap,&str));
	}	
	  
	return lst;
}

/*
 * function    : SQI_WorkS_EnvGet
 * purpose     : Builtin predicate "env.get"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_EnvGet(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"env.get","need one argument"));
  	else
    {
    	SQI_Keyword *word = IsKeyword((*args)[0]);
    	if(word)
    	{
    		string *str = word->Data();
			char *value;
			
			value = getenv(str->c_str());
			
			if(value)
			{
				return new SQI_String(squirrel->LocalHeap,value);	
			}
			else    		
    			throw(new SQI_Exception(SQI_EX_ERROR,"env.get","env variable not found"));	
    	}
    	else
    		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"env.get","need a word as input"));
    }		
}

/*
 * function    : SQI_WorkS_EnvExist
 * purpose     : Builtin predicate "env.exist"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_EnvExist(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"env.exists","need one argument"));
  	else
    {
    	SQI_Keyword *word = IsKeyword((*args)[0]);
    	if(word)
    	{
    		string *str = word->Data();
			char *value;
			
			value = getenv(str->c_str());
			
			if(value)
				return interp->True;	
			else    		
    			return interp->False;	
    	}
    	else
    		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"env.exists","need a word as input"));
    }		
}

/*
 * function    : SQI_WorkS_EnvPut
 * purpose     : Builtin predicate "env.set"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_WorkS_EnvSet(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=2)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"env.set","need two arguments"));
  	else
    {
    	SQI_Keyword *word = IsKeyword((*args)[0]);
    	if(word)
    	{
    		string *str = word->Data();
			SQI_Object *val = (*args)[1];
			string *value;
			string senv;	
			
			value = val->Print(squirrel->precision);
			
			senv = *str;
			senv += "=";
			senv += *value;	
			
			putenv(senv.c_str());	
			
			delete value;
			
			return NULL;			
    	}
    	else
    		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"env.set","need a word as first input"));
    }		
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
  Map->RemoveFunc("make.local");
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
  Map->RemoveFunc("bind");
  Map->RemoveFunc("unbind");
  Map->RemoveFunc("use");
  Map->RemoveFunc("env.list");
  Map->RemoveFunc("env.get");
  Map->RemoveFunc("env.set");
  Map->RemoveFunc("env.exist");
}

/*
 * function    : SQI_AddOn_Init
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
  Map->AddAddOn(addon_id,WorkS_Pkg_name,WorkS_Pkg_purpose,WorkS_Pkg_author,WorkS_Pkg_version,WorkS_Pkg_date,SQI_WorkS_Remove);

  Map->AddFunc("make",
	       WorkS_Pkg_name,
	       "Affect a value to a variable",
	       "make \"myvar myvalue",
	       SQI_WorkS_Make
	       );
	       
  Map->AddFunc("make.local",
	       WorkS_Pkg_name,
	       "Affect a value to a local variable (without having to define the variable as local before)",
	       "make.local \"myvar myvalue",
	       SQI_WorkS_MakeLocal
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
	       
  Map->AddFunc("bind",
	       WorkS_Pkg_name,
	       "Bind a variable to an event",
	       "bind variable event block | word args ...",
	       SQI_WorkS_Bind
	       );
	       
  Map->AddFunc("unbind",
	       WorkS_Pkg_name,
	       "Unbind a variable to an event",
	       "bind variable event",
	       SQI_WorkS_UnBind
	       );
	       
  Map->AddFunc("use",
	       WorkS_Pkg_name,
	       "Load Add-On(s)",
	       "use word | (string)+",
	       SQI_WorkS_Use
	       );	   
	       
  Map->AddFunc("env.list",
	       WorkS_Pkg_name,
	       "Output a list of all the environment variables",
	       "env.list",
	       SQI_WorkS_EnvList
	       );

  Map->AddFunc("env.get",
	       WorkS_Pkg_name,
	       "Get the value of a environment variable",
	       "env.get word",
	       SQI_WorkS_EnvGet
	       );
	       
  Map->AddFunc("env.exists",
	       WorkS_Pkg_name,
	       "Check if a environment variable exists",
	       "env.exists word",
	       SQI_WorkS_EnvExist
	       );
	       
  Map->AddFunc("env.set",
	       WorkS_Pkg_name,
	       "Add or replace the value of an environment variable",
	       "env.set word thing",
	       SQI_WorkS_EnvSet
	       );	       	       	       	           
}

