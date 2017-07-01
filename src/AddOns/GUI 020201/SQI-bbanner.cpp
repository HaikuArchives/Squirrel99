/*
 * Squirrel project
 *
 * file ?	SQI-bbanner.cpp
 * what	?  	bbanner object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	12/09/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbanner.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-banner.h"
#include "SQI-utils.h"
#include "SQI-font.h"

#include <StringView.h>

BBannerMembers BannerSettingMap;
BBannerMembers BannerMemberMap;
  
/*
 * function    : SQI_BBanner
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
SQI_BBanner::SQI_BBanner(SQI_Squirrel *squirrel,SQI_Keyword *var,int width,int height,alignment align,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BBANNER)
{	
	Variable = var;
	Variable->AddRef();
	Variable->Contained();
	
	char *text = NULL;	
	
	if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
	{
		SQI_Object *val = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
		string *str = val->Print();
		text = strdup(str->c_str());
		delete str;
	}
		
	// we need to set up the callback from when the variable is modified
	
	squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,BannerVarHook);

	View = new SQI_Banner(squirrel->interpreter,this,BRect(0,0,0,0),text,align,flags);		
	
	View->char_width = width;
	View->char_height = height;	
	
	Window = NULL;
	Members = &BannerMemberMap;
	Settings = &BannerSettingMap;	
	Contents = NULL;
			
	SetClass(CLASS_BWIDGET);
	
	if(text)
		free(text);
}

/*
 * function    : SQI_BBanner
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
SQI_BBanner::SQI_BBanner(SQI_Heap *target,BRect frame,SQI_Keyword *var,alignment align,uint32 flags)
	:SQI_BWidget(target,SQI_BBANNER)
{	
	Variable = var;
	Variable->AddRef();
	Variable->Contained();
	
	char *text = NULL;	
	
	/*
	if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
	{
		SQI_Object *val = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
		String *str = val->Print();
		text = strdup(str->c_str());
		delete str;
	}
		
	// we need to set up the callback from when the variable is modified
	
	squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,BannerVarHook);
	*/
	
	//View = new SQI_Banner(squirrel->interpreter,this,BRect(0,0,0,0),text,align,flags);		
		
	Window = NULL;
	Members = &BannerMemberMap;
	Settings = &BannerSettingMap;	
	Contents = NULL;
			
	SetClass(CLASS_BWIDGET);
	
	if(text)
		free(text);
		
	SetClass(CLASS_BWIDGET);
}

/*
 * function    : SQI_BBanner
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BBanner::~SQI_BBanner()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BBanner::Suicidal(bool force=false)
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
 			Variable->Alone();
 			Variable->RemRef();
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
void SQI_BBanner::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		// now we need to export all the widget 
		
		Variable->Export(nheap,force);
	}
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
SQI_Object *SQI_BBanner::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BBanner::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BBanner *clone = new SQI_BBanner(hp,View->Frame(),VariableClone(Variable),View->Alignment(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		View->Clone(clone->View);
					
		// and it's over
		
		if(View->Window())
			View->Window()->Unlock();		
			
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
SQI_Object *SQI_BBanner::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BBanner::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
string *SQI_BBanner::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Banner" << '\0';
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
string *SQI_BBanner::Show(int prec = 3)
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
string *SQI_BBanner::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BBANNER[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
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
bool SQI_BBanner::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
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
bool SQI_BBanner::UnHook(SQI_Keyword *event)
{
	if(View)
		return View->UnHook(event);
	else
		return false;
}

/*
 * function    : GiveBBanner
 * purpose     : Return the BBanner
 * input       : none
 * output      : BBanner *
 * side effect : none
 */
BView *SQI_BBanner::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BBanner *
 * side effect : none
 */
SQI_Widget *SQI_BBanner::GiveWidget()
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
BRect SQI_BBanner::TrueBounds()
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
BRect SQI_BBanner::TrueFrame()
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
void SQI_BBanner::MoveTo(float x,float y)
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
void SQI_BBanner::MoveBy(float w,float h)
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
double SQI_BBanner::Area()
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
void SQI_BBanner::Expand(float left,float top,float right,float bottom)
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
void SQI_BBanner::ResizeToPreferred()
{
	View->ResizeToPreferred();
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
void SQI_BBanner::SetEnabled(bool state)
{
	enabled = state;
	
	//View->SetEnabled(state);	
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
SQI_Object *SQI_BBanner::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BBanner::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Banner~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Banner~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Banner
 * purpose     : Set/Get the text
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BBanner::Text(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
				return squirrel->LocalHeap->String(View->Text());
			else
			if(*str == "set")
			{
				SQI_String *txt = IsString((*inputs)[1]);
				if(!txt)
					txt = IsKeyword((*inputs)[1]);	
				
				if(txt)
				{
					Lock();
					View->Interpreter->GlobalVar->Set(squirrel,Variable->Data(),(*inputs)[1],NO_SYSTEM_HOOK);
					View->SetText(txt->Data()->c_str());
					Unlock();	
					return NULL;				
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~text","second input must be a string/word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~text","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~text","first input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BBanner::Justify(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				alignment align = View->Alignment();
				
				switch(align)
				{
					case B_ALIGN_CENTER:
						return squirrel->LocalHeap->Keyword("center");
					case B_ALIGN_LEFT:
						return squirrel->LocalHeap->Keyword("left");
					case B_ALIGN_RIGHT:
						return squirrel->LocalHeap->Keyword("right");		
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~justify","alignment must be a valid word : \"center , \"left or \"right"));
							
					Lock();
					View->SetAlignment(a);
					Unlock();	
					
					return NULL;				
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~justify","second input must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~justify","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~justify","first input must be the word \"get or \"set"));		
	}
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
SQI_Object *SQI_BBanner::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BBanner::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BBanner::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BBanner::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be \"left \"center \"right"));
				
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BBanner::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BBanner::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BBanner::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Variable
 * purpose     : Set/Get the variable to use in the banner
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BBanner::Var(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			return Variable;
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *var = IsKeyword((*inputs)[2]);
		
			if(var)
			{
				var->Contained();
				var->AddRef();
			
				squirrel->interpreter->GlobalVar->RemoveSystemFunc(Variable->Data(),BIND_SET);
				Variable->Alone();
				Variable->RemRef();
				
				Variable = var;
				Variable->AddRef();
				Variable->Contained();
				
				if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
				{
					SQI_Object *value = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
					
					string *str = value->Print();
					View->SetText(str->c_str());
					delete str;	
				}
				else
					View->SetText(NULL);
					
				squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,BannerVarHook);	
			
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BANNER~config","second input must be the word \"get or \"set"));		
}

// some used function

// Fill up the member map
void InitBBanner()
{	
	BannerMemberMap[string("config")] = &SQI_BBanner::Config;
	BannerMemberMap[string("text")] = &SQI_BBanner::Text;
	BannerMemberMap[string("justify")] = &SQI_BBanner::Justify;
	BannerMemberMap[string("enable")] = &SQI_BBanner::Enable;
	BannerMemberMap[string("is.enable")] = &SQI_BBanner::IsEnable;
	BannerMemberMap[string("is.focus")] = &SQI_BBanner::IsFocus;			
	
	BannerSettingMap[string("align")] = &SQI_BBanner::Align;
	BannerSettingMap[string("align.v")] = &SQI_BBanner::AlignVertical;
	BannerSettingMap[string("align.h")] = &SQI_BBanner::AlignHorizontal;
	BannerSettingMap[string("bgcolor")] = &SQI_BBanner::BgColor;
	BannerSettingMap[string("expand")] = &SQI_BBanner::Expand;
	BannerSettingMap[string("expand.x")] = &SQI_BBanner::ExpandX;
	BannerSettingMap[string("expand.y")] = &SQI_BBanner::ExpandY;
	BannerSettingMap[string("font")] = &SQI_BBanner::Font;
	BannerSettingMap[string("low.color")] = &SQI_BBanner::LowColor;
	BannerSettingMap[string("high.color")] = &SQI_BBanner::HighColor;
	BannerSettingMap[string("pad")] = &SQI_BBanner::Pad;
	BannerSettingMap[string("pad.x")] = &SQI_BBanner::PadX;
	BannerSettingMap[string("pad.y")] = &SQI_BBanner::PadY;
	BannerSettingMap[string("variable")] = &SQI_BBanner::Var;			
}

/*
 * function    : IsBanner
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BBanner *, the bview or NULL else
 * side effect : none
 */
SQI_BBanner *IsBanner(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BBANNER)
    	return dynamic_cast<SQI_BBanner *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

void BannerVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel)
{
	SQI_BBanner *banner = static_cast<SQI_BBanner *>(data);
	
	string *str = value->Print();
	
	banner->Lock();
	banner->View->SetText(str->c_str());
	banner->Unlock();
	
	delete str;	
}
