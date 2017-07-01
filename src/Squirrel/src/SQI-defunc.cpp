/*
 * Squirrel project
 *
 * file ?	SQI-defunc.cpp
 * what	?	Defunc class 
 * who	?	jlv
 * when	?	07/30/99
 * last	?	08/14/99
 *
 *
 * (c) electron Technology 1999
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"

// SQI_Defunc class

/*
 * function    : SQI_Defunc
 * purpose     : initialise
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Defunc::SQI_Defunc()
{
  params = NULL;
  body = NULL;
}

/*
 * function    : ~SQI_Defunc
 * purpose     : destroy
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Defunc::~SQI_Defunc()
{
  if(params)
    {
      list<SQI_Object *>::const_iterator i;
      SQI_Object *obj;
      for(i=params->begin();i!=params->end();i++)
		{
	  		obj = *i;
	  		obj->RemRef(true);
		}

      	delete params;
    	}

  if(body)
    body->RemRef(true);
}

/*
 * function    : SetParams
 * purpose     : Set the param list
 * input       :
 *
 * list<SQI_Object *> *p, the param list
 *
 * output      : none
 * side effect : none
 */
void SQI_Defunc::SetParams(list<SQI_Object *> *p)
{
	params = p;
	if(p)
	{
	list<SQI_Object *>::const_iterator i;
    for(i=params->begin();i!=params->end();i++)
	  (*i)->AddRef();
	}
}


/*
 * function    : SetBody
 * purpose     : Set the body block
 * input       :
 *
 * SQI_nBlock *b, the body block
 *
 * output      : none
 * side effect : none
 */
void SQI_Defunc::SetBody(SQI_nBlock *b)
{
  body = b;
  if(b)
  	b->AddRef();
}



