/*
 * Squirrel project
 *
 * file ?	SQI-bbutton.cpp
 * what	?  	bbutton object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	02/20/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbutton.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-button.h"
#include "SQI-utils.h"
#include "SQI-font.h"

BButtonMembers ButtonSettingMap;
BButtonMembers ButtonMemberMap;
  
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
	View = new SQI_Button(squirrel->interpreter,this,label,new BMessage(EXEC_BUTTON),BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS);		
	
	View->char_width = width;
	View->char_height = height;	
	
	Window = NULL;
	Members = &ButtonMemberMap;
	Settings = &ButtonSettingMap;	
	Contents = NULL;
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
	View = new SQI_Button(NULL,this,label,new BMessage(EXEC_BUTTON),frame,resizingMode,flags | B_FRAME_EVENTS);		
	
	Window = NULL;
	Members = &ButtonMemberMap;
	Settings = &ButtonSettingMap;	
	Contents = NULL;
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
 * function    : Export
 * purpose     : The object is aked to export himself to another heap
 * input       : 
 *
 * SQI_Heap *nheap, the target heap
 * char force, force the export even if the object is eternal
 *
 *
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
void SQI_BButton::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
		nheap->Import(this);	
}

/*
 * function    : Clone
 * purpose     : clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BButton::Clone(SQI_Heap *target = NULL)
{
	return DeepClone(target);
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
			
		SQI_BButton *clone = new SQI_BButton(hp,View->Label(),View->Frame(),View->ResizingMode(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		View->Clone(clone->View);
					
		// and it's over
		
		Unlock();		
			
		return clone;		
	}
	
	return NULL;
}

/*
 * function    : Member
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, method name
 * deque<SQI_Object *> *inputs, arguments
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BButton::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BButton::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
		MemberHook = (*Members)[*member];
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
		{
			Members->erase(*member);
			throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unknow member"));
		}	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unsupported by the object"));
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
		out << View->Label() << '\0';
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

// legacy from SQI_Widget

/*
 * function    : Hook
 * purpose     : Setup a hook
 * input       : 
 *
 * SQI_Keyword *event, event to hook
 * SQI_Object *action, action to perform
 * list<SQI_Object *> *extra, extra input supplied by the user
 *
 * output      : bool , return true if succes, false else
 * side effect : none
 */
bool SQI_BButton::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	if(View)
		return View->Hook(event,action,extra);
	else
		return false;
}

/*
 * function    : UnHook
 * purpose     : Remove a hook
 * input       : 
 *
 * SQI_Keyword *event, event to unhook
 *
 * output      : bool , return true if succes, false else
 * side effect : none
 */
bool SQI_BButton::UnHook(SQI_Keyword *event)
{
	if(View)
		return View->UnHook(event);
	else
		return false;
}

/*
 * function    : GiveBButton
 * purpose     : Return the BButton
 * input       : none
 * output      : BButton *
 * side effect : none
 */
BView *SQI_BButton::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BButton *
 * side effect : none
 */
SQI_Widget *SQI_BButton::GiveWidget()
{
	return dynamic_cast<SQI_Widget *>(static_cast<BView *>(View));	
}

/*
 * function    : TrueBounds
 * purpose     : Return the Bounds of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BButton::TrueBounds()
{
	BRect b = View->Bounds();
	b.right += 2.0 * View->padx;
	b.bottom += 2.0 * View->pady;	
	return b;
}

/*
 * function    : TrueFrame
 * purpose     : Return the Frame of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BButton::TrueFrame()
{
	BRect b = View->Frame();
	b.InsetBy(-View->padx,-View->pady);		
	return b;
}

/*
 * function    : MoveTo
 * purpose     : Move the view to a new position, take the pading in consideration
 * input       : 
 *
 * float x, new position
 * float y, new position
 *
 * output      : none
 * side effect : none
 */
void SQI_BButton::MoveTo(float x,float y)
{	
	View->MoveTo(x+View->padx,y+View->pady);
}

/*
 * function    : MoveBy
 * purpose     : Move the view to a new position by specifying the amount to move (padding already considerated)
 * input       : 
 *
 * float w, width to add 
 * float h, height to add
 *
 * output      : none
 * side effect : none
 */
void SQI_BButton::MoveBy(float w,float h)
{
	View->MoveBy(w,h);
}


/*
 * function    : Area
 * purpose     : Return the area covered by the view
 * input       : none
 * output      : float
 * side effect : none
 */
double SQI_BButton::Area()
{
	BRect t = TrueBounds();	
	
	return t.Width() * t.Height();
}

/*
 * function    : Expand
 * purpose     : Expand the view
 * input       : 
 *
 * float left, value to add on left
 * float top, value to add on top
 * float right, value to add on right
 * float bottom, value to add on bottom
 *
 * output      : float
 * side effect : none
 */
void SQI_BButton::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
	
	View->MoveBy(-left,-top);
	
	// now we resize-it
	
	View->ResizeBy(right+left,top+bottom);
}

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it prefered size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BButton::ResizeToPreferred()
{
	View->ResizeToPreferred();
}

// members function

/*
 * function    : Config
 * purpose     : Set/Get the configuration of the view
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BButton::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
				Setting = (*Settings)[*key];
			
				if(Setting)	
				{
					Lock();
					SQI_Object * ret = (this->*Setting)(squirrel,inputs);
					Unlock();
					return ret;
				}
				else
				{
					Settings->erase(*key);
					throw(new SQI_Exception(SQI_EX_INVALIDE,key->c_str(),"unknow setting"));
				}			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Button~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Button~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

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
		if(View->IsDefault())
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
						View->makedefault = true;				
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
 * function    : Enable
 * purpose     : Set the enable state of the button
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()==1)
		{
			SQI_Number *state = IsBool((*inputs)[0]);
			
			if(state)
			{
				Lock();
				if(state->IsTrue())	
					SetEnabled(true);
				else
					SetEnabled(false);
				Unlock();		
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Button~enable","input must be a boolean"));					
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Button~enable","need one input"));	
	}
	
	return NULL;	
}

/*
 * function    : IsEnable
 * purpose     : Return true if the widget is enable
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BButton::IsEnable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(View->IsEnabled())
			return squirrel->interpreter->True;
		else
			return squirrel->interpreter->False;				
	}
	
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
		View->SetEnabled(state);	
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
		View->Invoked();
		Unlock();	
		return NULL;				
	}
	
	return NULL;	
}

// settings function

/*
 * function    : AlignVertical
 * purpose     : Set/Get the vertical alignment
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();
				
			switch(widget->valign)
			{	
				case ALIGN_TOP:
					return squirrel->LocalHeap->Keyword("top");
				case ALIGN_CENTER:
					return squirrel->LocalHeap->Keyword("center");
				case ALIGN_BOTTOM:
					return squirrel->LocalHeap->Keyword("bottom");
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *align = IsKeyword((*inputs)[2]);
			
			if(align)
			{
				string *a = align->Data();
				SQI_Widget *widget = GiveWidget();	
					
				
				if(*a == "top")
					widget->valign = ALIGN_TOP;
				else
				if(*a == "center")
					widget->valign = ALIGN_CENTER;
				else
				if(*a == "bottom")
					widget->valign = ALIGN_BOTTOM;										
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : AlignHorizontal
 * purpose     : Set/Get the vertical alignment
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();
				
			switch(widget->halign)
			{	
				case ALIGN_LEFT:
					return squirrel->LocalHeap->Keyword("left");
				case ALIGN_CENTER:
					return squirrel->LocalHeap->Keyword("center");
				case ALIGN_RIGHT:
					return squirrel->LocalHeap->Keyword("right");
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *align = IsKeyword((*inputs)[2]);
			
			if(align)
			{
				string *a = align->Data();
				SQI_Widget *widget = GiveWidget();	
					
				
				if(*a == "left")
					widget->halign = ALIGN_LEFT;
				else
				if(*a == "center")
					widget->halign = ALIGN_CENTER;
				else
				if(*a == "right")
					widget->halign = ALIGN_RIGHT;										
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : PadX
 * purpose     : Set/Get the x padding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->padx);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				SQI_Widget *widget = GiveWidget();
				widget->padx = Number2Float(pad);	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : PadY
 * purpose     : Set/Get the y padding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->pady);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				SQI_Widget *widget = GiveWidget();
				widget->pady = Number2Float(pad);
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Pad
 * purpose     : Set/Get the pading (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==4)
	{	
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
					
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Number(widget->padx));
				lst->Add2End(squirrel->LocalHeap->Number(widget->pady));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Number *x = IsNumber((*inputs)[2]);
				SQI_Number *y = IsNumber((*inputs)[3]);	
				
				if(x && y)
				{
					SQI_Widget *widget = GiveWidget();	
					
					widget->padx = Number2Float(x);
					widget->pady = Number2Float(y);	

					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","need 4 inputs"));		
	
}

/*
 * function    : Align
 * purpose     : Set/Get the alignment (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==4)
	{	
		
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
				char *h,*v;
					
				switch(widget->halign)
				{	
					case ALIGN_LEFT:
						h = "left";
					case ALIGN_CENTER:
						h = "center";
					case ALIGN_RIGHT:
						h = "right";
				}	
					
				switch(widget->valign)
				{	
					case ALIGN_TOP:
						v = "left";
					case ALIGN_CENTER:
						v = "center";
					case ALIGN_BOTTOM:
						v = "right";
				}
				
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Keyword(h));
				lst->Add2End(squirrel->LocalHeap->Keyword(v));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Keyword *ho = IsKeyword((*inputs)[2]);
				SQI_Keyword *ve = IsKeyword((*inputs)[3]);	
				
				if(ho && ve)
				{
					string *a = ho->Data();
					SQI_Widget *widget = GiveWidget();	
					
					if(*a == "left")
						widget->halign = ALIGN_LEFT;
					else
					if(*a == "center")
						widget->halign = ALIGN_CENTER;
					else
					if(*a == "right")
						widget->halign = ALIGN_RIGHT;										
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be \"left \"center \"right"));
				
					a = ve->Data();	
						
					if(*a == "top")
						widget->valign = ALIGN_TOP;
					else
					if(*a == "center")
						widget->valign = ALIGN_CENTER;
					else
					if(*a == "bottom")
						widget->valign = ALIGN_BOTTOM;										
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","need 4 inputs"));		
	
}

/*
 * function    : ExpandX
 * purpose     : Set/Get the x expand
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->expandx);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *ex = IsBool((*inputs)[2]);
			
			if(ex)
			{
				SQI_Widget *widget = GiveWidget();
				widget->expandx = Number2Bool(ex);	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : ExpandY
 * purpose     : Set/Get the y expand
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->expandy);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *ex = IsBool((*inputs)[2]);
			
			if(ex)
			{
				SQI_Widget *widget = GiveWidget();
				widget->expandy = Number2Bool(ex);
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Expand
 * purpose     : Set/Get the expand (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==4)
	{	
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
					
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Number(widget->expandx));
				lst->Add2End(squirrel->LocalHeap->Number(widget->expandy));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Number *x = IsBool((*inputs)[2]);
				SQI_Number *y = IsBool((*inputs)[3]);	
				
				if(x && y)
				{
					SQI_Widget *widget = GiveWidget();	
					
					widget->expandx = Number2Bool(x);
					widget->expandy = Number2Bool(y);	

					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","need 4 inputs"));		
	
}

/*
 * function    : BgColor
 * purpose     : Set/Get the background color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = View->ViewColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
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
				
				View->SetViewColor(color);	
				
				View->bgcolor_set = true;
				
				View->Invalidate();
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : HighColor
 * purpose     : Set/Get the high color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = View->HighColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
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
				
				View->SetHighColor(color);	
				
				View->hcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : LowColor
 * purpose     : Set/Get the low color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = View->LowColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
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
				
				View->SetLowColor(color);	
				
				View->lcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Font
 * purpose     : Set/Get the font
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BButton::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{	
			BFont font;
			
			View->GetFont(&font);	
			
			return new SQI_Font(squirrel,&font);	
		}	
		else
		if(*str == "set")
		{
			SQI_Font *font = IsFont((*inputs)[2]);
			
			if(font)
			{
				View->SetFont(font);
				
				View->font_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BUTTON~config","second input must be the word \"get or \"set"));		
}

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
			return squirrel->LocalHeap->String(View->Label());	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				string *str = label->Data();
				
				View->SetLabel(str->c_str());
		
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
	ButtonMemberMap[string("config")] = &SQI_BButton::Config;
	ButtonMemberMap[string("enable")] = &SQI_BButton::Enable;
	ButtonMemberMap[string("invoke")] = &SQI_BButton::Invoke;
	ButtonMemberMap[string("default")] = &SQI_BButton::MakeDefault;
	ButtonMemberMap[string("is.default")] = &SQI_BButton::IsDefault;
	ButtonMemberMap[string("is.enable")] = &SQI_BButton::IsEnable;
	ButtonMemberMap[string("is.focus")] = &SQI_BButton::IsFocus;	
	
	ButtonSettingMap[string("align")] = &SQI_BButton::Align;
	ButtonSettingMap[string("align.v")] = &SQI_BButton::AlignVertical;
	ButtonSettingMap[string("align.h")] = &SQI_BButton::AlignHorizontal;
	ButtonSettingMap[string("bgcolor")] = &SQI_BButton::BgColor;
	ButtonSettingMap[string("expand")] = &SQI_BButton::Expand;
	ButtonSettingMap[string("expand.x")] = &SQI_BButton::ExpandX;
	ButtonSettingMap[string("expand.y")] = &SQI_BButton::ExpandY;
	ButtonSettingMap[string("font")] = &SQI_BButton::Font;
	ButtonSettingMap[string("label")] = &SQI_BButton::Label;
	ButtonSettingMap[string("low.color")] = &SQI_BButton::LowColor;
	ButtonSettingMap[string("high.color")] = &SQI_BButton::HighColor;
	ButtonSettingMap[string("pad")] = &SQI_BButton::Pad;
	ButtonSettingMap[string("pad.x")] = &SQI_BButton::PadX;
	ButtonSettingMap[string("pad.y")] = &SQI_BButton::PadY;			
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

