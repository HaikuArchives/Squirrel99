/*
 * Squirrel project
 *
 * file ?	SQI-bbutton.cpp
 * what	?  	bbutton object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbutton.h"
#include "SQI-bwindow.h"
#include "SQI-button.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *ButtonMethods = NULL;	// methods hash table
MethodTable *ButtonSettings = NULL;	// settings hash table
  
/*
 * function    : SQI_BButton
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BButton::SQI_BButton(SQI_Squirrel *squirrel,const char *label,int width,int height,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BBUTTON)
{		
	button = new SQI_Button(squirrel->interpreter,this,label,new BMessage(EXEC_WIDGET),BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS);		
	
	View   = dynamic_cast<BView *>(button);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	button->char_width = width;
	button->char_height = height;	

	if(!ButtonMethods)
		InitBButton();	
		
	Methods		= ButtonMethods;
	Settings 	= ButtonSettings;	
}

/*
 * function    : SQI_BButton
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, the heap to use
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BButton::SQI_BButton(SQI_Heap *target,const char *label,BRect frame,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(target,SQI_BBUTTON)
{	
	button = new SQI_Button(NULL,this,label,new BMessage(EXEC_WIDGET),frame,resizingMode,flags | B_FRAME_EVENTS);		
	
	View   = dynamic_cast<BView *>(button);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!ButtonMethods)
		InitBButton();	
		
	Methods		= ButtonMethods;
	Settings 	= ButtonSettings;
}

/*
 * function    : SQI_BButton
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BButton::~SQI_BButton()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BButton::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View)
 			{
 				if(Window)
 				{
 					Lock();
					View->RemoveSelf();
					Unlock();	
 				}	
				delete View;
 			}
 			delete this;
			return true;
 		}
 		
 	return false;
}

/*
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BButton::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BButton *clone = new SQI_BButton(hp,button->Label(),View->Frame(),View->ResizingMode(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		button->Clone(clone->button);
					
		// and it's over
		
		Unlock();		
			
		return clone;		
	}
	
	return NULL;
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BButton::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << button->Label() << '\0';
  	else 
   		out << "NONE" << '\0';
   
  	string *ret = new string(out.str());
  	out.freeze(false);
  	return ret;
}	
	
/*
 * function    : Show
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BButton::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BButton::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BBUTTON[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// members function

/*
 * function    : IsDefault
 * purpose     : Return true if the widget is the default button
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::IsDefault(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(button->IsDefault())
			return squirrel->interpreter->True;
		else
			return squirrel->interpreter->False;				
	}
	
	return NULL;	
}

/*
 * function    : MakeDefault
 * purpose     : Make this button the default button of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::MakeDefault(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{	
		SQI_Number *b = IsBool((*inputs)[0]);	
	
		if(b)
		{
			if(View)
			{
				if(Window)
				{
					if(b->IsTrue())
					{	
						BButton *button = dynamic_cast<BButton *>(View);
						Lock();
						Window->Window->SetDefaultButton(button);
						Unlock();
					}	
					else
					{
						Lock();
						Window->Window->SetDefaultButton(NULL);
						Unlock();	
					}
				}
				else
					if(b->IsTrue())
						button->makedefault = true;				
			}
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Button~enable","input must be a boolean"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Button~default","need one input"));		
	
	return NULL;	
}

/*
 * function    : SetEnabled
 * purpose     : Set if the widget is enable or disable
 * input       : 
 *
 * bool state, enable or disable
 *
 * output      : none
 * side effect : none
 */
void SQI_BButton::SetEnabled(bool state)
{
	if(View)	
	{
		enabled = state;
		button->SetEnabled(state);	
	}
}

/*
 * function    : Invoke
 * purpose     : Invoke the button click hook
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		Lock();
		button->Invoked(View);
		Unlock();	
		return NULL;				
	}
	
	return NULL;	
}

// settings function



/*
 * function    : Label
 * purpose     : Set/Get the label
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::Label(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_String(squirrel->LocalHeap,button->Label());	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				string *str = label->Data();
				
				button->SetLabel(str->c_str());
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a string/word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBButton()
{	
	ButtonMethods 	= new MethodTable();
	ButtonSettings 	= new MethodTable();
	
	// specific	
	
	ButtonMethods->Add("invoke",(OMeth)(&SQI_BButton::Invoke));
	ButtonMethods->Add("default",(OMeth)(&SQI_BButton::MakeDefault));
	ButtonMethods->Add("is.default",(OMeth)(&SQI_BButton::IsDefault));
	
	ButtonSettings->Add("label",(OMeth)(&SQI_BButton::Label));			
}

void UnInitBButton()
{
	if(ButtonMethods)
	{
		delete ButtonMethods;
		delete ButtonSettings;	
	}	
}

/*
 * function    : IsButton
 * purpose     : Test if an object is a bbutton
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BButton *, the bbutton or NULL else
 * side effect : none
 */
SQI_BButton *IsButton(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BBUTTON)
    	return dynamic_cast<SQI_BButton *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

