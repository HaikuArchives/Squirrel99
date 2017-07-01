/*
 * Squirrel project
 *
 * file ?	SQI-bframe.cpp
 * what	?  	bframe object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	12/09/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bframe.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-frame.h"
#include "SQI-utils.h"
#include "SQI-font.h"

BFrameMembers FrameSettingMap;
BFrameMembers FrameMemberMap;
  
/*
 * function    : SQI_BFrame
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * char r, relief type
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BFrame::SQI_BFrame(SQI_Squirrel *squirrel,char r,int width,int height,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BFRAME)
{	
	if(width>0 && height>0)
		View = new SQI_Frame(squirrel->interpreter,this,r,BRect(0,0,width,height),resizingMode,flags | B_FRAME_EVENTS);	
	else
	if(width>0)
		View = new SQI_Frame(squirrel->interpreter,this,r,BRect(0,0,width,0),resizingMode,flags | B_FRAME_EVENTS);		
	else
	if(height>0)
		View = new SQI_Frame(squirrel->interpreter,this,r,BRect(0,0,0,height),resizingMode,flags | B_FRAME_EVENTS);	
	else
		View = new SQI_Frame(squirrel->interpreter,this,r,BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS);		
	
	Window = NULL;
	Members = &FrameMemberMap;
	Settings = &FrameSettingMap;	
	Contents = NULL;
	
	SetClass(CLASS_CONTAINER);
}

/*
 * function    : SQI_BFrame
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * char r, relief type
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BFrame::SQI_BFrame(SQI_Heap *target,char r,BRect frame,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(target,SQI_BFRAME)
{	
	View = new SQI_Frame(NULL,this,r,frame,resizingMode,flags | B_FRAME_EVENTS);		
	
	Window = NULL;
	Members = &FrameMemberMap;
	Settings = &FrameSettingMap;	
	Contents = NULL;
	
	SetClass(CLASS_CONTAINER);
}

/*
 * function    : SQI_BFrame
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BFrame::~SQI_BFrame()
{
				
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BFrame::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{ 	
 			BView *v = NULL;		
 			if(View)
 			{ 				
 				Lock();
				View->RemoveSelf();
				Unlock();
				v = View;																					
 			}
 			delete this;
 			if(v)
 				delete v;
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
void SQI_BFrame::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
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
SQI_Object *SQI_BFrame::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BFrame::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BFrame *clone = new SQI_BFrame(hp,View->relief,View->Frame(),View->ResizingMode(),View->Flags());
		
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
SQI_Object *SQI_BFrame::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BFrame::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
string *SQI_BFrame::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Frame" << '\0';
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
string *SQI_BFrame::Show(int prec = 3)
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
string *SQI_BFrame::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BFRAME[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
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
bool SQI_BFrame::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
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
bool SQI_BFrame::UnHook(SQI_Keyword *event)
{
	if(View)
		return View->UnHook(event);
	else
		return false;
}

/*
 * function    : GiveBView
 * purpose     : Return the BView
 * input       : none
 * output      : BView *
 * side effect : none
 */
BView *SQI_BFrame::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BFrame *
 * side effect : none
 */
SQI_Widget *SQI_BFrame::GiveWidget()
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
BRect SQI_BFrame::TrueBounds()
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
BRect SQI_BFrame::TrueFrame()
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
void SQI_BFrame::MoveTo(float x,float y)
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
void SQI_BFrame::MoveBy(float w,float h)
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
double SQI_BFrame::Area()
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
void SQI_BFrame::Expand(float left,float top,float right,float bottom)
{
	if(left || top || right || bottom)
	{		
		// first we move the view to the new position
		
		View->MoveBy(-left,-top);
		
		// now we resize-it
			
		View->ResizeBy(right+left,top+bottom);
		
		// Time to expand the childs also
		
		View->Adapt();	
		
		View->ExpandChilds();
	}
}

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it prefered size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BFrame::ResizeToPreferred()
{
	View->ResizeToPreferred();
}

/*
 * function    : Glue
 * purpose     : Glue a set of widget on the view
 * input       : 
 *
 * char align, alignement of the widgets
 * list<SQI_Widget *> *widgets
 *
 * output      : none
 * side effect : none
 */
void SQI_BFrame::Glue(char align,list<SQI_BWidget *> *widgets)
{
	if(View)
	{	
		SQI_Widget *w;
		BView *widget;
		list<SQI_BWidget *>::iterator i,j;
					
		for(i=widgets->begin();i!=widgets->end();i++)
		{
			(*i)->Contained();
			(*i)->AddRef();
			widget = (*i)->GiveBView();		
			View->AddChild(widget);	
		}			
		
		// now we call the DynamicGluing methods which does the job
		
		Add2Contents(align,widgets);
	}
}	

/*
 * function    : UnGlue
 * purpose     : Unglue a widget from the window
 * input       : 
 *
 * SQI_Widget *widget, widget to unglue
 *
 * output      : none
 * side effect : none
 */
void SQI_BFrame::UnGlue(SQI_BWidget *widget)
{
	if(Contents)
	{
		Lock();
		list<TGlue *>::iterator i;
		list<SQI_BWidget *>::iterator j;
		SQI_BWidget *w;
		TGlue *glue;
		bool found = false;
			
		// we look for the widget in the glue list		
		for(i=Contents->begin();i!=Contents->end();i++)
		{
			glue = *i;
			
			for(j=glue->elements->begin();j!=glue->elements->end();j++)
				if(*j == widget)
				{
					found = true;
					break;
				}	
				
			if(found)
			{
				// we found it , so we erase it
				glue->elements->erase(j);
				
				// if it was the last widget of the gluing sequence, we remove it also
				if(!glue->elements->size())
				{
					Contents->erase(i);
					delete glue;	
				}
				
				break;
			}
		}		
					
		if(found)
		{
			if(!Contents->size())
			{
				delete Contents;
				Contents = NULL;	
			}
			
			// now we need to remove the child
							
			BView *view = widget->GiveBView();
			View->RemoveChild(view);
			widget->Export(View->Interpreter->GlobalHeap);	
			widget->Alone();
			widget->RemRef();	
			// that's all
		}	
		
		Unlock();		
	}	
}	
	
/*
 * function    : Add2Contents
 * purpose     : Add a new gluing sequence
 * input       : 
 *
 * char align, alignement of the widgets
 * list<SQI_Widget *> *widgets
 *
 * output      : none
 * side effect : none
 */
void SQI_BFrame::Add2Contents(char align,list<SQI_BWidget *> *widgets)
{
	if(!Contents)
		Contents = new list<TGlue *>;

	TGlue *pack = new TGlue;
	
	pack->alignment = align;
	pack->elements = widgets;

	Contents->push_back(pack);
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
SQI_Object *SQI_BFrame::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BFrame::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Frame~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Frame~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

/*
 * function    : ReGlue
 * purpose     : ReGlue the frame (dynamicaly)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BFrame::ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(Window)
		{
			Lock();
			View->ReGluing();
			Unlock();
		}	
	}	
	return NULL;	
}

/*
 * function    : Relief
 * purpose     : Set/Get the relief style
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BFrame::Relief(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{	
			SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
			
			if(cmd)
			{
				string *str = cmd->Data();
				if(*str == "get")
				{		
					switch(View->relief)
					{
						case RELIEF_RAISED:
							return squirrel->LocalHeap->Keyword("raised");
						case RELIEF_LOWERED:
							return squirrel->LocalHeap->Keyword("lowered");
						case RELIEF_FLATENED:
							return squirrel->LocalHeap->Keyword("flattened");
						case RELIEF_BORDERED:
							return squirrel->LocalHeap->Keyword("bordered");	
					}
				}	
				else
				if(*str == "set")
				{
					SQI_Keyword *relief = IsKeyword((*inputs)[1]);
					
					if(relief)
					{
						str = relief->Data();
						
						if(*str == "raised")
							View->relief = RELIEF_RAISED;
						else
						if(*str == "lowered")
							View->relief = RELIEF_LOWERED;
						else
						if(*str == "flattened")
							View->relief = RELIEF_FLATENED;
						else
						if(*str == "bordered")
							View->relief = RELIEF_BORDERED;	
						else									
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~relief","second input must be a valid word"));	
							
						if(View->relief==RELIEF_FLATENED)
							View->border =0;		
						else
							View->border = View->level + 2;	
							
						if(Window)
						{
							Lock();
							View->DrawRelief();
							Unlock();
						}
											
						return NULL;			
					}
					else	
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~relief","second input must be a word"));
				}		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~relief","first input must be the word \"get or \"set"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~relief","first input must be the word \"get or \"set"));		

		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~relief","need at least one input"));				
	}
	
	return NULL;
}

/*
 * function    : Widgets
 * purpose     : Output a list of all the widgets glued on the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BFrame::Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_List *lst = squirrel->LocalHeap->List();
		Lock();
		
		if(Contents)
		{
				list<TGlue *>::const_iterator i;
				list<SQI_BWidget *>::const_iterator j;			
				for(i=Contents->begin();i!=Contents->end();i++)						
					for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
						lst->Add2End(*j);
		}	
		
		Unlock();
		return lst;	
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
SQI_Object *SQI_BFrame::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BFrame::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BFrame::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
						v = "top";
					case ALIGN_CENTER:
						v = "center";
					case ALIGN_BOTTOM:
						v = "bottom";
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be \"left \"center \"right"));
				
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BFrame::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BFrame::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				
				View->ColoringBasis(color);
				
				View->Invalidate();
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BFrame::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Level
 * purpose     : Set/Get the relief level
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BFrame::Level(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return squirrel->LocalHeap->Number(View->level);
		else
		if(*str == "set")
		{
			SQI_Number *size = IsNumber((*inputs)[2]);
			
			if(size)
			{
				View->level = Number2Int(size);
				View->border = View->level + 2;
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","third input must be a number"));
			
			return NULL;	
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FRAME~config","second input must be the word \"get or \"set"));		
}

// some used function

// Fill up the member map
void InitBFrame()
{	
	FrameMemberMap[string("config")] = &SQI_BFrame::Config;
	FrameMemberMap[string("relief")] = &SQI_BFrame::Relief;
	FrameMemberMap[string("reglue")] = &SQI_BFrame::ReGlue;
	FrameMemberMap[string("enable")] = &SQI_BFrame::Enable;
	FrameMemberMap[string("is.enable")] = &SQI_BFrame::IsEnable;
	FrameMemberMap[string("widgets")] = &SQI_BFrame::Widgets;
	FrameMemberMap[string("is.focus")] = &SQI_BFrame::IsFocus;	
	
	FrameSettingMap[string("align")] = &SQI_BFrame::Align;
	FrameSettingMap[string("align.v")] = &SQI_BFrame::AlignVertical;
	FrameSettingMap[string("align.h")] = &SQI_BFrame::AlignHorizontal;
	FrameSettingMap[string("bgcolor")] = &SQI_BFrame::BgColor;
	FrameSettingMap[string("expand")] = &SQI_BFrame::Expand;
	FrameSettingMap[string("expand.x")] = &SQI_BFrame::ExpandX;
	FrameSettingMap[string("expand.y")] = &SQI_BFrame::ExpandY;
	FrameSettingMap[string("font")] = &SQI_BFrame::Font;
	FrameSettingMap[string("level")] = &SQI_BFrame::Level;
	FrameSettingMap[string("low.color")] = &SQI_BFrame::LowColor;
	FrameSettingMap[string("high.color")] = &SQI_BFrame::HighColor;
	FrameSettingMap[string("pad")] = &SQI_BFrame::Pad;
	FrameSettingMap[string("pad.x")] = &SQI_BFrame::PadX;
	FrameSettingMap[string("pad.y")] = &SQI_BFrame::PadY;			
}

/*
 * function    : IsFrame
 * purpose     : Test if an object is a bframe
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BView *, the bview or NULL else
 * side effect : none
 */
SQI_BFrame *IsFrame(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BFRAME)
    	return dynamic_cast<SQI_BFrame *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

