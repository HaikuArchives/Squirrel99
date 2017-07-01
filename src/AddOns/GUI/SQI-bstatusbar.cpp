/*
 * Squirrel project
 *
 * file ?	SQI-bstatusbar.cpp
 * what	?  	bstatusbar object
 * who	?	jlv
 * when	?	09/19/00
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bstatusbar.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-statusbar.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *SBarMethods = NULL;		// methods hash table
MethodTable *SBarSettings= NULL;	// settings hash table
 
  
/*
 * function    : SQI_BStatusBar
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
SQI_BStatusBar::SQI_BStatusBar(SQI_Squirrel *squirrel,const char *label,const char *trailing,int ttwidth,int trwidth,float max)
	:SQI_BWidget(squirrel,SQI_BSTATUSBAR)
{		
	sbar = new SQI_StatusBar(squirrel->interpreter,this,label,trailing,ttwidth,trwidth);
	
	sbar->SetMaxValue(max);		
		
	View   = dynamic_cast<BView *>(sbar);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!SBarMethods)
		InitBStatusBar();	
		
	Methods		= SBarMethods;
	Settings 	= SBarSettings;		
}

/*
 * function    : SQI_BStatusBar
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
SQI_BStatusBar::SQI_BStatusBar(SQI_Heap *target,const char *label,const char *trailing,int ttwidth,int trwidth,float max)
	:SQI_BWidget(target,SQI_BSTATUSBAR)
{	
	sbar = new SQI_StatusBar(NULL,this,label,trailing,ttwidth,trwidth);	
	
	sbar->SetMaxValue(max);		
	
	View   = dynamic_cast<BView *>(sbar);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!SBarMethods)
		InitBStatusBar();	
		
	Methods		= SBarMethods;
	Settings 	= SBarSettings;	
}

/*
 * function    : SQI_BStatusBar
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BStatusBar::~SQI_BStatusBar()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BStatusBar::Suicidal(bool force=false)
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
				View = NULL;
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
SQI_Object *SQI_BStatusBar::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BStatusBar *clone = new SQI_BStatusBar(hp,sbar->Label(),sbar->TrailingLabel(),sbar->tt_width,sbar->tr_width,sbar->MaxValue());
		
		// now we need to "clone" the view itself (set the same way)
		
		sbar->Clone(clone->sbar);
					
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
string *SQI_BStatusBar::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << sbar->Label() << '\0';
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
string *SQI_BStatusBar::Show(int prec = 3)
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
string *SQI_BStatusBar::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BSTATUSBAR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// members function

/*
 * function    : Update
 * purpose     : Update the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Update(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{		
		if(inputs->Length()>0)
		{
			SQI_Number *delta;
			
			delta = IsNumber((*inputs)[0]);
			
			if(delta)
			{
				SQI_String *str;
				SQI_String *str2;
				float d;
				const char *text = NULL,*trailing = NULL;	
			
				d = Number2Float(delta);
								
				if(inputs->Length()>1)
				{		
					str = IsString((*inputs)[1]);
					if(!str)
						str = IsKeyword((*inputs)[1]);
					if(str)
					{
						text = str->Data()->c_str();
					
						if(inputs->Length()==3)
						{		
							str2 = IsString((*inputs)[2]);
							if(!str2)
								str2 = IsKeyword((*inputs)[2]);
							if(str2)
								trailing = str2->Data()->c_str();
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~update","third input must be a string/word"));		
						}			
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~update","second input must be a string/word"));		
				}
				
				Lock();
				sbar->Update(d,text,trailing);
				Unlock();	
					
				return NULL;	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~update","first input must be a number"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"STATUSBAR~update","need one input at least"));		
	}
}

/*
 * function    : Reset
 * purpose     : reset the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Reset(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		const char *text = NULL,*trailing = NULL;
	
		if(inputs->Length()>0)
		{	
			SQI_String *str;
			
			str = IsString((*inputs)[1]);
			if(!str)
				str = IsKeyword((*inputs)[1]);
			if(str)
			{
				text = str->Data()->c_str();
				
				if(inputs->Length()==3)
				{	
					SQI_String *str;
					
					str = IsString((*inputs)[2]);
					if(!str)
						str = IsKeyword((*inputs)[2]);
					if(str)
						trailing = str->Data()->c_str();
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~update","third input must be a string/word"));		
				}			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~update","second input must be a string/word"));		
		}
		
		Lock();
		sbar->Reset(text,trailing);
		Unlock();	
		
		return NULL;
	}
}

// settings function

/*
 * function    : BarColor
 * purpose     : Set/Get the bar color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::BarColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = sbar->BarColor();
			SQI_List *lst = new SQI_List(squirrel->LocalHeap);
			
			GetListFromColor(squirrel,lst,&color);	
			
			return lst;
		}	
		else
		if(*str == "set")
		{
			SQI_List *lst = IsList((*inputs)[2]);
			
			if(lst)
			{
				rgb_color color;
				
				GetColorFromList(lst,&color);
				
				sbar->SetBarColor(color);	
							
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Text
 * purpose     : Set/Get the Text
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Text(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_String(squirrel->LocalHeap,sbar->Text());	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				string *str = label->Data();
				
				Lock();
				sbar->SetText(str->c_str());
				Unlock();
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","third input must be a string/word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Trailing
 * purpose     : Set/Get the Trailing
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Trailing(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_String(squirrel->LocalHeap,sbar->TrailingText());	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				string *str = label->Data();
				
				Lock();
				sbar->SetTrailingText(str->c_str());
				Unlock();
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","third input must be a string/word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Max
 * purpose     : Set/Get the max value
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Max(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")		
			return new SQI_Number(squirrel->LocalHeap,sbar->MaxValue());	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				Lock();
				sbar->SetMaxValue(Number2Float(pad));
				Unlock();	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Value
 * purpose     : Set/Get the current value
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BStatusBar::Value(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")		
			return new SQI_Number(squirrel->LocalHeap,sbar->CurrentValue());	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				Lock();
				sbar->Update(Number2Float(pad));
				Unlock();	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"STATUSBAR~config","second input must be the word \"get or \"set"));		
}


// some used function

// Fill up the member map
void InitBStatusBar()
{	
	SBarMethods 	= new MethodTable();
	SBarSettings 	= new MethodTable();

	SBarMethods->Add("update",(OMeth)(&SQI_BStatusBar::Update));
	SBarMethods->Add("reset",(OMeth)(&SQI_BStatusBar::Reset));
	
	SBarSettings->Add("text",(OMeth)(&SQI_BStatusBar::Text));
	SBarSettings->Add("trailing",(OMeth)(&SQI_BStatusBar::Trailing));
	SBarSettings->Add("max",(OMeth)(&SQI_BStatusBar::Max));
	SBarSettings->Add("value",(OMeth)(&SQI_BStatusBar::Value));
	SBarSettings->Add("bar.color",(OMeth)(&SQI_BStatusBar::BarColor));			
}

void UnInitBStatusBar()
{
	if(SBarMethods)
	{
		delete SBarMethods;
		delete SBarSettings;	
	}	
}

/*
 * function    : IsStatusBar
 * purpose     : Test if an object is a bbutton
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BStatusBar *, the bbutton or NULL else
 * side effect : none
 */
SQI_BStatusBar *IsStatusBar(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BSTATUSBAR)
    	return dynamic_cast<SQI_BStatusBar *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

