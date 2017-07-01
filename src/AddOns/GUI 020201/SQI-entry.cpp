/*
 * Squirrel project
 *
 * file ?	SQI-entry.cpp
 * what	?   entry object
 * who	?	jlv
 * when	?	12/10/99
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bentry.h"
#include "SQI-glue.h"
#include "SQI-entry.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_Entry[HOOKS_ENTRY] = {
	string("activated"),
	string("attached"),
	string("changed"),
	string("detached"),
	string("draw"),
	string("drop"),
	string("entered"),
	string("exited"),
	string("focused"),
	string("keydown"),
	string("keyup"),
	string("message"),
	string("mousedown"),
	string("mouseup"),
	string("pulse")
};	
     
/*
 * function    : SQI_Entry
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
SQI_Entry::SQI_Entry(SQI_Interp *interp,SQI_BEntry *ptr,const char *label,const char *txt,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags)
	:BTextControl(frame,NULL,label,txt,message,resizingMode,flags | B_FRAME_EVENTS) , 
	SQI_Widget(ptr,HOOKS_ENTRY)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SHooks = Hooks_Entry;
	
	BTextView *tview = TextView();
	
	tview->SetResizingMode(B_FOLLOW_RIGHT);		 
}


/*
 * function    : ~SQI_Entry
 * purpose     : Entry destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Entry::~SQI_Entry()
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
void SQI_Entry::AttachedToWindow(void)
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
						
	if(Hooks[HOOK_ENTRY_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();	
			
		AddExtra(args,Hooks[HOOK_ENTRY_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}		
			
	BTextControl::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Entry::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing	
		
	BTextControl::AllAttached();
	
	BTextView *text = TextView();
	
	text->SetHighColor(HighColor());
	text->SetLowColor(LowColor());
	BFont font;
	GetFont(&font);
	text->SetFontAndColor(&font);
	
	GetPreferredWidth(&pref_width);	
	
	ResizeToPreferred();
	
	SetDivider(pref_divider);	
}

/*
 * function    : AllDetached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Entry::AllDetached(void)
{					
	BTextControl::AllDetached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Entry::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_ENTRY_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_ENTRY_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}
	
	MagicPtr->Window = NULL;
	Squirrel = NULL;		
			
	BTextControl::DetachedFromWindow();	
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
void SQI_Entry::Draw(BRect updateRect)
{
	if(Hooks[HOOK_ENTRY_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_ENTRY_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_DRAW].action,args);		
				
		delete args;
	}		
			
	BTextControl::Draw(updateRect);			
}

/*
 * function    : FrameResized
 * purpose     : Called when the view size has changed
 * input       :
 *
 * float width, new width
 * float height, new height
 *
 * output      : none
 * side effect : none
 */
void SQI_Entry::FrameResized(float width,float height)
{
	SetDivider(pref_divider);				
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
void SQI_Entry::KeyDown(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_ENTRY_KEYDOWN].action)
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
	
		AddExtra(args,Hooks[HOOK_ENTRY_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_KEYDOWN].action,args);
						
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BTextControl::KeyDown(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BTextControl::KeyDown(bytes,numBytes);		
		}
	}
	else
		BTextControl::KeyDown(bytes,numBytes);
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
void SQI_Entry::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_ENTRY_KEYUP].action)
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
	
		AddExtra(args,Hooks[HOOK_ENTRY_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_KEYUP].action,args);
					
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BTextControl::KeyUp(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BTextControl::KeyUp(bytes,numBytes);		
		}
	}
	else
		BTextControl::KeyUp(bytes,numBytes);
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
void SQI_Entry::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_ENTRY_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_ENTRY_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BTextControl::MakeFocus(focused);		
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
void SQI_Entry::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case B_SIMPLE_DATA:
		case B_MIME_DATA:
		case B_ARCHIVED_OBJECT:
		{
			if(Hooks[HOOK_ENTRY_DROP].action)
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
				
				AddExtra(args,Hooks[HOOK_ENTRY_DROP].extra);	
		
				SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_DROP].action,args);
						
				delete args;
				}
			else
				BTextControl::MessageReceived(message);					
		}
		case WAS_CHANGED:
		{
			Invoked();
			break;	
		}
		default:
			BTextControl::MessageReceived(message);	
	}		
}

void SQI_Entry::Invoked()
{
	SQI_Object *s,*old;		
	
	if(!dynamic_cast<SQI_BEntry *>(MagicPtr)->isNUM)
		s = Squirrel->LocalHeap->String(Text());
	else
	{
		// we need to parse it as a number
		int i;
		const char *text = Text();
		
		for(i=0;i<strlen(text);i++)	
			if(text[i] == '.')
				break;	
	
		if(i==strlen(text))
			s = Squirrel->LocalHeap->Number(atol(text));	
		else
			s = Squirrel->LocalHeap->Number(atof(text));		
	}	
			 	
	s->AddRef();	 	
		 	
	old = Interpreter->GlobalVar->Get(Squirrel,MagicPtr->Variable->Data(),NO_SYSTEM_HOOK);
	if(old)
		old->AddRef();
	else
	{
		if(!dynamic_cast<SQI_BEntry *>(MagicPtr)->isNUM)
			old = Squirrel->LocalHeap->String(""); 
		else
			old = Squirrel->LocalHeap->Number(0);	
	}	
		 	
	Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),s,NO_SYSTEM_HOOK);	

	if(Hooks[HOOK_ENTRY_CHANGED].action)
	{
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
			
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		args->push_back(old);
		args->push_back(s);
	
		AddExtra(args,Hooks[HOOK_ENTRY_CHANGED].extra);	
	
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_CHANGED].action,args);
					
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
void SQI_Entry::MouseDown(BPoint point)
{
	if(Hooks[HOOK_ENTRY_MOUSEDOWN].action)
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
			
		AddExtra(args,Hooks[HOOK_ENTRY_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_MOUSEDOWN].action,args);
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BTextControl::MouseDown(point);	
		else
		{
			if(ret->IsFalse())
				BTextControl::MouseDown(point);		
		}	
				
		delete args;
	}
	else
		BTextControl::MouseDown(point);		
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
void SQI_Entry::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_ENTRY_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_ENTRY_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_ENTRY_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_ENTRY_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_EXITED].action,args);
						
			delete args;
		}
	}	
	
	BTextControl::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Entry::Pulse(void)
{
	if(Hooks[HOOK_ENTRY_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_ENTRY_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_PULSE].action,args);
				
		delete args;
	}
	
	BTextControl::Pulse();
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
void SQI_Entry::WindowActivated(bool active)
{
	if(Hooks[HOOK_ENTRY_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_ENTRY_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_ENTRY_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BTextControl::WindowActivated(active);
} 

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it's preferred size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Entry::ResizeToPreferred()
{
	BTextControl::ResizeToPreferred();		
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
void SQI_Entry::GetPreferredSize(float *width,float *height)
{		
	BTextControl::GetPreferredSize(width,height);
	
	*width = pref_width;	
}

/*
 * function    : GetPreferredWidth
 * purpose     : Get the Preferred width of the view
 * input       : 
 *
 * float *width, preferred width
 *
 * output      : none
 * side effect : none
 */
void SQI_Entry::GetPreferredWidth(float *width)
{		
	if(label_width>0)
	{	
		string s(label_width,'X');	
		*width =  5 + StringWidth(s.c_str()) + 5;
	}
	else
		*width = 10 + StringWidth(Label());
			
	pref_divider = *width;
	
	if(entry_width>0)
	{
		string s(entry_width,'X');	
		*width += StringWidth(s.c_str()) + 5;			
	}
	else
	{
		string s(10,'X');	
		*width += StringWidth(s.c_str()) + 5;			
	}				
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_Entry *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Entry::Clone(SQI_Entry *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
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
