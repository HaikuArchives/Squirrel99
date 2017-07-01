/*
 * Squirrel project
 *
 * file ?	SQI-btext.cpp
 * what	?  	btext object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-btext.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-text.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include <StringView.h>

MethodTable *TextMethods = NULL;		// methods hash table
MethodTable *TextSettings= NULL;	// settings hash table

/*
 * function    : SQI_BText
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
SQI_BText::SQI_BText(SQI_Squirrel *squirrel,const char *text,int width,int height,alignment align,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BTEXT)
{	
	this->text = new SQI_Text(squirrel->interpreter,this,BRect(0,0,0,0),text,align,flags);		
	
	this->text->char_width = width;
	this->text->char_height = height;	
			
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(this->text);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!TextMethods)
		InitBText();	
		
	Methods		= TextMethods;
	Settings 	= TextSettings;	
}

/*
 * function    : SQI_BText
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
SQI_BText::SQI_BText(SQI_Heap *target,BRect frame,const char *text,alignment align,uint32 flags)
	:SQI_BWidget(target,SQI_BTEXT)
{	
	this->text = new SQI_Text(NULL,this,frame,text,align,flags);		
			
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(this->text);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!TextMethods)
		InitBText();	
		
	Methods		= TextMethods;
	Settings 	= TextSettings;		
}

/*
 * function    : SQI_BText
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BText::~SQI_BText()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BText::Suicidal(bool force=false)
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
SQI_Object *SQI_BText::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BText *clone = new SQI_BText(hp,View->Frame(),text->Text(),text->Alignment(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		text->Clone(clone->text);
		
		// now we need to clone all the view attached to this view
		
		if(Contents)
		{
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			SQI_BWidget *widget;
			TGlue *glue;
			
			clone->Contents = new list<TGlue *>;		
				
			for(i=Contents->begin();i!=Contents->end();i++)
			{
				glue = new TGlue;
				
				glue->alignment = (*i)->alignment;
				glue->elements = new list<SQI_BWidget *>;
				
				for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				{
					widget = dynamic_cast<SQI_BWidget *>((*j)->DeepClone(hp));
					widget->AddRef();	
					glue->elements->push_back(widget);
					
					// we add the view to the window	
					
					clone->View->AddChild(widget->GiveBView());
				}		
				
				clone->Contents->push_back(glue);	
			}
		}	
			
		// and it's over
		
		if(View->Window())
			View->Window()->Unlock();		
			
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
string *SQI_BText::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Text" << '\0';
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
string *SQI_BText::Show(int prec = 3)
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
string *SQI_BText::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BTEXT[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
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
void SQI_BText::SetEnabled(bool state)
{
	enabled = state;	
}

// members function

/*
 * function    : Text
 * purpose     : Set/Get the text
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BText::Text(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
				return new SQI_String(squirrel->LocalHeap,text->Text());
			else
			if(*str == "set")
			{
				SQI_String *txt = IsString((*inputs)[1]);
				if(!txt)
					txt = IsKeyword((*inputs)[1]);	
				
				if(txt)
				{
					Lock();
					text->SetText(txt->Data()->c_str());
					text->Invalidate();
					Unlock();	
					return NULL;				
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~text","second input must be a string/word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~text","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~text","first input must be the word \"get or \"set"));		
	}
}

/*
 * function    : Justfy
 * purpose     : Set/Get the text justification
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BText::Justify(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				alignment align = text->Alignment();
				
				switch(align)
				{
					case B_ALIGN_CENTER:
						return new SQI_Keyword(squirrel->LocalHeap,"center");
					case B_ALIGN_LEFT:
						return new SQI_Keyword(squirrel->LocalHeap,"left");
					case B_ALIGN_RIGHT:
						return new SQI_Keyword(squirrel->LocalHeap,"right");		
				}	
			}
			else
			if(*str == "set")
			{
				SQI_Keyword *align = IsKeyword((*inputs)[1]);
					
				if(align)
				{
					alignment a;
					string *str = align->Data();
					
					if(*str == "center")
						a = B_ALIGN_CENTER;
					else
					if(*str == "left")
						a = B_ALIGN_LEFT;
					else				
					if(*str == "right")
						a = B_ALIGN_RIGHT;
					else	
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~justify","alignment must be a valid word : \"center , \"left or \"right"));
							
					Lock();
					text->SetAlignment(a);
					Unlock();	
					
					return NULL;				
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~justify","second input must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~justify","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"TEXT~justify","first input must be the word \"get or \"set"));		
	}
}

// some used function

// Fill up the member map
void InitBText()
{	
	TextMethods 	= new MethodTable();
	TextSettings 	= NULL;

	TextMethods->Add("text",(OMeth)(&SQI_BText::Text));
	TextMethods->Add("justify",(OMeth)(&SQI_BText::Justify));				
}

void UnInitBText()
{
	if(TextMethods)
		delete TextMethods;	
}

/*
 * function    : IsText
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BText *, the bview or NULL else
 * side effect : none
 */
SQI_BText *IsText(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BTEXT)
    	return dynamic_cast<SQI_BText *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
