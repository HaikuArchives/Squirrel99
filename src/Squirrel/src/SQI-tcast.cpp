/*
 * Squirrel project
 *
 * file ?	SQI-tcast.cpp
 * what	?	Test and cast the type of an object
 * who	?	jlv
 * when	?	08/07/99
 * last	?	02/14/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-Obj.h"

/*
 * function    : IsNode
 * purpose     : Return the object if of type SQI_Node
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Node *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Node *IsNode(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NODE)
      return NULL;
    else
    	return dynamic_cast<SQI_Node *>(obj);
}


/*
 * function    : IsBlock
 * purpose     : Return the object if of type SQI_nBlock
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_nBlock *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_nBlock *IsBlock(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NODE)
      return NULL;
    else
    {
    	SQI_Node *node = dynamic_cast<SQI_Node *>(obj);
      	if(node->IsA()!=SQI_TNODE_BLOCK)
      		return NULL;
      	else
      		return dynamic_cast<SQI_nBlock *>(node);
    }		
}

/*
 * function    : IsUFunc
 * purpose     : Return the object if of type SQI_nUfunc
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_nUfunc *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_nUfunc *IsUFunc(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NODE)
      return NULL;
    else
    {
    	SQI_Node *node = dynamic_cast<SQI_Node *>(obj);
      	if(node->IsA()!=SQI_TNODE_USERFUN)
      		return NULL;
      	else
      		return dynamic_cast<SQI_nUfunc *>(node);
    }		
}

/*
 * function    : IsBool
 * purpose     : Return the object if of type SQI_Number (bool)
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Number *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Number *IsBool(SQI_Object *obj)
{
  if(!obj)
    return NULL;	
	

  if(obj->Is()!=SQI_NUMBER)
    return NULL;
    else
    {
    	SQI_Number *num = dynamic_cast<SQI_Number *>(obj);
      	switch(num->IsA())
      	{
      		case SQI_BOOL:
      			return num;
      		default:
      			return NULL;	
      	}
    }
}

/*
 * function    : IsInteger
 * purpose     : Return the object if of type SQI_Number (int,long)
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Number *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Number *IsInteger(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	

	if(obj->Is()!=SQI_NUMBER)
      return NULL;
    else
    {
    	SQI_Number *num = dynamic_cast<SQI_Number *>(obj);
      	switch(num->IsA())
      	{
	case SQI_BOOL:
	case SQI_INT:
	case SQI_LONG:
	  return num;
	default:
	  return NULL;	
      	}
    }
}

/*
 * function    : IsFloat
 * purpose     : Return the object if of type SQI_Number (float,double)
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Number *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Number *IsFloat(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NUMBER)
      return NULL;
    else
    {
    	SQI_Number *num = dynamic_cast<SQI_Number *>(obj);
      	switch(num->IsA())
      	{
      		case SQI_INT:
      		case SQI_LONG:
      			return NULL;
      		default:
      			return num;	
      	}
    }
}

/*
 * function    : IsNumber
 * purpose     : Return the object if of type SQI_Number
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Number *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Number *IsNumber(SQI_Object *obj)
{	
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NUMBER)
    	return NULL;
    else
      return dynamic_cast<SQI_Number *>(obj);
}

/*
 * function    : IsString
 * purpose     : Return the object if of type SQI_String
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_String *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_String *IsString(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_STRING)
    	return NULL;
    else
		return dynamic_cast<SQI_String *>(obj);
}

/*
 * function    : IsKeyword
 * purpose     : Return the object if of type SQI_Keyword
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_Keyword *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_Keyword *IsKeyword(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_KEYWORD)
    	return NULL;
    else
		return dynamic_cast<SQI_Keyword *>(obj);
}

/*
 * function    : IsList
 * purpose     : Return the object if of type SQI_List
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_List *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_List *IsList(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_LIST)
    	return NULL;
    else
		return dynamic_cast<SQI_List *>(obj);
}

/*
 * function    : IsExpr
 * purpose     : Return the object if of type SQI_nExpr
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_nExpr *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_nExpr *IsExpr(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NODE)
      return NULL;
    else
    {
    	SQI_Node *node = dynamic_cast<SQI_Node *>(obj);
      	if(node->IsA()!=SQI_TNODE_EXPR)
      		return NULL;
      	else
      		return dynamic_cast<SQI_nExpr *>(node);
    }		
}

/*
 * function    : IsVarRef
 * purpose     : Return the object if of type SQI_nExpr
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_nExpr *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_nVarref *IsVarRef(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_NODE)
      return NULL;
    else
    {
    	SQI_Node *node = dynamic_cast<SQI_Node *>(obj);
      	if(node->IsA()!=SQI_TNODE_VARREF)
      		return NULL;
      	else
      		return dynamic_cast<SQI_nVarref *>(node);
    }		
}

/*
 * function    : IsThePointer
 * purpose     : Return the object if of type SQI_ThePointer
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : SQI_nExpr *, or NULL if the object is not of the correct type
 * side effect : none
 */
SQI_ThePointer *IsThePointer(SQI_Object *obj)
{
	if(!obj)
		return NULL;	
	
	if(obj->Is()!=SQI_THEPOINTER)
      return NULL;
    else
    	return dynamic_cast<SQI_ThePointer *>(obj);		
}


/*
 * function    : Number2Bool
 * purpose     : Return a bool value of the object
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : bool
 * side effect : none
 */
bool Number2Bool(SQI_Number *obj)
{
	switch(obj->IsA())
	{
		case SQI_BOOL:
			return obj->bData();
		case SQI_INT:
			return static_cast<bool>(obj->iData());
		case SQI_LONG:
			return static_cast<bool>(obj->lData());
		case SQI_FLOAT:
			return static_cast<bool>(obj->fData());
		case SQI_DOUBLE:
			return static_cast<float>(obj->dData());				
	}	
}

/*
 * function    : Number2Int
 * purpose     : Return a int value of the object
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : int
 * side effect : none
 */
int Number2Int(SQI_Number *obj)
{
	switch(obj->IsA())
	{
		case SQI_BOOL:
			return static_cast<int>(obj->bData());
		case SQI_INT:
			return obj->iData();
		case SQI_LONG:
			return static_cast<int>(obj->lData());
		case SQI_FLOAT:
			return static_cast<int>(obj->fData());
		case SQI_DOUBLE:
			return static_cast<int>(obj->dData());				
	}	
}

/*
 * function    : Number2Long
 * purpose     : Return a long value of the object
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : long
 * side effect : none
 */
long Number2Long(SQI_Number *obj)
{
	switch(obj->IsA())
	{
		case SQI_BOOL:
			return static_cast<long>(obj->bData());
		case SQI_INT:
			return static_cast<long>(obj->iData());
		case SQI_LONG:
			return obj->lData();
		case SQI_FLOAT:
			return static_cast<long>(obj->fData());
		case SQI_DOUBLE:
			return static_cast<long>(obj->dData());				
	}	
}

/*
 * function    : Number2Float
 * purpose     : Return a float value of the object
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : float
 * side effect : none
 */
float Number2Float(SQI_Number *obj)
{
	switch(obj->IsA())
	{
		case SQI_BOOL:
			return static_cast<float>(obj->bData());
		case SQI_INT:
			return static_cast<float>(obj->iData());
		case SQI_LONG:
			return static_cast<float>(obj->lData());
		case SQI_FLOAT:
			return obj->fData();
		case SQI_DOUBLE:
			return static_cast<float>(obj->dData());				
	}	
}

/*
 * function    : Number2Double
 * purpose     : Return a double value of the object
 * input       : 
 *
 * SQI_Object *obj, the object to test
 *
 * output      : float
 * side effect : none
 */
double Number2Double(SQI_Number *obj)
{
	switch(obj->IsA())
	{
		case SQI_BOOL:
			return static_cast<double>(obj->bData());
		case SQI_INT:
			return static_cast<double>(obj->iData());
		case SQI_LONG:
			return static_cast<double>(obj->lData());
		case SQI_FLOAT:
			return static_cast<double>(obj->fData());
		case SQI_DOUBLE:
			return obj->dData();				
	}	
}
