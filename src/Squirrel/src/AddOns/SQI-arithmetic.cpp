/*
 * Squirrel project
 *
 * file ?	SQI-arithmetic.cpp
 * what	?	Arithmetic builtin predicates
 * who	?	jlv
 * when	?	07/28/99
 * last	?	08/05/99
 *
 *
 * (c) electron Technology 1999
 */

// First the needed header

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "SQI-AddOn.h"

// now some infos on the package

char *Arith_Pkg_name = "Arithmetic";
char *Arith_Pkg_purpose = "Mathematics builtin";
char *Arith_Pkg_author = "[e-]";
char *Arith_Pkg_version = "0.4";
char *Arith_Pkg_date = "08/04/99";

/*
 * function    : SQI_Arith_Max
 * purpose     : Builtin predicate "max"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Max(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"max","need at least 1 arguments"));
  else
    {
      // we jump any object which is not a number

      deque<SQI_Object *>::const_iterator i;

      // first we need to figure where is the first number

      SQI_Object *max = NULL;
      SQI_Object *obj,*tmp = NULL;

      for(i=args->begin();i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      max = obj;
	      break;
	    }
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Max();
		if(tmp)
		  {
		    max = tmp;
		    break;
		  }
	      }
	}
      
      for(i++;i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      if (*max<obj)
		  max = obj;
	    }
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Max();
		if(tmp)
		  {
		    if (*max<tmp)
		      max = tmp;
		  }
	      }
	}

      if(!max)
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"max","no number find in the arguments"));

      return max->Clone(squirrel->LocalHeap);
    }
}

/*
 * function    : SQI_Arith_Min
 * purpose     : Builtin predicate "min"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Min(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"max","need at least 1 argument"));
  else
    {
      // we jump any object which is not a number

      deque<SQI_Object *>::const_iterator i;

      // first we need to figure where is the first number

      SQI_Object *min = NULL;
      SQI_Object *obj,*tmp = NULL;

      for(i=args->begin();i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      min = obj;
	      break;
	    }
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Min();
		if(tmp)
		  {
		    min = tmp;
		    break;
		  }
	      }
	}
      
      for(i++;i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      if (*min>obj)
		  min = obj;
	    }
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Min();
		if(tmp)
		  {
		    if (*min>tmp)
		      min = tmp;
		  }
	      }
	}

      if(!min)
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"min","no number find in the arguments"));

      return min->Clone(squirrel->LocalHeap);
    }
}

/*
 * function    : SQI_Arith_Avg
 * purpose     : Builtin predicate "avg"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Avg(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"avg","need at least 1 arguments"));
  else
    {
      // we jump any object which is not a number

      deque<SQI_Object *>::const_iterator i;

      // first we need to figure where is the first number

      SQI_Object *sum = NULL;
      SQI_Object *obj,*tmp = NULL;
      int num = 0;

      for(i=args->begin();i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      sum = obj->Clone();
	      num++;
	      break;
	    }
	  else
	  if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Sum(&num);
		if(tmp)
		  {
		    sum = tmp;
		    break;
		  }
	      }
	  
	}

      int lnum;
      
      for(i++;i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    {
	      *sum += obj;
	      num++;
	    }
	   else
	     if(obj->Is()==SQI_LIST)
	       {
		 tmp = (dynamic_cast<SQI_List *>(obj))->Sum(&lnum);
		 if(tmp)
		   {
		     *sum += tmp;
		     num+=lnum;
		   }
	       }
	}

      if(!sum)
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"avg","no number find in the arguments"));

      return (*sum/=num);
    }
}

/*
 * function    : SQI_Arith_Incr
 * purpose     : Builtin predicate "incr"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Incr(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"incr","need at least 1 argument"));
  else
    {
      SQI_Keyword *num = IsKeyword((*args)[0]);
 	  SQI_Object *val; 
   
  	  if(!num)
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"incr","the first element must be a variable name"));

      // we get the variable value

      string *var = num->Data();

      if(!(val = squirrel->GetVarValue(interp,var)))
		throw(new SQI_Exception(SQI_EX_UNKNOWVAR,"incr",var));

      // if we have a 2nd argument , we add this value to the first one

      if(args->size()>=2)
	{
	  SQI_Number *inc = IsNumber((*args)[1]);

	  if(!inc)
	    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"incr","the increment must be a number"));

	  if((val->REF>1)||(val->IsEternal()))
	    {
	      SQI_Number *value = dynamic_cast<SQI_Number *>(val->Clone(squirrel->LocalHeap));
	      (*value)+=inc;
	      squirrel->SetVarValue(interp,var,value);
	      return NULL;
	    }
	  else
	    {
	      (*val)+=inc;
	      return NULL;
	    }
	}
      else
	{
	  // we add only one to the value
	  // if the object had more than one ref , we clone it
	  if((val->REF>1)||val->IsEternal())
	    {
	      SQI_Number *value = dynamic_cast<SQI_Number *>(val->Clone(squirrel->LocalHeap));
	      (*value)+=1;
	      squirrel->SetVarValue(interp,var,value);
	      return NULL;
	    }
	  else
	    {
	      (*val)+=1;
	      return NULL;
	    }
	}
    }
}

/*
 * function    : SQI_Arith_Sum
 * purpose     : Builtin predicate "sum"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Sum(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"sum","need at least 1 arguments"));
  else
    {
      // we jump any object which is not a number

      deque<SQI_Object *>::const_iterator i;

      // first we need to figure where is the first number

      SQI_Object *sum = squirrel->LocalHeap->Number(0);
      SQI_Object *obj,*tmp = NULL;
      int num = 0;

      for(i=args->begin();i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    *sum += obj;
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Sum(&num);
		if(tmp)
		  {
		    *sum += tmp;
		    delete tmp;
		  }
	      }
	}
      
      return sum;
    }
}

/*
 * function    : SQI_Arith_Difference
 * purpose     : Builtin predicate "difference"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Difference(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"difference","need 2 arguments"));
  else
    {
      SQI_Object *ag1 = (*args)[0];
      SQI_Object *ag2 = (*args)[1];

      if((ag1->Is()==ag2->Is())&&(ag1->Is()==SQI_NUMBER))
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *right = dynamic_cast<SQI_Number *>(ag2);
	  
	  SQI_Object *diff = *left - right;
	  
	  diff->Export(squirrel->LocalHeap,SQI_NORMAL);
	  
	  return diff;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"difference","need 2 numbers"));
      
    }
}

/*
 * function    : SQI_Arith_Minus
 * purpose     : Builtin predicate "minus"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Minus(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"minus","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];
      
      if(ag1->Is()==SQI_NUMBER)
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>((*args)[0]);
	  
	  SQI_Number *diff = dynamic_cast<SQI_Number *>(left->Clone(squirrel->LocalHeap));

	  diff->Neg();
      	  
	  return diff;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"minus","need 1 numbers"));
      
    }
}

/*
 * function    : SQI_Arith_Product
 * purpose     : Builtin predicate "product"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Product(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"product","need at least 2 arguments"));
  else
    {
      // we jump any object which is not a number

      deque<SQI_Object *>::const_iterator i;

      // first we need to figure where is the first number

      SQI_Object *prod = squirrel->LocalHeap->Number(1);
      SQI_Object *obj,*tmp = NULL;
      int num = 0;

      for(i=args->begin();i!=args->end();i++)
	{
	  obj = *i;
	  if(obj->Is()==SQI_NUMBER)
	    *prod *= obj;
	  else
	    if(obj->Is()==SQI_LIST)
	      {
		tmp = (dynamic_cast<SQI_List *>(obj))->Product();
		if(tmp)
		  {
		    *prod *= tmp;
		    delete tmp;
		  }
	      }
	}
      
      return prod;
    }
}

/*
 * function    : SQI_Arith_Quotient
 * purpose     : Builtin predicate "quotient"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Quotient(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"quotient","need 2 arguments"));
  else
    {
      SQI_Object *ag1 = (*args)[0];
      SQI_Object *ag2 = (*args)[1];

      if((ag1->Is()==ag2->Is())&&(ag1->Is()==SQI_NUMBER))
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *right = dynamic_cast<SQI_Number *>(ag2);
	  
	  SQI_Object *diff = *left / right;
	  
	  diff->Export(squirrel->LocalHeap,SQI_NORMAL);
	  
	  return diff;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"quotient","need 2 numbers"));
      
    }
}

/*
 * function    : SQI_Arith_Modulo
 * purpose     : Builtin predicate "modulo"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Modulo(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"modulo","need 2 arguments"));
  else
    {
      SQI_Object *ag1 = (*args)[0];
      SQI_Object *ag2 = (*args)[1];

      if((ag1->Is()==ag2->Is())&&(ag1->Is()==SQI_NUMBER))
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *right = dynamic_cast<SQI_Number *>(ag2);

	  if(((left->IsA()==SQI_INT)||(left->IsA()==SQI_LONG))&&((right->IsA()==SQI_INT)||(right->IsA()==SQI_LONG)))
	    {
	      SQI_Object *diff = *left % right;
	      
	      diff->Export(squirrel->LocalHeap,SQI_NORMAL);
	      
	      return diff;
	    }
	  else
	    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"modulo","need 2 integers"));
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"modulo","need 2 integers"));
      
    }
}

/*
 * function    : SQI_Arith_Int
 * purpose     : Builtin predicate "int"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Int(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"int","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

      if(ag1->Is()==SQI_NUMBER)
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *ret = NULL;

	  switch(left->IsA())
	    {
	    case SQI_INT:
	    case SQI_LONG:
	      {
		ret = left;
		break;
	      }
	    case SQI_FLOAT:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<int>(left->fData()));
		break;
	      }
	    case SQI_DOUBLE:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<long>(left->dData()));
		break;
	      }
	    }

	  return ret;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"int","need 1 number"));
      
    }
}

/*
 * function    : SQI_Arith_Floor
 * purpose     : Builtin predicate "floor"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Floor(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"floor","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

      if(ag1->Is()==SQI_NUMBER)
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *ret = NULL;

	  switch(left->IsA())
	    {
	    case SQI_INT:
	    case SQI_LONG:
	      {
		ret = left;
		break;
	      }
	    case SQI_FLOAT:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<int>(floor(left->fData())));
		break;
	      }
	    case SQI_DOUBLE:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<long>(floor(left->dData())));
		break;
	      }
	    }

	  return ret;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"floor","need 1 number"));
      
    }
}

/*
 * function    : SQI_Arith_Ceil
 * purpose     : Builtin predicate "ceil"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Ceil(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ceil","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

      if(ag1->Is()==SQI_NUMBER)
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *ret = NULL;

	  switch(left->IsA())
	    {
	    case SQI_INT:
	    case SQI_LONG:
	      {
		ret = left;
		break;
	      }
	    case SQI_FLOAT:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<int>(ceil(left->fData())));
		break;
	      }
	    case SQI_DOUBLE:
	      {
		ret = squirrel->LocalHeap->Number(static_cast<long>(ceil(left->dData())));
		break;
	      }
	    }

	  return ret;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ceil","need 1 number"));
      
    }
}

/*
 * function    : SQI_Arith_Sqrt
 * purpose     : Builtin predicate "sqrt"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Sqrt(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"sqrt","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

      if(ag1->Is()==SQI_NUMBER)
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  SQI_Number *ret = NULL;

	  switch(left->IsA())
	    {
	    case SQI_INT:
	      {
		ret = squirrel->LocalHeap->Number(sqrt(static_cast<double>(left->iData())));
		break;
	      }
	    case SQI_LONG:
	      {
		ret = squirrel->LocalHeap->Number(sqrt(static_cast<double>(left->lData())));
		break;
	      }
	    case SQI_FLOAT:
	      {
		ret = squirrel->LocalHeap->Number(sqrt(static_cast<double>(left->fData())));
		break;
	      }
	    case SQI_DOUBLE:
	      {
		ret = squirrel->LocalHeap->Number(sqrt(left->dData()));
		break;
	      }
	    }

	  return ret;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"sqrt","need 1 number"));
      
    }
}

/*
 * function    : SQI_Arith_Power
 * purpose     : Builtin predicate "power"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Power(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"power","need 2 arguments"));
  else
    {
      SQI_Object *ag1 = (*args)[0];
      SQI_Object *ag2 = (*args)[1];

      if((ag1->Is()==SQI_NUMBER)&&(ag2->Is()==SQI_NUMBER))
	{
	  SQI_Number *left = dynamic_cast<SQI_Number *>(ag1);
	  SQI_Number *right = dynamic_cast<SQI_Number *>(ag2); 
	  SQI_Object *ret = NULL;

	  if(*left<0)
	    {
	      if((right->IsA()==SQI_INT)||(right->IsA()==SQI_INT))
		ret = *left ^ right;
	      else
		throw(new SQI_Exception(SQI_EX_BADVALUE,"power","the second number must be an integer as the first is negative"));
	    }
	  else
	    ret = *left ^ right;
	  
	  return ret;
	}
      else
	throw(new SQI_Exception(SQI_EX_BADARGTYPE,"power","need 2 number"));
      
    }
}

/*
 * function    : SQI_Arith_Exp
 * purpose     : Builtin predicate "exp"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Exp(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"exp","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(exp(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(exp(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(exp(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(exp(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exp","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Log10
 * purpose     : Builtin predicate "log10"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Log10(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"log10","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(log10(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(log10(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(log10(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(log10(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"log10","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Ln
 * purpose     : Builtin predicate "ln"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Ln(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ln","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(log(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(log(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(log(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(log(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"log","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rsin
 * purpose     : Builtin predicate "rsin"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rsin(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rsin","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(sin(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(sin(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(sin(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(sin(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rsin","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rcos
 * purpose     : Builtin predicate "rcos"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rcos(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rcos","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(cos(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(cos(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(cos(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(cos(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rcos","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rtan
 * purpose     : Builtin predicate "rtan"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rtan(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rtan","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(tan(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(tan(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(tan(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(tan(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rtan","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rasin
 * purpose     : Builtin predicate "rasin"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rasin(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rasin","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(asin(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(asin(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(asin(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(asin(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rasin","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Racos
 * purpose     : Builtin predicate "radacos"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Racos(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"racos","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(acos(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(acos(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(acos(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(acos(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"racos","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Ratan
 * purpose     : Builtin predicate "ratan"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Ratan(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ratan","need 1 or 2 argument"));
  else
    {
      if(args->size()==2)
      {
      	SQI_Object *ag1 = (*args)[0];
		SQI_Object *ag2 = (*args)[2];

		if((ag1->Is()==SQI_NUMBER)&&(ag2->Is()==SQI_NUMBER))
		{
			SQI_Number *left = dynamic_cast<SQI_Number *>(ag1);
			SQI_Number *right = dynamic_cast<SQI_Number *>(ag2);  
	  		SQI_Number *ret = NULL;

	  		switch(left->IsA())
	    	{
	    		case SQI_INT:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->iData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->iData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->iData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->iData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;
	      		}
	    		case SQI_LONG:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->lData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->lData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->lData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->lData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;					
	      		}
	    		case SQI_FLOAT:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->fData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->fData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->fData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(static_cast<double>(left->fData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;				
	      		}
	    		case SQI_DOUBLE:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(left->dData(),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(left->dData(),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(left->dData(),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(atan2(left->dData(),left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;				
	      		}
	    	}

	  		return ret;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ratan","need number"));
      }
      else
      {
      	SQI_Object *ag1 = (*args)[0];

		if(ag1->Is()==SQI_NUMBER)
		{
			SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  		SQI_Number *ret = NULL;

	  		switch(left->IsA())
	    	{
	    		case SQI_INT:
	      		{
					ret = squirrel->LocalHeap->Number(atan(static_cast<double>(left->iData())));
					break;
	      		}
	    		case SQI_LONG:
	      		{
					ret = squirrel->LocalHeap->Number(atan(static_cast<double>(left->lData())));
					break;
	      		}
	    		case SQI_FLOAT:
	      		{
					ret = squirrel->LocalHeap->Number(atan(static_cast<double>(left->fData())));
					break;
	      		}
	    		case SQI_DOUBLE:
	      		{
					ret = squirrel->LocalHeap->Number(atan(left->dData()));
					break;
	      		}
	    	}

	  		return ret;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ratan","need number"));
      }
    }
}

/*
 * function    : SQI_Arith_Rcosh
 * purpose     : Builtin predicate "rcosh"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rcosh(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rcosh","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(cosh(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(cosh(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(cosh(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(cosh(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rcosh","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rsinh
 * purpose     : Builtin predicate "rsinh"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rsinh(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rsinh","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(sinh(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(sinh(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(sinh(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(sinh(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rsinh","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Rtanh
 * purpose     : Builtin predicate "rtanh"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Rtanh(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"rtanh","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(tanh(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(tanh(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(tanh(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(tanh(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"rtanh","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Abs
 * purpose     : Builtin predicate "abs"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Abs(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"abs","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(abs(left->iData()));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(abs(left->lData()));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(fabs(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(fabs(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"abs","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Erf
 * purpose     : Builtin predicate "erf"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Erf(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"erf","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(erf(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(erf(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(erf(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(erf(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"erf","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Erfc
 * purpose     : Builtin predicate "erfc"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Erfc(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"erfc","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(erfc(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(erfc(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(erfc(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(erfc(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"erfc","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Gamma
 * purpose     : Builtin predicate "gamma"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Gamma(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"gamma","need 1 argument"));
  else
    {
      SQI_Object *ag1 = (*args)[0];

	if(ag1->Is()==SQI_NUMBER)
	{
		SQI_Number *left = dynamic_cast<SQI_Number *>(ag1); 
	  	SQI_Number *ret = NULL;

	  	switch(left->IsA())
	    {
	    case SQI_INT:
	      {
			ret = squirrel->LocalHeap->Number(gamma(static_cast<double>(left->iData())));
			break;
	      }
	    case SQI_LONG:
	      {
			ret = squirrel->LocalHeap->Number(gamma(static_cast<double>(left->lData())));
			break;
	      }
	    case SQI_FLOAT:
	      {
			ret = squirrel->LocalHeap->Number(gamma(static_cast<double>(left->fData())));
			break;
	      }
	    case SQI_DOUBLE:
	      {
			ret = squirrel->LocalHeap->Number(gamma(left->dData()));
			break;
	      }
	    }

	  return ret;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"gamma","need 1 number"));
    }
}

/*
 * function    : SQI_Arith_Hypot
 * purpose     : Builtin predicate "hypot"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Hypot(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"hypot","need 2 arguments"));
  else
    {
      	SQI_Object *ag1 = (*args)[0];
		SQI_Object *ag2 = (*args)[2];

		if((ag1->Is()==SQI_NUMBER)&&(ag2->Is()==SQI_NUMBER))
		{
			SQI_Number *left = dynamic_cast<SQI_Number *>(ag1);
			SQI_Number *right = dynamic_cast<SQI_Number *>(ag2);  
	  		SQI_Number *ret = NULL;

	  		switch(left->IsA())
	    	{
	    		case SQI_INT:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->iData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->iData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->iData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->iData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;
	      		}
	    		case SQI_LONG:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->lData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->lData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->lData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->lData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;					
	      		}
	    		case SQI_FLOAT:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->fData()),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->fData()),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->fData()),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(static_cast<double>(left->fData()),
	      						left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;				
	      		}
	    		case SQI_DOUBLE:
	      		{
	      			switch(right->IsA())
	      			{
	      				case SQI_INT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(left->dData(),
	      						static_cast<double>(left->iData())));
	      						break;	
	      					}
	      				case SQI_LONG:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(left->dData(),
	      						static_cast<double>(left->lData())));
	      						break;	
	      					}
	      				case SQI_FLOAT:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(left->dData(),
	      						static_cast<double>(left->fData())));
	      						break;	
	      					}	      				
	      				case SQI_DOUBLE:
	      					{
	      						ret = squirrel->LocalHeap->Number(hypot(left->dData(),left->dData()));
	      						break;	
	      					}	      					
	      			}
					break;				
	      		}
	    	}

	  		return ret;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ratan","need number"));
    }
}

// Some Random number generator functions

double Random(double min,double max)
{
double r;

/* Sanity check */
//ASSERT(min <= max);
if (min > max) { return(0); }

/* Find range of values. */
max -= min;

/* This ensures that it's (x <= max) as opposed
               * to (x < max) */
++max;

/* This is the /recommended/ method for getting a
* number between min and max-1. The high bits are
* more random than the low bits.*/
r = (double)max*random()/(RAND_MAX+1.0);

return(r+min);
}

int Random(int min,int max)
{
long r;

/* Sanity check */
//ASSERT(min <= max);
if (min > max) { return(0); }

/* Find range of values. */
max -= min;

/* This ensures that it's (x <= max) as opposed
               * to (x < max) */
++max;

/* This is the /recommended/ method for getting a
* number between min and max-1. The high bits are
* more random than the low bits.*/
r = (int)((double)max*random()/(RAND_MAX+1.0));

return(r+min);
}

int Random(long min,long max)
{
long r;

/* Sanity check */
//ASSERT(min <= max);
if (min > max) { return(0); }

/* Find range of values. */
max -= min;

/* This ensures that it's (x <= max) as opposed
               * to (x < max) */
++max;

/* This is the /recommended/ method for getting a
* number between min and max-1. The high bits are
* more random than the low bits.*/
r = (long)((double)max*random()/(RAND_MAX+1.0));

return(r+min);
}

long Random(long max)
{
/* This is the /recommended/ method for getting a
* number between 0 and max-1. The high bits are more
* random than the low bits.*/
return((long) ((double)max*random()/(RAND_MAX+1.0)));
}

/*
 * function    : SQI_Arith_Random
 * purpose     : Builtin predicate "random"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Arith_Random(deque<SQI_Object *> *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  // first we check if we have enough argument

  if(args->size()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"random","need at least argument"));
  else
    {
      SQI_Object *max = (*args)[0];
      SQI_Object *min = NULL;

      if(args->size()>1)
	{
	  min = (*args)[1];
	  if((max->Is()==SQI_NUMBER)&&(min->Is()==SQI_NUMBER))
	    {
	      SQI_Number *left = dynamic_cast<SQI_Number *>(max);
	      SQI_Number *right = dynamic_cast<SQI_Number *>(min);
	      SQI_Number *ret = NULL;

	      switch(left->IsA())
		{
		case SQI_INT:
		  {
		    switch(right->IsA())
		      {
		      case SQI_INT:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->iData(),right->iData()));
			  break;
			}
		      case SQI_LONG:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<long>(left->iData()),right->lData()));
			  break;
			}
		      case SQI_FLOAT:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->iData()),static_cast<double>(right->fData())));
			  break;
			}
		      case SQI_DOUBLE:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->iData()),right->dData()));
			  break;
			}
		      }
		    break;
		  }
		case SQI_LONG:
		  {
		    switch(right->IsA())
		      {
		      case SQI_INT:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->lData(),static_cast<long>(right->iData())));
			  break;
			}
		      case SQI_LONG:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->lData(),right->lData()));
			  break;
			}
		      case SQI_FLOAT:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->lData()),static_cast<double>(right->fData())));
			  break;
			}
		      case SQI_DOUBLE:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->lData()),right->dData()));
			  break;
			}
		      }
		    break;
		  }
		case SQI_FLOAT:
		  {
		    switch(right->IsA())
		      {
		      case SQI_INT:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->fData()),static_cast<double>(right->iData())));
			  break;
			}
		      case SQI_LONG:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->fData()),static_cast<double>(right->lData())));
			  break;
			}
		      case SQI_FLOAT:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->fData()),static_cast<double>(right->fData())));
			  break;
			}
		      case SQI_DOUBLE:
			{
			  ret = squirrel->LocalHeap->Number(Random(static_cast<double>(left->fData()),right->dData()));
			  break;
			}
		      }
		    break;
		  }
		case SQI_DOUBLE:
		  {
		    switch(right->IsA())
		      {
		      case SQI_INT:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->dData(),static_cast<double>(right->iData())));
			  break;
			}
		      case SQI_LONG:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->dData(),static_cast<double>(right->lData())));
			  break;
			}
		      case SQI_FLOAT:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->dData(),static_cast<double>(right->fData())));
			  break;
			}
		      case SQI_DOUBLE:
			{
			  ret = squirrel->LocalHeap->Number(Random(left->dData(),right->dData()));
			  break;
			}
		      }
		    break;
		  }
		}
	      
	      return ret;
	    }
	  else
	    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"random","need numbers"));
	  
	}
      else
	{
	  if(max->Is()==SQI_NUMBER)
	    {
	      SQI_Number *left = dynamic_cast<SQI_Number *>(max); 
	      SQI_Number *ret = NULL;
	      
	      switch(left->IsA())
		{
		case SQI_INT:
		  {
		    ret = squirrel->LocalHeap->Number(Random(static_cast<long>(left->iData())));
		    break;
		  }
		case SQI_LONG:
		  {
		    ret = squirrel->LocalHeap->Number(Random(left->lData()));
		    break;
		  }
		case SQI_FLOAT:
		  {
		    ret = squirrel->LocalHeap->Number(Random(0.0,static_cast<double>(left->fData())));
		    break;
		  }
		case SQI_DOUBLE:
		  {
		    ret = squirrel->LocalHeap->Number(Random(0.0,left->dData()));
		    break;
		  }
		}
	      
	      return ret;
	    }
	  else
	    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"random","need number(s)"));
	}
    }
}


/*
 * function    : Randomize
 * purpose     : Initialize the seed of the random generator
 * input       : none
 * output      : none
 * side effect : none
 */
void Randomize(void)
{
	time_t t;
	srandom((unsigned) time(&t));
}

/*
 * function    : SQI_Arith_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Arith_Remove(SQI_BuiltInMap *Map)
{
	Map->RemoveFunc("max");
  	Map->RemoveFunc("min");
  	Map->RemoveFunc("avg");
  	Map->RemoveFunc("incr");
  	Map->RemoveFunc("sum");
  	Map->RemoveFunc("difference");
  	Map->RemoveFunc("minus");
  	Map->RemoveFunc("product"); 
  	Map->RemoveFunc("quotient");
  	Map->RemoveFunc("modulo");
  	Map->RemoveFunc("int");
  	Map->RemoveFunc("floor");
  	Map->RemoveFunc("ceil");
  	Map->RemoveFunc("sqrt");
 	Map->RemoveFunc("power");
  	Map->RemoveFunc("exp");
  	Map->RemoveFunc("log10");
  	Map->RemoveFunc("ln");
  	Map->RemoveFunc("rsin");
  	Map->RemoveFunc("rcos");
  	Map->RemoveFunc("rtan");
  	Map->RemoveFunc("rasin");
  	Map->RemoveFunc("racos");
  	Map->RemoveFunc("ratan");
  	Map->RemoveFunc("rsinh");
  	Map->RemoveFunc("rcosh");
  	Map->RemoveFunc("rtanh");
  	Map->RemoveFunc("abs");
  	Map->RemoveFunc("erf");	
  	Map->RemoveFunc("erfc");
  	Map->RemoveFunc("gamma");
  	Map->RemoveFunc("hypot");
  	Map->RemoveFunc("random");		
}


/*
 * function    : SQI_Arith_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 *
 * output      : none
 * side effect : none
 */
void SQI_Arith_Init(SQI_BuiltInMap *Map)
{
  Randomize();	
	
  Map->AddAddOn(Arith_Pkg_name,Arith_Pkg_purpose,Arith_Pkg_author,Arith_Pkg_version,Arith_Pkg_date,SQI_Arith_Remove);

  Map->AddFunc("max",
	       Arith_Pkg_name,
	       "Return the maximum of several value (or list)",
	       "max value1 value2 ....",
	       SQI_Arith_Max
	       );

  Map->AddFunc("min",
	       Arith_Pkg_name,
	       "Return the minimum of several value (or list)",
	       "min value1 value2 ....",
	       SQI_Arith_Min
	       );

  Map->AddFunc("avg",
	       Arith_Pkg_name,
	       "Return the average value of several value (or list)",
	       "avg value1 value2 ....",
	       SQI_Arith_Avg
	       );
  
  Map->AddFunc("incr",
	       Arith_Pkg_name,
	       "Modify a value by adding another value (or list)",
	       "incr variable_name increment",
	       SQI_Arith_Incr
	       );
  
  Map->AddFunc("sum",
	       Arith_Pkg_name,
	       "Output the sum of all the argument (list/number)",
	       "sum thing1 thing2 ....",
	       SQI_Arith_Sum
	       );

  Map->AddFunc("difference",
	       Arith_Pkg_name,
	       "Output the difference of all the argument (number)",
	       "difference num1 num2",
	       SQI_Arith_Difference
	       );

  Map->AddFunc("minus",
	       Arith_Pkg_name,
	       "Output the negative value of the argument (number)",
	       "minus num",
	       SQI_Arith_Minus
	       );
  
  Map->AddFunc("product",
	       Arith_Pkg_name,
	       "Output the product of all the argument (number/list)",
	       "product thing1 thing2 ...",
	       SQI_Arith_Product
	       );

  Map->AddFunc("quotient",
	       Arith_Pkg_name,
	       "Output the quotient of the argument (number)",
	       "quotient num1 num2",
	       SQI_Arith_Quotient
	       );

  Map->AddFunc("modulo",
	       Arith_Pkg_name,
	       "Output the remainder of the integer quotient of the argument (number)",
	       "modulo num1 num2",
	       SQI_Arith_Modulo
	       );

  Map->AddFunc("int",
	       Arith_Pkg_name,
	       "Output a cast of the argument in integer",
	       "int num",
	       SQI_Arith_Int
	       );

  Map->AddFunc("floor",
	       Arith_Pkg_name,
	       "Output the largest integer not greater than the argument",
	       "floor num",
	       SQI_Arith_Floor
	       );

  Map->AddFunc("ceil",
	       Arith_Pkg_name,
	       "Output the smallest integer not less than the argument",
	       "ceil num",
	       SQI_Arith_Ceil
	       );

  Map->AddFunc("sqrt",
	       Arith_Pkg_name,
	       "Output the square root of the argument",
	       "sqrt num",
	       SQI_Arith_Sqrt
	       );

  Map->AddFunc("power",
	       Arith_Pkg_name,
	       "Output the power of the first argument by the second",
	       "power num1 num2",
	       SQI_Arith_Power
	       );
	       
	Map->AddFunc("exp",
	       Arith_Pkg_name,
	       "Output the exponantial value of the argument",
	       "exp num",
	       SQI_Arith_Exp
	       );
	       
	Map->AddFunc("log10",
	       Arith_Pkg_name,
	       "Output the logarithm (base 10) of the argument",
	       "log10 num",
	       SQI_Arith_Log10
	       );
	       
	Map->AddFunc("ln",
	       Arith_Pkg_name,
	       "Output the natural logarithm of the argument",
	       "ln num",
	       SQI_Arith_Ln
	       );
	       
	Map->AddFunc("rsin",
	       Arith_Pkg_name,
	       "Output the sine of the argument (radian)",
	       "rsin num",
	       SQI_Arith_Rsin
	       );
	       
	Map->AddFunc("rcos",
	       Arith_Pkg_name,
	       "Output the cosine of the argument (radian)",
	       "rcos num",
	       SQI_Arith_Rcos
	       );
	       
	Map->AddFunc("rtan",
	       Arith_Pkg_name,
	       "Output the tangent of the argument (radian)",
	       "rtan num",
	       SQI_Arith_Rtan
	       );
	       
    Map->AddFunc("rasin",
	       Arith_Pkg_name,
	       "Output the arcsine of the argument (radian)",
	       "rasin num",
	       SQI_Arith_Rasin
	       );
	       
	Map->AddFunc("racos",
	       Arith_Pkg_name,
	       "Output the arccosine of the argument (radian)",
	       "racos num",
	       SQI_Arith_Rasin
	       );
	       
	Map->AddFunc("ratan",
	       Arith_Pkg_name,
	       "Output the arctangent of the argument (radian) or arctangent of y/x if 2 argument",
	       "ratan num1 (num2)",
	       SQI_Arith_Ratan
	       );
	       
	Map->AddFunc("rsinh",
	       Arith_Pkg_name,
	       "Output the hyperbolic-sine of the argument (radian)",
	       "rsinh num",
	       SQI_Arith_Rsinh
	       );
	       
	Map->AddFunc("rcosh",
	       Arith_Pkg_name,
	       "Output the hyperbolic-cosine of the argument (radian)",
	       "rcosh num",
	       SQI_Arith_Rcosh
	       );
	       
	Map->AddFunc("rtanh",
	       Arith_Pkg_name,
	       "Output the hyperbolic-tangent of the argument (radian)",
	       "rtanh num",
	       SQI_Arith_Rtanh
	       );
	       
	Map->AddFunc("abs",
	       Arith_Pkg_name,
	       "Output the absolute value of the argument",
	       "abs num",
	       SQI_Arith_Abs
	       );
	       
	Map->AddFunc("erf",
	       Arith_Pkg_name,
	       "Output the error function of the argument",
	       "erf num",
	       SQI_Arith_Erf
	       );
	       
	Map->AddFunc("erfc",
	       Arith_Pkg_name,
	       "Output the error function of the argument",
	       "erfc num",
	       SQI_Arith_Erfc
	       );
	       
	Map->AddFunc("gamma",
	       Arith_Pkg_name,
	       "Output the gamma value of the argument",
	       "gamma num",
	       SQI_Arith_Gamma
	       );
	       
	Map->AddFunc("hypot",
	       Arith_Pkg_name,
	       "Output the hypot value of the argument",
	       "hypot x y",
	       SQI_Arith_Hypot
	       );
	       
	Map->AddFunc("random",
	       Arith_Pkg_name,
	       "Output a random value between min and max, or greater than 0 and less than max if only one arugument is supplied",
	       "random (min) max",
	       SQI_Arith_Random
	       );
}

