/*
 * Squirrel project
 *
 * file ?	SQI-colorcontrol.cpp
 * what	?   colorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	12/07/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bcolorcontrol.h"
#include "SQI-glue.h"
#include "SQI-colorcontrol.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_ColorControl[HOOKS_COLORCONTROL] = {
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
 * function    : SQI_ColorControl
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
SQI_ColorControl::SQI_ColorControl(SQI_Interp *interp,SQI_BColorControl *ptr,color_control_layout matrix,float cellSide,BMessage *message,BPoint lefttop)
	:BColorControl(lefttop,matrix,cellSide,NULL,message) , 
	SQI_Widget(ptr,HOOKS_COLORCONTROL)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SHooks = Hooks_ColorControl;
	SetViewColor(BeButtonGrey); // default color
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	 
}


/*
 * function    : ~SQI_ColorControl
 * purpose     : ColorControl destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_ColorControl::~SQI_ColorControl()
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
void SQI_ColorControl::AttachedToWindow(void)
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
					
	if(Hooks[HOOK_COLORCONTROL_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}		
	
	ResizeToPreferred();
			
	BColorControl::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing	
	
	ResizeToPreferred();
	
	BColorControl::AllAttached();	
}

/*
 * function    : AllDetached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::AllDetached(void)
{					
	BColorControl::AllDetached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_COLORCONTROL_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}	
	
	MagicPtr->Window = NULL;
	Squirrel = NULL;	
			
	BColorControl::DetachedFromWindow();	
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
void SQI_ColorControl::Draw(BRect updateRect)
{
	if(Hooks[HOOK_COLORCONTROL_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_COLORCONTROL_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_DRAW].action,args);		
				
		delete args;
	}		
			
	BColorControl::Draw(updateRect);			
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
void SQI_ColorControl::KeyDown(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_COLORCONTROL_KEYDOWN].action)
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
	
		AddExtra(args,Hooks[HOOK_COLORCONTROL_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_KEYDOWN].action,args);
						
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BColorControl::KeyDown(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BColorControl::KeyDown(bytes,numBytes);		
		}
	}
	else
		BColorControl::KeyDown(bytes,numBytes);
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
void SQI_ColorControl::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_COLORCONTROL_KEYUP].action)
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
	
		AddExtra(args,Hooks[HOOK_COLORCONTROL_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_KEYUP].action,args);
					
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BColorControl::KeyUp(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BColorControl::KeyUp(bytes,numBytes);		
		}
	}
	else
		BColorControl::KeyUp(bytes,numBytes);
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
void SQI_ColorControl::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_COLORCONTROL_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BColorControl::MakeFocus(focused);		
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
void SQI_ColorControl::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case B_SIMPLE_DATA:
		case B_MIME_DATA:
		case B_ARCHIVED_OBJECT:
		{
			if(Hooks[HOOK_COLORCONTROL_DROP].action)
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
				
				AddExtra(args,Hooks[HOOK_COLORCONTROL_DROP].extra);	
		
				SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_DROP].action,args);
						
				delete args;
				}
			else
				BColorControl::MessageReceived(message);					
		}
		case WAS_INVOKED:
		{
			Invoked();
			break;	
		}
		default:
			BColorControl::MessageReceived(message);	
	}		
}

/*
 * function    : Invoked
 * purpose     : the widget is invoked
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::Invoked()
{
	SQI_List *color = Squirrel->LocalHeap->List();
	rgb_color c = ValueAsColor();	
	
	GetListFromColor(Squirrel,color,&c);	
		 	
	Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),color,NO_SYSTEM_HOOK);	
	
	if(Hooks[HOOK_COLORCONTROL_INVOKED].action)
	{
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
			
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		args->push_back(color);
		color->AddRef();
	
		AddExtra(args,Hooks[HOOK_COLORCONTROL_INVOKED].extra);	
	
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_INVOKED].action,args);
					
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
void SQI_ColorControl::MouseDown(BPoint point)
{
	if(Hooks[HOOK_COLORCONTROL_MOUSEDOWN].action)
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
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_MOUSEDOWN].action,args);
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BColorControl::MouseDown(point);	
		else
		{
			if(ret->IsFalse())
				BColorControl::MouseDown(point);		
		}	
				
		delete args;
	}
	else
		BColorControl::MouseDown(point);		
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
void SQI_ColorControl::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_COLORCONTROL_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_COLORCONTROL_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_COLORCONTROL_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_COLORCONTROL_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_EXITED].action,args);
						
			delete args;
		}
	}	
	
	BColorControl::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::Pulse(void)
{
	if(Hooks[HOOK_COLORCONTROL_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_PULSE].action,args);
				
		delete args;
	}
	
	BColorControl::Pulse();
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
void SQI_ColorControl::WindowActivated(bool active)
{
	if(Hooks[HOOK_COLORCONTROL_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_COLORCONTROL_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_COLORCONTROL_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BColorControl::WindowActivated(active);
} 

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it's preferred size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::ResizeToPreferred()
{
	BColorControl::ResizeToPreferred();		
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
void SQI_ColorControl::GetPreferredSize(float *width,float *height)
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
			BColorControl::GetPreferredSize(width,height);
	}
	else
		BColorControl::GetPreferredSize(width,height);
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_ColorControl *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::Clone(SQI_ColorControl *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a ColorControl
	
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
