/*
 * Squirrel project
 *
 * file ?	SQI-interp.cpp
 * what	?	Logo Interpreter class
 * who	?	jlv
 * when	?	07/27/99
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-varmap.h"
#include "SQI-builtin.h"
#include "SQI-ufunc.h"
#include "SQI-interp.h"
#include "SQI-squirrel.h"
#include <stack>

stack<char *> TheFilenames;

/*
 * function    : SQI_Interp
 * purpose     : initialise the interpreter
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Interp::SQI_Interp()
{
  Stream = NULL;
  Eternal = new SQI_Heap(SQI_ETERNAL);         
  Tree = new SQI_Heap(SQI_ETERNAL); 
  GlobalHeap = new SQI_Heap();
  GlobalVar = new SQI_VarMap();     	
  BuiltInMap = new SQI_BuiltInMap();
  UFuncMap = new SQI_UFuncMap();
  
  // now we create true and false value

  True = new SQI_Number(Eternal,true);
  False = new SQI_Number(Eternal,false);
  True->AddRef();
  False->AddRef();
  
  // now we set up the Instantiation function
  
  AddArchive(SQI_NUMBER,InstantiateANumber);
  AddArchive(SQI_STRING,InstantiateAString);
  AddArchive(SQI_KEYWORD,InstantiateAKeyword);
  AddArchive(SQI_LIST,InstantiateAList);
  AddArchive(SQI_IMAGE,InstantiateAnImage);
  
  
  // we init the member of the message object
  InitTranslator();
  
  Pool = new SQI_Pool();
}

/*
 * function    : ~SQI_Interp
 * purpose     : destroy the interpreter
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Interp::~SQI_Interp()
{
	UnInitMessage();
	UnInitImage();	
	
	delete GlobalVar;
	
	//cout << "after delete globalvar\n";		
	
	delete Pool;		// first delete the pool of squirrel	
	
	//cout << "after delete pool\n";	
		
	GlobalVar = NULL;
  	delete UFuncMap;
  	UFuncMap = NULL;
  	
  	//cout << "after delete ufuncmap\n";

	delete Tree;        // always delete the heap tree before the eternal heap	
  	Tree = NULL;	
  	delete GlobalHeap;
  	GlobalHeap = NULL;
  	delete Eternal;     // eternal object last
	Eternal = NULL;
	
	//cout << "delete builtinmap\n";	
	
	delete BuiltInMap;
  	BuiltInMap = NULL;
}

/*
 * function    : IsBuiltIn
 * purpose     : Return the function pointer for a builtin function or NULL if it's not a builtin
 * input       : 
 *
 * string *key, name of the function
 *
 * output      : BuiltInFunc, pointer to the function , or NULL if none find
 * side effect : none
 */
SQI_Object *(*SQI_Interp::IsBuiltIn(string *funcname))(SQI_Args *,SQI_Squirrel *,SQI_Interp *)
{
  	return BuiltInMap->GetFunc(funcname);
}

/*
 * function    : IsUFunc
 * purpose     : Return the function pointer for a user function or NULL if it's not a ufunc
 * input       : 
 *
 * string *key, name of the function
 *
 * output      : SQI_Defunc *, pointer to the function , or NULL if none find
 * side effect : none
 */
SQI_Defunc *SQI_Interp::IsUFunc(string *funcname)
{
  return UFuncMap->GetFunc(funcname);
}

/*
 * function    : FindCall
 * purpose     : Make a node for a function call
 * input       : 
 *
 * char *cmd, name of the function
 *
 * output      : SQI_Node *, the node created
 * side effect : none
 */
SQI_Node *SQI_Interp::FindCall(const char *cmd)
{
 SQI_nBuiltin *built;
 SQI_Defunc *func;
 SQI_Object *(*ptr)(SQI_Args *,SQI_Squirrel *,SQI_Interp *);
 string scmd(cmd);
    
 if(!(ptr=IsBuiltIn(&scmd)))
   {
     if((func=IsUFunc(&scmd)))
       return new SQI_nUfunc(Tree,&scmd,func); // the function is allready define
     else
	 	return new SQI_nUnknow(Tree,&scmd); // the function is not yet define
   }
 else
   {
     built = new SQI_nBuiltin(Tree,NULL);
     built->function = ptr;
     return built;
   }
}

/*
 * function    : RemoveFunc
 * purpose     : Remove a user func from the function map
 * input       : 
 *
 * string *name, name of the function
 *
 * output      : none
 * side effect : none
 */
void SQI_Interp::RemoveFunc(string *name)
{
  // first we remove the function from the map
  UFuncMap->RemoveFunc(name);
  // now we seek all the BLG_nufunc node to
  // see if they call this function and in this case
  // we need to set the direct pointer to NULL
  
  // for that we search in the Tree heap

  SQI_Object *current = Tree->Top();
  SQI_Node *node;
  SQI_nUfunc *func;

  while(current)
    {
      if(current->Is()==SQI_NODE)
		{
	  		node = dynamic_cast<SQI_Node *>(current);
	
	  		if(node->IsA()==SQI_TNODE_USERFUN)
	    		{
	      			func = dynamic_cast<SQI_nUfunc *>(node);

	      			if(*(func->name) == (*name))
						func->func = NULL;
	    		}

			}

      current = current->next;
    }
}

/*
 * function    : SQI_Pool
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Pool::SQI_Pool()
	:BLocker()
{	
}

/*
 * function    : ~SQI_Pool
 * purpose     : Destructor (Delete all the Squirrel)
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Pool::~SQI_Pool()
{
	SQI_Squirrel *sqi;
		
	while(!Pool.empty())
	{
		sqi = Pool.top();
		Pool.pop();
		delete sqi;	
	}	
}

/*
 * function    : Get
 * purpose     : Reuse a Squirrel (create one if no reuse possible)
 * input       :
 *
 * SQI_Interp *interpreter, interpreter
 *
 * output      : SQI_Squirrel *, the squirrel
 * side effect : none
 */
SQI_Squirrel *SQI_Pool::Get(SQI_Interp *interpreter)
{
	SQI_Squirrel *sqi;

	Lock();
	if(!Pool.empty())
	{
		sqi = Pool.top();
		Pool.pop();
		Unlock();	
	}	
	else
	{
		Unlock();
		sqi = new SQI_Squirrel();
  		sqi->interpreter = interpreter;
	}	
	
	return sqi;	
}

/*
 * function    : Put
 * purpose     : Make a Squirrel ready to be reused
 * input       :
 *
 * SQI_Squirrel *squirrel, squirrel to reuse
 *
 * output      : none
 * side effect : Clean the Squirrel
 */
void SQI_Pool::Put(SQI_Squirrel *squirrel)
{
	Lock();
	squirrel->Clean();
	Pool.push(squirrel);
	//delete squirrel;	
	Unlock();	
}
