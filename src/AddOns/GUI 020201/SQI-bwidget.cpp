/*
 * Squirrel project
 *
 * file ?	SQI-bwidget.cpp
 * what	?   bwidget object
 * who	?	jlv
 * when	?	11/23/99
 * last	?	12/09/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <View.h> 
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
  
/*
 * function    : SQI_BWidget
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * char type, object type
 *
 * output      : none
 * side effect : none
 */
SQI_BWidget::SQI_BWidget(SQI_Squirrel *squirrel,char type)
	:SQI_ThePointer(squirrel,type)
{	
	SetClass(CLASS_BWIDGET);
	Contents = NULL;
	enabled = true;
}

/*
 * function    : SQI_BWidget
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to use
 * char type, object type
 *
 * output      : none
 * side effect : none
 */
SQI_BWidget::SQI_BWidget(SQI_Heap *target,char type)
	:SQI_ThePointer(target,type)
{
	SetClass(CLASS_BWIDGET);	
	Contents = NULL;
	enabled = true;
}

/*
 * function    : ~SQI_BWidget
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BWidget::~SQI_BWidget()
{
	if(Contents)
	{
		list<TGlue *>::const_iterator i;
		list<SQI_BWidget *>::const_iterator j;
		TGlue *elem;	
		
		for(i=Contents->begin();i!=Contents->end();i++)
		{
			elem = *i;
			for(j=elem->elements->begin();j!=elem->elements->end();j++)
			{
				(*j)->RemoveSelf();
				(*j)->Alone();
				(*j)->RemRef();			
			}	
			elem->elements->clear();
			delete elem->elements;
			delete elem;
		}	
		
		Contents->clear();
		delete Contents;	
	}	
}

/*
 * function    : Lock
 * purpose     : Lock the window in which the widget is attached (if so)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BWidget::Lock()
{
	if(Window)
		Window->Lock();	
}

/*
 * function    : Unlock
 * purpose     : Unlock the window in which the widget is attached (if so)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BWidget::Unlock()
{
	if(Window)
		Window->Unlock();
}

/*
 * function    : Enable
 * purpose     : Enable/Disable all the widget in the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWidget::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Contents)
	{
		if(inputs->Length()==1)
		{
			SQI_Number *state = IsBool((*inputs)[0]);
			if(state)
			{
				bool s = false;
				if(state->IsTrue())
					s = true;
				
				Lock();
				SetEnabled(s);
				Unlock();	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WIDGET~enable","input must be a boolean"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WIDGET~enable","need one input"));
	}	
	
	return NULL;	
}

/*
 * function    : Is.Enable
 * purpose     : Return true if the widget is enabled
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWidget::IsEnable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(enabled)
		return squirrel->interpreter->True;
	else
		return squirrel->interpreter->False;	
}


/*
 * function    : SetEnabled
 * purpose     : Set if the widget is enabled or not
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
void SQI_BWidget::SetEnabled(bool state)
{
	enabled = state;
	
	if(Contents)
	{			
		list<TGlue *>::const_iterator i;
		list<SQI_BWidget *>::const_iterator j;			
		for(i=Contents->begin();i!=Contents->end();i++)
		{						
			for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				(*j)->SetEnabled(state);
		}
	}		
}

/*
 * function    : MakeFocus
 * purpose     : Set the focus to or from the widget
 * input       : 
 *
 * bool focused, true if we set the focus
 *
 * output      : none
 * side effect : none
 */
void SQI_BWidget::MakeFocus(bool focused = true)
{
	BView *view = GiveBView();
	if(view)
	{
		Lock();
		view->MakeFocus(focused);
		Unlock();	
	}
}

/*
 * function    : IsFocus
 * purpose     : Return true if the widget is focused
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWidget::IsFocus(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	BView *view = GiveBView();

	if(view)
	{
		if(view->IsFocus())
			return squirrel->interpreter->True;
		else
			return squirrel->interpreter->False;	
	}
	else
		return squirrel->interpreter->False;	
}

/*
 * function    : RemoveSelf
 * purpose     : The widget remove it self from it parents
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BWidget::RemoveSelf()
{
	BView *view = GiveBView();	

	if(view)
	{
			Lock();
			view->RemoveSelf();
			Unlock();
			Window = NULL;	
	}	
}

/*
 * function    : IsWidget
 * purpose     : Test if an object is a widget
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BWidget *, the bwidget or NULL else
 * side effect : none
 */
SQI_BWidget *IsWidget(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->OfClass()==CLASS_BWIDGET || ptr->OfClass()==CLASS_CONTAINER)
    	return dynamic_cast<SQI_BWidget *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

/*
 * function    : IsContainer
 * purpose     : Test if an object is a widget container
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BWidget *, the bwidget or NULL else
 * side effect : none
 */
SQI_BWidget *IsContainer(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->OfClass()==CLASS_CONTAINER)
    	return dynamic_cast<SQI_BWidget *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}




