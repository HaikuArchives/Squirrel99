/*
 * Squirrel project
 *
 * file ?	SQI-bmenubar.cpp
 * what	?  	bmenubar object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	10/10/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bmenubar.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-menubar.h"
#include "SQI-bmenu.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include "SQI-menu.h"

#include <MenuItem.h>

BMenuBarMembers MenuBarSettingMap;
BMenuBarMembers MenuBarMemberMap;
  
/*
 * function    : SQI_BMenuBar
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
SQI_BMenuBar::SQI_BMenuBar(SQI_Squirrel *squirrel,menu_layout layout)
	:SQI_BWidget(squirrel,SQI_BMENUBAR)
{	
	View = new SQI_MenuBar(squirrel->interpreter,this,BRect(0,0,0,0),layout);		
	
	Layout = layout;
	Window = NULL;
	Members = &MenuBarMemberMap;
	Settings = &MenuBarSettingMap;	
	Contents = NULL;
			
	SetClass(CLASS_BWIDGET);
}

/*
 * function    : SQI_BMenuBar
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
SQI_BMenuBar::SQI_BMenuBar(SQI_Heap *target,BRect frame,menu_layout layout)
	:SQI_BWidget(target,SQI_BMENUBAR)
{	
	View = new SQI_MenuBar(NULL,this,frame,layout);		
	
	Layout = layout;
	Window = NULL;
	Members = &MenuBarMemberMap;
	Settings = &MenuBarSettingMap;	
	Contents = NULL;
		
	SetClass(CLASS_BWIDGET);
}

/*
 * function    : SQI_BMenuBar
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BMenuBar::~SQI_BMenuBar()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BMenuBar::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{	
 			if(View)
 			{	
 				BMenu *item;	
 				SQI_Menu *menu;	
 					
 				if(Window)
 				{	
 					Lock();	
					View->RemoveSelf();
					Unlock();	
 				}	
									
				for(int32 j=0;j<View->CountItems();j++)
				{
					item = View->SubmenuAt(j);
					if(item)
					{
						menu = dynamic_cast<SQI_Menu *>(item);
						menu->MagicPtr->View = NULL;
						menu->MagicPtr->Alone();
						menu->MagicPtr->RemRef();
					}					
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
void SQI_BMenuBar::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		// now we need to export all the widget 
		
		if(Contents)
		{
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			SQI_BWidget *widget;
	
			for(i=Contents->begin();i!=Contents->end();i++)
			{	
				for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				{
					widget = (*j);
					widget->Export(nheap,force);
				}
			}
		}	
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
SQI_Object *SQI_BMenuBar::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BMenuBar::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BMenuBar *clone = new SQI_BMenuBar(hp,View->Frame(),Layout);
		
		// now we need to "clone" the view itself (set the same way)
		
		View->Clone(clone->View);
		
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
SQI_Object *SQI_BMenuBar::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BMenuBar::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
string *SQI_BMenuBar::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "MenuBar" << '\0';
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
string *SQI_BMenuBar::Show(int prec = 3)
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
string *SQI_BMenuBar::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BMENUBAR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// legacy from SQI_Widget

/*
 * function    : GiveBMenuBar
 * purpose     : Return the BMenuBar
 * input       : none
 * output      : BMenuBar *
 * side effect : none
 */
BView *SQI_BMenuBar::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BMenuBar *
 * side effect : none
 */
SQI_Widget *SQI_BMenuBar::GiveWidget()
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
BRect SQI_BMenuBar::TrueBounds()
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
BRect SQI_BMenuBar::TrueFrame()
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
void SQI_BMenuBar::MoveTo(float x,float y)
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
void SQI_BMenuBar::MoveBy(float w,float h)
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
double SQI_BMenuBar::Area()
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
void SQI_BMenuBar::Expand(float left,float top,float right,float bottom)
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
void SQI_BMenuBar::ResizeToPreferred()
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
void SQI_BMenuBar::SetEnabled(bool state)
{
	enabled = state;
	
	View->SetEnabled(state);	
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
SQI_Object *SQI_BMenuBar::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BMenuBar::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MenuBar~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Add
 * purpose     : Add a new menu to the menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Add(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_BMenu *menu;	
			
			for(int i=0;i<inputs->Length();i++)
			{
				if(menu = IsMenu((*inputs)[i]))
				{
					View->AddItem(dynamic_cast<BMenu *>(menu->GiveBView()));
					menu->Export(squirrel->interpreter->GlobalHeap);
					menu->Contained();
					menu->AddRef();
				}			
			}
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~add","need 1 argument at least"));	
	}
	
	return NULL;	
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
SQI_Object *SQI_BMenuBar::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
		
	return NULL;	
}

/*
 * function    : Remove
 * purpose     : remove a menu from menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Remove(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_BMenu *menu;	
		
			// we need to keep in mind the frame of the menubar before
			
			BRect frame = View->Frame();	
			
			Lock();
			for(int i=0;i<inputs->Length();i++)
			{	
				if(menu = IsMenu((*inputs)[i]))
				{
					// need to check if the menu exists
					if(View->RemoveItem(dynamic_cast<BMenu *>(menu->GiveBView())))
					{
						menu->Alone();
						menu->RemRef();
					}
				}
			}
			Unlock();
			
			// we need to resize the menubar to the previous size now
			
			View->ResizeTo(frame.Width(),frame.Height());			

		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~remove","need 1 argument at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Find
 * purpose     : Find a menu in the menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Find(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_String *label = IsString((*inputs)[0]);
			if(!label)
				label = IsKeyword((*inputs)[0]);					
					
			if(label)
			{
				string *str = label->Data();
				BMenuItem *item;	
							
				Lock();			
				item = View->FindItem(str->c_str());
				Unlock();
			
				if(item)
				{
					if(item->Submenu())
					{
						BMenu *menu = item->Submenu();
						SQI_Menu *m = dynamic_cast<SQI_Menu *>(menu);
						return m->MagicPtr;		
					}
					else
						return squirrel->LocalHeap->Number(-1);
				}
				else
					return squirrel->LocalHeap->Number(-1);	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MenuBar~find","input must be a string/word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~find","need 1 input"));	
	}
	
	return squirrel->LocalHeap->Number(-1);	
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
SQI_Object *SQI_BMenuBar::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BMenuBar::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BMenuBar::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be \"left \"center \"right"));
				
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BMenuBar::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BMenuBar::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMenuBar::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENUBAR~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBMenuBar()
{	
	MenuBarMemberMap[string("add")] = &SQI_BMenuBar::Add;
	MenuBarMemberMap[string("config")] = &SQI_BMenuBar::Config;
	MenuBarMemberMap[string("enable")] = &SQI_BMenuBar::Enable;
	MenuBarMemberMap[string("find")] = &SQI_BMenuBar::Find;
	MenuBarMemberMap[string("is.enable")] = &SQI_BMenuBar::IsEnable;
	MenuBarMemberMap[string("remove")] = &SQI_BMenuBar::Remove;	
	
	MenuBarSettingMap[string("align")] = &SQI_BMenuBar::Align;
	MenuBarSettingMap[string("align.v")] = &SQI_BMenuBar::AlignVertical;
	MenuBarSettingMap[string("align.h")] = &SQI_BMenuBar::AlignHorizontal;
	MenuBarSettingMap[string("bgcolor")] = &SQI_BMenuBar::BgColor;
	MenuBarSettingMap[string("expand")] = &SQI_BMenuBar::Expand;
	MenuBarSettingMap[string("expand.x")] = &SQI_BMenuBar::ExpandX;
	MenuBarSettingMap[string("expand.y")] = &SQI_BMenuBar::ExpandY;
	MenuBarSettingMap[string("font")] = &SQI_BMenuBar::Font;
	MenuBarSettingMap[string("low.color")] = &SQI_BMenuBar::LowColor;
	MenuBarSettingMap[string("high.color")] = &SQI_BMenuBar::HighColor;
	MenuBarSettingMap[string("pad")] = &SQI_BMenuBar::Pad;
	MenuBarSettingMap[string("pad.x")] = &SQI_BMenuBar::PadX;
	MenuBarSettingMap[string("pad.y")] = &SQI_BMenuBar::PadY;			
}

/*
 * function    : IsMenuBar
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BMenuBar *, the bview or NULL else
 * side effect : none
 */
SQI_BMenuBar *IsMenuBar(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BMENUBAR)
    	return dynamic_cast<SQI_BMenuBar *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
