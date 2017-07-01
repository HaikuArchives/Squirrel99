/*
 * Squirrel project
 *
 * file ?	sqi-bwindow.cpp
 * what	?   BWindow object
 * who	?	jlv
 * when	?	11/23/99
 * last	?	03/07/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Button.h> 
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include "SQI-bbutton.h"

BWindowMembers WindowSettingMap;
BWindowMembers WindowMemberMap;

/*
 * function    : SQI_BWindow
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * BRect frame, frame of the window
 * char *title, title of the window
 * window_type type, type of the window
 * int32 flags, flags
 *
 * output      : none
 * side effect : none
 */
SQI_BWindow::SQI_BWindow(SQI_Squirrel *squirrel,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags)
	:SQI_ThePointer(squirrel,SQI_BWINDOW)
{	
	SetClass(CLASS_BWINDOW);
	
	// each window as it own squirrel	
	Squirrel =  new SQI_Squirrel();
	
	Squirrel->SameGCConfig(squirrel);
	Squirrel->SamePrecision(squirrel);
		
	// We set the interpreter
	Interpreter = squirrel->interpreter;
	Squirrel->interpreter = Interpreter;
	
	Window = new SQI_Window(this,frame,title,look,feel,flags);
		
	Members = &WindowMemberMap;
	Settings = &WindowSettingMap;
	
	Contents = NULL;
	
	Font = *be_plain_font;
}

/*
 * function    : SQI_BWindow
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to create the window
 * BRect frame, frame of the window
 * char *title, title of the window
 * window_type type, type of the window
 * int32 flags, flags
 *
 * output      : none
 * side effect : none
 */
SQI_BWindow::SQI_BWindow(SQI_Heap *target,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags)
	:SQI_ThePointer(target,SQI_BWINDOW)
{	
	SetClass(CLASS_BWINDOW);
	
	// each window as it own squirrel	
	Squirrel =  new SQI_Squirrel();
				
	Window = new SQI_Window(this,frame,title,look,feel,flags);
	
	Members = &WindowMemberMap;
	Settings = &WindowSettingMap;
	
	Contents = NULL;
	
	Squirrel->thread = Window->Thread();
}


/*
 * function    : SQI_BWindow
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BWindow::~SQI_BWindow()
{
	if(Contents)
	{	
		Contents->clear();
		delete Contents;
	}		
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BWindow::Suicidal(bool force=false)
{		
	if(status || force)
 		if(!REF)
 		{			
			Terminate();												 			 			 			
 			if(Window)
 			{	
 				Lock();
 				Window->MagicPtr = NULL;
				Window->PostMessage(B_QUIT_REQUESTED);
				Unlock();
 			}	
 								
			delete this;	
			return true;
 		}
 	
 	return false;
}

void SQI_BWindow::Terminate()
{
	static bool done = false;	
	
	if(!done)
	{	
		done = true;
	
		if(Window)
		 {
		 	// we remove all the widget attached to the window
		 	Window->Lock();	
			BView *child = Window->ChildAt(0);
			BView *next;
			while(child)
			{
				next = child->NextSibling();
				child->RemoveSelf();
				child = next;
			}
			Window->Unlock();	
		 }	
		 			 		
		if(Contents)
		{		
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			TGlue *elem;	
			
			for(i=Contents->begin();i!=Contents->end();i++)
			{
				elem = *i;
				// we remref every widgets
				for(j=elem->elements->begin();j!=elem->elements->end();j++)
				{
					(*j)->Alone();
					(*j)->RemRef();
				}	
				elem->elements->clear();
				delete elem->elements;
				delete elem;
			}	
			
			Contents->clear();
			delete Contents;
			Contents = NULL;
		}
	}	
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
SQI_Object *SQI_BWindow::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BWindow::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
string *SQI_BWindow::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(Window)	
		out << Window->Title() << '\0';
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
string *SQI_BWindow::Show(int prec = 3)
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
string *SQI_BWindow::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BWINDOW[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

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
bool SQI_BWindow::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	if(Window)
		return Window->Hook(event,action,extra);
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
bool SQI_BWindow::UnHook(SQI_Keyword *event)
{
	if(Window)
		return Window->UnHook(event);
	else
		return false;
}

// Member Methods

/*
 * function    : Activate
 * purpose     : Activate the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Activate(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->Activate(true);
	return NULL;	
}

/*
 * function    : Add2Subset
 * purpose     : Add a window to the window's subset
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Add2Subset(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==1)
		{
			SQI_BWindow *win = IsWindow((*inputs)[0]);
			if(win)
			{		
				Lock();		
				Window->AddToSubset(win->Window);
				Unlock();	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~add.to.subset","input must be a window"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~add.to.subset","need one input"));				
	}
	
	return NULL;	
}

/*
 * function    : Rem2Subset
 * purpose     : Removed a window from the window's subset
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Rem2Subset(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==1)
		{
			SQI_BWindow *win = IsWindow((*inputs)[0]);
			if(win)
			{		
				Lock();		
				Window->RemoveFromSubset(win->Window);
				Unlock();	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~rem.from.subset","input must be a window"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~rem.from.subset","need one input"));				
	}
	
	return NULL;	
}

/*
 * function    : DeActivate
 * purpose     : DeActivate the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::DeActivate(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->Activate(false);
	return NULL;	
}

/*
 * function    : Show
 * purpose     : Show the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Show(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		Window->Lock();
		Window->Show();
		Squirrel->thread = Window->Thread();
		Window->Unlock();
	}	
	return NULL;	
}

/*
 * function    : ReGlue
 * purpose     : ReGlue the window (staticaly)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		Window->Lock();
		Window->ReGluing();
		Window->Unlock();
	}	
	return NULL;	
}

/*
 * function    : Enable
 * purpose     : Enable/Disable all the widget in the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window && Contents)
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
				list<TGlue *>::const_iterator i;
				list<SQI_BWidget *>::const_iterator j;			
				for(i=Contents->begin();i!=Contents->end();i++)
				{						
					for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
						(*j)->SetEnabled(s);
				}	
				Unlock();	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~enable","input must be a boolean"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~enable","need one input"));
	}	
	
	return NULL;	
}


/*
 * function    : Bounds
 * purpose     : Return the Bounds of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Bounds(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		return Frame2List(Squirrel,Window->Bounds());
	else
		return Interpreter->False;	
}

/*
 * function    : Close
 * purpose     : Close the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Close(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->PostMessage(B_QUIT_REQUESTED);	
	return NULL;	
}

/*
 * function    : Frame
 * purpose     : Return the Frame of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Frame(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		return Frame2List(Squirrel,Window->Frame());
	else
		return Interpreter->False;	
}

/*
 * function    : Center
 * purpose     : Return the center of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Center(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
			SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
			
			if(cmd)
			{
				string *str = cmd->Data();
				if(*str == "get")
				{
					BPoint p;
					int mw,mh;
					BRect f = Window->Frame();
				
					mw = f.IntegerWidth() / 2;
					mh = f.IntegerHeight() / 2;
					
					p = f.LeftTop();
					p.x += mw;
					p.y += mh;	
					
					return Point2List(squirrel,&p);
				}
				else
				if(*str == "set")
				{
					if(inputs->Length()==2)
					{
						SQI_List *lst = IsList((*inputs)[1]);
						
						if(lst)
						{
							BPoint p;
							int mw,mh;
							BRect f = Window->Frame();
							
							p = List2Point(lst);
							
							mw = f.IntegerWidth() / 2;
							mh = f.IntegerHeight() / 2;
							
							p.x -= mw;
							p.y -= mh;
							
							Window->Lock();
							Window->MoveTo(p);
							Window->Unlock();						
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","need a list as third input"));
										
						return NULL;
					}
					else	
						throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~center","need 2 arguments when using \"set"));		
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~center","first input must be the word \"get or \"set"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~center","first input must be the word \"get or \"set"));		
	}	
	else
		return Interpreter->False;	
}

/*
 * function    : Hide
 * purpose     : Hide the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Hide(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->Hide();
	return NULL;	
}

/*
 * function    : IsActive
 * purpose     : return true if the window is active
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::IsActive(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(Window->IsActive())
			return Interpreter->True;
		else	
			return Interpreter->False;
	}
	else
		return Interpreter->False;	
}

/*
 * function    : IsFront
 * purpose     : return true if the window is the frontmost
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::IsFront(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(Window->IsFront())
			return Interpreter->True;
		else	
			return Interpreter->False;
	}
	else
		return Interpreter->False;	
}

/*
 * function    : IsHidden
 * purpose     : return true if the window is off screen
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::IsHidden(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		Window->Lock();
		if(Window->IsHidden())
		{
			Window->Unlock();
			return Interpreter->True;
		}
		else	
		{
			Window->Unlock();
			return Interpreter->False;
		}	
	}
	else
		return Interpreter->False;	
}

/*
 * function    : Minimize
 * purpose     : Minimize the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Minimize(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->Minimize(true);
	return NULL;	
}

/*
 * function    : UnMinimize
 * purpose     : UnMinimize the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::UnMinimize(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
		Window->Minimize(false);
	return NULL;	
}

/*
 * function    : MoveBy
 * purpose     : Move the window by a spcified amount of pixel
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::MoveBy(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==2)
		{
			SQI_Number *horizontal = IsNumber((*inputs)[0]);
			SQI_Number *vertical = IsNumber((*inputs)[1]);
			
			if(horizontal && vertical)
				Window->MoveBy(Number2Float(horizontal),Number2Float(vertical));
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~move.by","need 2 numbers"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~move.by","need 2 arguments"));	
	}
	
	return NULL;	
}

/*
 * function    : MoveTo
 * purpose     : Move the window to a spcified position
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::MoveTo(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==2)
		{
			SQI_Number *x = IsNumber((*inputs)[0]);
			SQI_Number *y = IsNumber((*inputs)[1]);
			
			if(x && y)
				Window->MoveTo(Number2Float(x),Number2Float(y));
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~move.to","need 2 numbers"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~move.to","need 2 arguments"));	
	}
	
	return NULL;	
}

/*
 * function    : ResizeBy
 * purpose     : Resize the window by a spcified amount of pixel
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::ResizeBy(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==2)
		{
			SQI_Number *horizontal = IsNumber((*inputs)[0]);
			SQI_Number *vertical = IsNumber((*inputs)[1]);
			
			if(horizontal && vertical)
			{
				Window->Lock();
				Window->ResizeBy(Number2Float(horizontal),Number2Float(vertical));
				Window->StaticGluing();
				Window->Unlock();	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~resize.by","need 2 numbers"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~resize.by","need 2 arguments"));	
	}
	
	return NULL;	
}

/*
 * function    : ResizeTo
 * purpose     : Move the window to a spcified size
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::ResizeTo(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()==2)
		{
			SQI_Number *x = IsNumber((*inputs)[0]);
			SQI_Number *y = IsNumber((*inputs)[1]);
			
			if(x && y)
			{
				Window->Lock();
				Window->ResizeTo(Number2Float(x),Number2Float(y));
				Window->StaticGluing();
				Window->Unlock();					
			}	
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~resize.to","need 2 numbers"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~resize.to","need 2 arguments"));	
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
SQI_Object *SQI_BWindow::Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		SQI_List *lst = new SQI_List(squirrel->LocalHeap);
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

/*
 * function    : Config
 * purpose     : Set/Get the configuration of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Window)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BWindow::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
				Setting = (*Settings)[*key];
			
				if(Setting)	
					return (this->*Setting)(squirrel,inputs);
				else
				{
					Settings->erase(*key);
					throw(new SQI_Exception(SQI_EX_INVALIDE,key->c_str(),"unknow setting"));
				}			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

// Setting methods

/*
 * function    : Constraint
 * purpose     : set/get the constraint behavior of the window
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Constraint(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			if(Window->constraint==AUTO)
				return new SQI_Keyword(squirrel->LocalHeap,"auto");
			else
			if(Window->constraint==NONE)
				return new SQI_Keyword(squirrel->LocalHeap,"none");
			else			
			if(Window->constraint==MANUAL)
				return new SQI_Keyword(squirrel->LocalHeap,"manual");
		}	
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_String *cons = IsKeyword((*inputs)[2]);
				if(!cons)
					cons = IsString((*inputs)[2]);
				if(cons)
				{
					string *str = cons->Data();
					
					if(*str == "auto")
						Window->constraint=AUTO;
					else
					if(*str == "none")
						Window->constraint=NONE;
					else			
					if(*str == "manual")
					{
						Window->constraint=MANUAL;
						Window->SetSizeLimits(Window->minh,Window->maxw,Window->minh,Window->maxh);
					}	
					else
						throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","third input must be a valid word"));
				}	
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","third input must be a word"));
								
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
}

/*
 * function    : Limit
 * purpose     : Set/Get the window size limit
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Limit(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	SQI_Keyword *one = IsKeyword((*inputs)[2]);
	
	if(cmd && one)
	{
		string *o = one->Data();	
		string *str = cmd->Data();
		if(*str == "get")
		{
			if(*o == "max")
			{
				SQI_List *lst = new SQI_List(squirrel->LocalHeap);	
			
				lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->maxw));
				lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->maxh));	
			
				return lst;	
			}
			else
			if(*o == "min")
			{
				SQI_List *lst = new SQI_List(squirrel->LocalHeap);	
			
				lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->minw));
				lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->minh));	
			
				return lst;				
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","third input must be the word \"max ir \"min"));							
		}
		else
		if(*str == "set")
		{
			if(inputs->Length()==4)
			{
				float w,h;
				SQI_List *lst;
				
				lst = IsList((*inputs)[3]);
				if(lst)
					List2Pad(lst,&w,&h); // reuse a function that normaly is used for widget padding :)	
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","fourth input must be a list"));
				
				if(*o == "max")
				{
					Window->maxw = w;
					Window->maxh = h;	
				}
				else
				if(*o == "min")
				{
					Window->minw = w;
					Window->minh = h;	
				}
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","third input must be the word \"max ir \"min"));							
				
				if(Window->constraint==MANUAL)
					Window->SetSizeLimits(Window->minh,Window->maxw,Window->minh,Window->maxh);
				
				return NULL;			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need a fourth input"));				
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));				
}

/*
 * function    : Focus
 * purpose     : Set/Get the window focus
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Focus(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			BView *v = Window->CurrentFocus();
			if(v)
			{	
				SQI_Widget *focus = dynamic_cast<SQI_Widget *>(v);
				return focus->BWidget;	
			}
			else
				return squirrel->interpreter->False;		
		}
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_BWidget *focus = IsWidget((*inputs)[2]);
				if(focus)
					focus->MakeFocus(true);	
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","focus must be a widget"));
								
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));				
}

/*
 * function    : DefaultButton
 * purpose     : Set/Get the default button
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::DefaultButton(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			BView *v = Window->DefaultButton();
			if(v)
			{	
				SQI_Widget *def = dynamic_cast<SQI_Widget *>(v);
				return def->BWidget;	
			}
			else
				return squirrel->interpreter->False;		
		}
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_BButton *def = IsButton((*inputs)[2]);
				if(def)
				{
					Lock();
					BButton *view = dynamic_cast<BButton *>(def->GiveBView());
					Window->SetDefaultButton(view);
					Unlock();	
				}		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","default button must be a button object"));				
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));			
}

/*
 * function    : PulseRate
 * purpose     : Set/Get the window pulse rate
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::PulseRate(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_Number(squirrel->LocalHeap,(long)(Window->PulseRate()));
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_Number *rate = IsInteger((*inputs)[2]);
				if(rate)
					Window->SetPulseRate(Number2Long(rate));		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","pulse rate must be an integer"));			
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
}

/*
 * function    : Title
 * purpose     : Set/Get the window title
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Title(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_String(squirrel->LocalHeap,Window->Title());
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_String *title = IsString((*inputs)[2]);
				if(!title)
					title = IsKeyword((*inputs)[2]);	
				if(title)
					Window->SetTitle(title->Data()->c_str());		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","title must be a string or a word"));
								
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
}

/*
 * function    : Look
 * purpose     : Set/Get the window look
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Look(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			const window_look type = Window->Look();	
			switch(type)
			{	
				case B_MODAL_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"modal");
				case B_BORDERED_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"bordered");
				case B_TITLED_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"titled");
				case B_DOCUMENT_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"document");
				case B_NO_BORDER_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"no.bordered");
				case B_FLOATING_WINDOW_LOOK:
					return new SQI_Keyword(squirrel->LocalHeap,"floating");	
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *type = IsKeyword((*inputs)[2]);
			
			if(type)
			{
				string *look = type->Data();
				
				if(*look == "modal")
					Window->SetLook(B_MODAL_WINDOW_LOOK);
				else
				if(*look == "bordered")
					Window->SetLook(B_BORDERED_WINDOW_LOOK);
				else
				if(*look == "titled")
					Window->SetLook(B_TITLED_WINDOW_LOOK);
				else
				if(*look == "document")
					Window->SetLook(B_DOCUMENT_WINDOW_LOOK);
				else
				if(*look == "no.bordered")
					Window->SetLook(B_NO_BORDER_WINDOW_LOOK);
				else
				if(*look == "floating")
					Window->SetLook(B_FLOATING_WINDOW_LOOK);
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","look must be a valide word"));					
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","third input must be a word"));
			return NULL;		
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
}

/*
 * function    : Feel
 * purpose     : Set/Get the window feel
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Feel(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			const window_feel type = Window->Feel();	
			switch(type)
			{	
				case B_NORMAL_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"normal");
				case B_MODAL_SUBSET_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"modal.subset");
				case B_MODAL_APP_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"modal.app");
				case B_MODAL_ALL_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"modal.all");
				case B_FLOATING_SUBSET_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"floating.subset");
				case B_FLOATING_APP_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"floating.app");
				case B_FLOATING_ALL_WINDOW_FEEL:
					return new SQI_Keyword(squirrel->LocalHeap,"floating.all");	
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *type = IsKeyword((*inputs)[2]);
			
			if(type)
			{
				string *feel = type->Data();
				
				if(*feel == "normal")
					Window->SetFeel(B_NORMAL_WINDOW_FEEL);
				else
				if(*feel == "modal.subset")
					Window->SetFeel(B_MODAL_SUBSET_WINDOW_FEEL);
				else
				if(*feel == "modal.app")
					Window->SetFeel(B_MODAL_APP_WINDOW_FEEL);
				else
				if(*feel == "modal.all")
					Window->SetFeel(B_MODAL_ALL_WINDOW_FEEL);
				else
				if(*feel == "floating.subset")
					Window->SetFeel(B_FLOATING_SUBSET_WINDOW_FEEL);
				else
				if(*feel == "floating.app")
					Window->SetFeel(B_FLOATING_APP_WINDOW_FEEL);
				else
				if(*feel == "floating.all")
					Window->SetFeel(B_FLOATING_ALL_WINDOW_FEEL);
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","feel must be a valide word"));					
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","third input must be a word"));
			return NULL;		
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
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
SQI_Object *SQI_BWindow::DefaultFont(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")		
			return new SQI_Font(squirrel,&Font);	
		else
		if(*str == "set")
		{
			SQI_Font *font = IsFont((*inputs)[2]);
			
			if(font)
			{
				Font = font;
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Workspace
 * purpose     : Set/Get the workspace
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Workspace(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	return NULL;		
}

/*
 * function    : Zoom
 * purpose     : Set/Get the window zoom limits
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BWindow::Zoom(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_List *lst = new SQI_List(squirrel->LocalHeap);
			
			lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->zoom_max_w));
			lst->Add2End(new SQI_Number(squirrel->LocalHeap,Window->zoom_max_h));
			
			return lst;	
		}
		else
		if(*str == "set")
		{
			if(inputs->Length()==3)
			{
				SQI_List *lst = IsList((*inputs)[2]);
				if(lst)
				{
					if(lst->Length()!=2)
						throw(new SQI_Exception(SQI_EX_INVALIDE,"WINDOW~config","list must have a size of 2"));
					
					list<SQI_Object *>::const_iterator *j = lst->Iterator();
		
					SQI_Number *maxw = IsNumber(*(*j));
					(*j)++;
					SQI_Number *maxh = IsNumber(*(*j));
							
					delete j;	
					
					Window->zoom_max_h = Number2Float(maxh);
					Window->zoom_max_w = Number2Float(maxw);
					Window->SetZoomLimits(Window->zoom_max_w,Window->zoom_max_h);	
				}		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","third input must be a list"));
								
				return NULL;
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"WINDOW~config","need 3 arguments when using \"set"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"WINDOW~config","second input must be the word \"get or \"set"));	
}

/*
 * function    : Glue
 * purpose     : Glue a set of widget on the window
 * input       : 
 *
 * char align, alignement of the widgets
 * list<SQI_Widget *> *widgets
 *
 * output      : none
 * side effect : none
 */
void SQI_BWindow::Glue(char align,list<SQI_BWidget *> *widgets)
{
	if(Window)
	{	
		BView *widget;
		BView *sibling = NULL;
		list<SQI_BWidget *>::const_iterator i;
		SQI_BWidget *w;	
					
		for(i=widgets->begin();i!=widgets->end();i++)
		{
			w = *i;
			w->Export(Squirrel->LocalHeap);
			w->AddRef();
			w->Contained();
			widget = w->GiveBView();
			Window->AddChild(widget,NULL);	
		}			
		
		// now we call the DynamicGluing methods which does the job
		
		Window->Lock();
		Add2Contents(align,widgets);
		Window->DynamicGluing(align,widgets);
		Window->Unlock();
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
void SQI_BWindow::UnGlue(SQI_BWidget *widget)
{
	if(Window)
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
				Window->RemoveChild(view);
				widget->Export(Interpreter->GlobalHeap);	
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
void SQI_BWindow::Add2Contents(char align,list<SQI_BWidget *> *widgets)
{
	if(!Contents)
		Contents = new list<TGlue *>;

	TGlue *pack = new TGlue;
	
	pack->alignment = align;
	pack->elements = widgets;

	Contents->push_back(pack);
}

/*
 * function    : Lock
 * purpose     : Lock the window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BWindow::Lock()
{
	if(Window)
		Window->Lock();		
}

/*
 * function    : Unlock
 * purpose     : Unlock the window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BWindow::Unlock()
{
	if(Window)
		Window->Unlock();		
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
SQI_Object *SQI_BWindow::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BWindow::DeepClone(SQI_Heap *target = NULL)
{
	if(Window)
	{	
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
	
		// first we create the SQI_BWindow	
	
		SQI_BWindow *clone = new SQI_BWindow(hp,Window->Frame(),Window->Title(),Window->Look(),Window->Feel(),Window->Flags());		
	
		// We set the interpreter for the clone
		clone->Interpreter = Interpreter;
		clone->Squirrel->interpreter = Interpreter;
		
		clone->Squirrel->SameGCConfig(Squirrel);
		clone->Squirrel->SamePrecision(Squirrel);
						
		// now we need to "clone" the Window itself (set the same way)
		
		Window->Clone(clone->Window);
		
		// now we need to clone all the view attached to this window
		
		if(Contents)
		{
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			SQI_BWidget *widget;
			SQI_Widget *w;
			TGlue *glue;
			BButton *def = Window->DefaultButton();	
		
			clone->Contents = new list<TGlue *>;		
				
			for(i=Contents->begin();i!=Contents->end();i++)
			{
				glue = new TGlue;
				
				glue->alignment = (*i)->alignment;
				glue->elements = new list<SQI_BWidget *>;
				
				for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				{
					w = (*j)->GiveWidget();	
					widget = dynamic_cast<SQI_BWidget *>((*j)->DeepClone(hp));
					widget->AddRef();	
					glue->elements->push_back(widget);
					
					// we add the view to the window	
					
					clone->Window->AddChild(widget->GiveBView());
					
					BView *v = widget->GiveBView();
					BButton *d = dynamic_cast<BButton *>(v);	
					
					// did the widget is the default button ?
					if(d == def)
					{
						// yes so we need to set it also in the clone
						clone->Window->SetDefaultButton(d);	
					}
				}		
				
				clone->Contents->push_back(glue);	
			}
		}		
		
		// now we going to set some properties of the new window
		
		// sizelimits
	
		float minw,maxw,minh,maxh;
		
		Window->GetSizeLimits(&minw,&maxw,&minh,&maxh);
		clone->Window->SetSizeLimits(minw,maxw,minh,maxh);
	
		// pulserate	
		
		clone->Window->SetPulseRate(Window->PulseRate());	
		
		// workspace	
		
		clone->Window->SetWorkspaces(Window->Workspaces());
	
		// we now move the window to not overlap totaly the original
		
		clone->Window->MoveBy(20,20);
		
		// and the font
				
		clone->Font = Font;		
	
		// and we're done	
	
		return clone;
	}
	else
		return NULL;
}

// some used function

// Fill up the member map
void InitBWindow()
{	
	WindowMemberMap[string("activate")] = &SQI_BWindow::Activate;
	WindowMemberMap[string("add.to.subset")] = &SQI_BWindow::Add2Subset;
	WindowMemberMap[string("rem.from.subset")] = &SQI_BWindow::Rem2Subset;
	WindowMemberMap[string("bounds")] = &SQI_BWindow::Bounds;
	WindowMemberMap[string("close")] = &SQI_BWindow::Close;
	WindowMemberMap[string("quit")] = &SQI_BWindow::Close;
	WindowMemberMap[string("config")] = &SQI_BWindow::Config;
	WindowMemberMap[string("deactivate")] = &SQI_BWindow::DeActivate;
	WindowMemberMap[string("enable")] = &SQI_BWindow::Enable;
	WindowMemberMap[string("frame")] = &SQI_BWindow::Frame;
	WindowMemberMap[string("hide")] = &SQI_BWindow::Hide;
	WindowMemberMap[string("is.active")] = &SQI_BWindow::IsActive;
	WindowMemberMap[string("is.front")] = &SQI_BWindow::IsFront;
	WindowMemberMap[string("is.hidden")] = &SQI_BWindow::IsHidden;
	WindowMemberMap[string("minimize")] = &SQI_BWindow::Minimize;
	WindowMemberMap[string("move.to")] = &SQI_BWindow::MoveTo;
	WindowMemberMap[string("move.by")] = &SQI_BWindow::MoveBy;
	WindowMemberMap[string("resize.to")] = &SQI_BWindow::ResizeTo;
	WindowMemberMap[string("resize.by")] = &SQI_BWindow::ResizeBy;
	WindowMemberMap[string("reglue")] = &SQI_BWindow::ReGlue;
	WindowMemberMap[string("unminimize")] = &SQI_BWindow::UnMinimize;
	WindowMemberMap[string("show")] = &SQI_BWindow::Show;
	WindowMemberMap[string("widgets")] = &SQI_BWindow::Widgets;
	WindowMemberMap[string("center")] = &SQI_BWindow::Center;
	
	WindowSettingMap[string("constraint")] = &SQI_BWindow::Constraint;
	WindowSettingMap[string("limit")] = &SQI_BWindow::Limit;
	WindowSettingMap[string("focus")] = &SQI_BWindow::Focus;
	WindowSettingMap[string("font")] = &SQI_BWindow::DefaultFont;
	WindowSettingMap[string("defaultbutton")] = &SQI_BWindow::DefaultButton;
	WindowSettingMap[string("pulserate")] = &SQI_BWindow::PulseRate;
	WindowSettingMap[string("title")] = &SQI_BWindow::Title;	
	WindowSettingMap[string("look")] = &SQI_BWindow::Look;
	WindowSettingMap[string("feel")] = &SQI_BWindow::Feel;
	WindowSettingMap[string("zoom")] = &SQI_BWindow::Zoom;
	WindowSettingMap[string("workspace")] = &SQI_BWindow::Workspace;			
}

/*
 * function    : IsWindow
 * purpose     : Test if an object is a bwindow
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BWindow *, the bwindow or NULL if not of this type
 * side effect : none
 */
SQI_BWindow *IsWindow(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BWINDOW)
    	return dynamic_cast<SQI_BWindow *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

