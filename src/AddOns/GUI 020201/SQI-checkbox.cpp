/*
 * Squirrel project
 *
 * file ?	SQI-checkbox.cpp
 * what	?   checkbox object
 * who	?	jlv
 * when	?	12/02/99
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
#include "SQI-bcheckbox.h"
#include "SQI-glue.h"
#include "SQI-checkbox.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_CheckBox[HOOKS_CHECKBOX] = {
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
 * function    : SQI_CheckBox
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
SQI_CheckBox::SQI_CheckBox(SQI_Interp *interp,SQI_BCheckBox *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags)
	:BCheckBox(frame,NULL,label,message,resizingMode,flags) , 
	SQI_Widget(ptr,HOOKS_CHECKBOX)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SHooks = Hooks_CheckBox;
	SetViewColor(BeButtonGrey); // default color
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	 
}


/*
 * function    : ~SQI_CheckBox
 * purpose     : CheckBox destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_CheckBox::~SQI_CheckBox()
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
void SQI_CheckBox::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	
	SetTarget(this,NULL);
		
	BWindow *win = Window();	
	SQI_Window *w = static_cast<SQI_Window *>(win);	
	SQI_BWindow *bw = w->MagicPtr;
		
	MagicPtr->Window = bw;
	Squirrel = bw->Squirrel; 	
			
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
		// attached to a window , we check for the windows default font
		if(!font_set)
			SetFont(&(bw->Font));	
	}
					
	if(Hooks[HOOK_CHECKBOX_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();	
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}		
	
	ResizeToPreferred();
			
	BCheckBox::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing	
	
	ResizeToPreferred();
	
	BCheckBox::AllAttached();	
}

/*
 * function    : AllDetached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::AllDetached(void)
{					
	BCheckBox::AllDetached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_CHECKBOX_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}
	
	MagicPtr->Window = NULL;
	Squirrel = NULL;		
			
	BCheckBox::DetachedFromWindow();	
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
void SQI_CheckBox::Draw(BRect updateRect)
{
	if(Hooks[HOOK_CHECKBOX_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_CHECKBOX_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_DRAW].action,args);		
				
		delete args;
	}		
			
	BCheckBox::Draw(updateRect);			
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
void SQI_CheckBox::KeyDown(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_CHECKBOX_KEYDOWN].action)
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
	
		AddExtra(args,Hooks[HOOK_CHECKBOX_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_KEYDOWN].action,args);
						
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BCheckBox::KeyDown(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BCheckBox::KeyDown(bytes,numBytes);		
		}
	}
	else
		BCheckBox::KeyDown(bytes,numBytes);
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
void SQI_CheckBox::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_CHECKBOX_KEYUP].action)
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
	
		AddExtra(args,Hooks[HOOK_CHECKBOX_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_KEYUP].action,args);
					
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BCheckBox::KeyUp(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BCheckBox::KeyUp(bytes,numBytes);		
		}
	}
	else
		BCheckBox::KeyUp(bytes,numBytes);
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
void SQI_CheckBox::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_CHECKBOX_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BCheckBox::MakeFocus(focused);		
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
void SQI_CheckBox::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case B_SIMPLE_DATA:
		case B_MIME_DATA:
		case B_ARCHIVED_OBJECT:
		{
			if(Hooks[HOOK_CHECKBOX_DROP].action)
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
				
				AddExtra(args,Hooks[HOOK_CHECKBOX_DROP].extra);	
		
				SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_DROP].action,args);
						
				delete args;
				}
			else
				BCheckBox::MessageReceived(message);					
		}
		case WAS_INVOKED:
		{
			Invoked();
			break;	
		}
		default:
			BCheckBox::MessageReceived(message);	
	}		
}

/*
 * function    : Button invoked
 * purpose     : we execute the hook (if any)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::Invoked()
{
	SQI_Number *v;	
	
	if(Value()==B_CONTROL_ON)
		v = Interpreter->True;
	else
		v = Interpreter->False;
		 	
	Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),v,NO_SYSTEM_HOOK);	

	if(Hooks[HOOK_CHECKBOX_INVOKED].action)
	{
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
			
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		args->push_back(v);
		v->AddRef();
	
		AddExtra(args,Hooks[HOOK_CHECKBOX_INVOKED].extra);	
	
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_INVOKED].action,args);
					
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
void SQI_CheckBox::MouseDown(BPoint point)
{
	if(Hooks[HOOK_CHECKBOX_MOUSEDOWN].action)
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
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_MOUSEDOWN].action,args);
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BCheckBox::MouseDown(point);	
		else
		{
			if(ret->IsFalse())
				BCheckBox::MouseDown(point);		
		}	
				
		delete args;
	}
	else
		BCheckBox::MouseDown(point);		
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
void SQI_CheckBox::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_CHECKBOX_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_CHECKBOX_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_CHECKBOX_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_CHECKBOX_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_EXITED].action,args);
						
			delete args;
		}
	}	
	
	BCheckBox::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::Pulse(void)
{
	if(Hooks[HOOK_CHECKBOX_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_PULSE].action,args);
				
		delete args;
	}
	
	BCheckBox::Pulse();
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
void SQI_CheckBox::WindowActivated(bool active)
{
	if(Hooks[HOOK_CHECKBOX_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_CHECKBOX_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_CHECKBOX_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BCheckBox::WindowActivated(active);
} 

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it's preferred size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::ResizeToPreferred()
{
	BCheckBox::ResizeToPreferred();		
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
void SQI_CheckBox::GetPreferredSize(float *width,float *height)
{
		
	// for now we return the original size, but when we will
	// have view attached to it, it will be diferrent	
		
	if(original_width>0)
	{
		if(original_height>0)
		{	
			*width = original_width;
			*height = original_height;
		}
		else
			BCheckBox::GetPreferredSize(width,height);
	}
	else
		BCheckBox::GetPreferredSize(width,height);
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_CheckBox *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_CheckBox::Clone(SQI_CheckBox *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
		
	// now we set the specific setting of a CheckBox
	
	clone->RealBounds = RealBounds;
	clone->original_width = original_width;
	clone->original_height = original_height;

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
	
	// the value now
	
	clone->SetValue(Value());	
	
	// that's all	
}

// other methods

void SQI_CheckBox::SetValue(int32 value)
{
	if(value != Value())
		BCheckBox::SetValue(value);	
}
