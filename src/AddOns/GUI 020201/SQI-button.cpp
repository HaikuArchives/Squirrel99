/*
 * Squirrel project
 *
 * file ?	SQI-view.cpp
 * what	?   view object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbutton.h"
#include "SQI-glue.h"
#include "SQI-button.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_Button[HOOKS_BUTTON] = {
	string("activated"),
	string("attached"),
	string("detached"),
	string("draw"),
	string("drop"),
	string("entered"),
	string("exited"),
	string("focused"),
	string("invoked"),
	string("keydown"),
	string("keyup"),
	string("message"),
	string("mousedown"),
	string("mouseup"),
	string("pulse")
};	
     
/*
 * function    : SQI_Button
 * purpose     : Constructor
 * input       : 
 *
 * BRect frame, view frame
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags
 *
 * output      : none
 * side effect : none
 */
SQI_Button::SQI_Button(SQI_Interp *interp,SQI_BButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags)
	:BButton(frame,NULL,label,message,resizingMode,flags) , 
	SQI_Widget(ptr,HOOKS_BUTTON)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SHooks = Hooks_Button;
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	
	
	makedefault = false; 
}


/*
 * function    : ~SQI_Button
 * purpose     : Button destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Button::~SQI_Button()
{
	MagicPtr->View = NULL;
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	
	BButton::AttachedToWindow();	
	
	SetTarget(this,NULL);
		
	BWindow *win = Window();	
	SQI_Window *w = static_cast<SQI_Window *>(win);	
	SQI_BWindow *bw = w->MagicPtr;
		
	MagicPtr->Window = bw;
	Squirrel = bw->Squirrel; 
	
	if(makedefault)
		win->SetDefaultButton(this);
					
	if(Parent())
	{
		// now we set the bgcolor and the font to the same that the
		// parent if not allready set by the user
		
		if(!hcolor_set)
			SetHighColor(Parent()->HighColor());
		if(!lcolor_set)	
			SetLowColor(Parent()->LowColor());
		if(!bgcolor_set)	
			SetViewColor(Parent()->ViewColor());
		if(!font_set)
		{	
			BFont font;
			Parent()->GetFont(&font);
			SetFont(&font);	
		}	
	}	
	else
	{
		if(!font_set)
			SetFont(&(bw->Font));				
	} 
					
	if(Hooks[HOOK_BUTTON_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();	
			
		AddExtra(args,Hooks[HOOK_BUTTON_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}		
	
	ResizeToPreferred();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing	
	
	ResizeToPreferred();
	
	BButton::AllAttached();	
}

/*
 * function    : AllDetached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::AllDetached(void)
{					
	BButton::AllDetached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_BUTTON_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();	
			
		AddExtra(args,Hooks[HOOK_BUTTON_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}		
			
	MagicPtr->Window = NULL;
	Squirrel = NULL; 			
			
	BButton::DetachedFromWindow();	
}

/*
 * function    : Draw
 * purpose     : Called when the view need to be redraw
 * input       :
 *
 * BRect updateRect, update rect
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::Draw(BRect updateRect)
{
	if(Hooks[HOOK_BUTTON_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_BUTTON_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_DRAW].action,args);		
				
		delete args;
	}		
			
	BButton::Draw(updateRect);			
}

/*
 * function    : KeyDown
 * purpose     : Called when the user press a key in the view
 * input       : 
 *
 * const char *bytes, character code
 * int32 numBytes, ??
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::KeyDown(const char *bytes,int32 numBytes)
{	
	if(Hooks[HOOK_BUTTON_KEYDOWN].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		uint32 c = uint32(*bytes);
		
		key_info modi;
		get_key_info(&modi);		
		SQI_Object *modifiers = GetModifiers(modi.modifiers,MagicPtr->Window->Squirrel->LocalHeap);				
		SQI_String *key = MagicPtr->Window->Squirrel->LocalHeap->String(bytes);
		SQI_Number *code = MagicPtr->Window->Squirrel->LocalHeap->Number((long)c);
		modifiers->AddRef();	
		key->AddRef();
		code->AddRef();	
		
		args->push_back(modifiers);
		args->push_back(key);
		args->push_back(code);
	
		AddExtra(args,Hooks[HOOK_BUTTON_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_KEYDOWN].action,args);
						
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BButton::KeyDown(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BButton::KeyDown(bytes,numBytes);		
		}
	}
	else
		BButton::KeyDown(bytes,numBytes);
}

/*
 * function    : KeyUp
 * purpose     : Called when the user release a key in the view
 * input       : 
 *
 * const char *bytes, character code
 * int32 numBytes, ??
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_BUTTON_KEYUP].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		uint32 c = uint32(*bytes);
		
		key_info modi;
		get_key_info(&modi);		
		SQI_Object *modifiers = GetModifiers(modi.modifiers,MagicPtr->Window->Squirrel->LocalHeap);			
		SQI_String *key = MagicPtr->Window->Squirrel->LocalHeap->String(bytes);
		SQI_Number *code = MagicPtr->Window->Squirrel->LocalHeap->Number((long)c);
		modifiers->AddRef();
		key->AddRef();
		code->AddRef();	
		
		args->push_back(modifiers);
		args->push_back(key);
		args->push_back(code);
	
		AddExtra(args,Hooks[HOOK_BUTTON_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_KEYUP].action,args);
					
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BButton::KeyUp(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BButton::KeyUp(bytes,numBytes);		
		}
	}
	else
		BButton::KeyUp(bytes,numBytes);
}

/*
 * function    : MakeFocus
 * purpose     : The focus is set to the view , or removed from the view
 * input       : 
 *
 * bool focused, true if we have the focus, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_BUTTON_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_BUTTON_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BButton::MakeFocus(focused);		
}

/*
 * function    : MessageReceived
 * purpose     : We receive a message
 * input       : 
 *
 * BMessage *message, message received
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case B_SIMPLE_DATA:
		case B_MIME_DATA:
		case B_ARCHIVED_OBJECT:
		{
			if(Hooks[HOOK_BUTTON_DROP].action)
			{
				SQI_Keyword *nat;
				list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
				args->push_back(MagicPtr);
				MagicPtr->AddRef();
				Window()->DetachCurrentMessage();	
						
				SQI_Message *m = new SQI_Message(MagicPtr->Window->Squirrel,message);	
				m->AddRef();	
					
				if(message->what==B_SIMPLE_DATA)
					nat = MagicPtr->Window->Squirrel->LocalHeap->Keyword("simple");
				else
				if(message->what==B_MIME_DATA)
					nat = MagicPtr->Window->Squirrel->LocalHeap->Keyword("mime");
				else					
					nat = MagicPtr->Window->Squirrel->LocalHeap->Keyword("archive");	
				
				nat->AddRef();
				args->push_back(nat);	
				args->push_back(m);
				
				AddExtra(args,Hooks[HOOK_BUTTON_DROP].extra);	
		
				SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_DROP].action,args);
						
				delete args;
				}
			else
				BButton::MessageReceived(message);					
		}
		case EXEC_BUTTON:
		{
			Invoked();
			break;	
		}
		default:
			BButton::MessageReceived(message);	
	}		
}

/*
 * function    : Button invoked
 * purpose     : we execute the hook (if any)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::Invoked()
{
	if(Hooks[HOOK_BUTTON_INVOKED].action)
	{
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
			
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
	
		AddExtra(args,Hooks[HOOK_BUTTON_INVOKED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_INVOKED].action,args);
					
		delete args;
	}	
}

/*
 * function    : MouseDown
 * purpose     : A button of the mouse has been clicked
 * input       : 
 *
 * BPoint point, position of the mouse
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::MouseDown(BPoint point)
{
	if(Hooks[HOOK_BUTTON_MOUSEDOWN].action)
	{
		// we need to know which button was used
		
		uint32 button = Window()->CurrentMessage()->FindInt32("buttons");	
		
		// we need to create a list of value for the Hook which contain the input of the function	
				
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		key_info modi;
		get_key_info(&modi);		
		SQI_Object *modifiers = GetModifiers(modi.modifiers,MagicPtr->Window->Squirrel->LocalHeap);		
		SQI_Number *x = MagicPtr->Window->Squirrel->LocalHeap->Number(point.x);
		SQI_Number *y = MagicPtr->Window->Squirrel->LocalHeap->Number(point.y);
		SQI_Number *b = MagicPtr->Window->Squirrel->LocalHeap->Number((int)button);
		modifiers->AddRef();	
		x->AddRef();
		y->AddRef();
		b->AddRef();

		args->push_back(modifiers);		
		args->push_back(x);
		args->push_back(y);
		args->push_back(b);
			
		AddExtra(args,Hooks[HOOK_BUTTON_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_MOUSEDOWN].action,args);
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BButton::MouseDown(point);	
		else
		{
			if(ret->IsFalse())
				BButton::MouseDown(point);		
		}	
				
		delete args;
	}
	else
		BButton::MouseDown(point);		
}

/*
 * function    : MouseMoved
 * purpose     : Enter or exit the view
 * input       :
 *
 * BPoint point, where we have entered or exited
 * uint32 transit, what's happening
 * const BMessage *message, message
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_BUTTON_ENTERED].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
			
			list<SQI_Object *> *args = new 	list<SQI_Object *>;
					
			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			SQI_Number *x = MagicPtr->Window->Squirrel->LocalHeap->Number(point.x);
			SQI_Number *y = MagicPtr->Window->Squirrel->LocalHeap->Number(point.y);
			
			x->AddRef();
			y->AddRef();
		
			args->push_back(x);
			args->push_back(y);
				
			AddExtra(args,Hooks[HOOK_BUTTON_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_BUTTON_EXITED].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
			
			list<SQI_Object *> *args = new 	list<SQI_Object *>;
					
			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			SQI_Number *x = MagicPtr->Window->Squirrel->LocalHeap->Number(point.x);
			SQI_Number *y = MagicPtr->Window->Squirrel->LocalHeap->Number(point.y);
			
			x->AddRef();
			y->AddRef();
		
			args->push_back(x);
			args->push_back(y);
				
			AddExtra(args,Hooks[HOOK_BUTTON_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_EXITED].action,args);
						
			delete args;
		}
	}	
	
	BButton::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::Pulse(void)
{
	if(Hooks[HOOK_BUTTON_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_BUTTON_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_PULSE].action,args);
				
		delete args;
	}
	
	BButton::Pulse();
} 

/*
 * function    : WindowActivated
 * purpose     : Executed when the window is activated or deactivated
 * input       : 
 *
 * bool active, trie if active, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::WindowActivated(bool active)
{
	if(Hooks[HOOK_BUTTON_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_BUTTON_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BUTTON_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BButton::WindowActivated(active);
} 

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it's preferred size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::ResizeToPreferred()
{
	BButton::ResizeToPreferred();		
}


/*
 * function    : GetPreferredSize
 * purpose     : Get the Preferred size of the view
 * input       : 
 *
 * float *width, preferred width
 * float *height, preferred height
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::GetPreferredSize(float *width,float *height)
{
	BButton::GetPreferredSize(width,height);	
	
	if(char_width>0 || char_height>0)
	{
		if(char_width>0)
		{	
			string s(char_width,'X');		
			*width = StringWidth(s.c_str()) + 20;
		}
		
		if(char_height>0)
		{
			font_height h;
			GetFontHeight(&h);
		
			*height = char_height * (ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1) - 1 + 10;	
		}	
	}
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_Button *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::Clone(SQI_Button *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a Button
	
	clone->RealBounds = RealBounds;
	clone->original_width = original_width;
	clone->original_height = original_height;
	clone->char_width = char_width;
	clone->char_height = char_height;

	// the font used
	
	BFont font;
	GetFont(&font);					
	clone->SetFont(&font);
	
	// Drawing mode		
	
	clone->SetDrawingMode(DrawingMode());
	
	// color
	
	clone->SetHighColor(HighColor());
	clone->SetLowColor(LowColor());
	clone->SetViewColor(ViewColor());
	
	// default button ?
	
	clone->makedefault = makedefault;
	
	// that's all	
}
