/*
 * Squirrel project
 *
 * file ?	SQI-barberpole.cpp
 * what	?   barberpole widget
 * who	?	jlv
 * when	?	10/14/00
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 2000
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbarberpole.h"
#include "SQI-glue.h"
#include "SQI-barberpole.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_BarberPole[HOOKS_BARBERPOLE] = {
	string("activated"),
	string("attached"),
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
 * function    : SQI_BarberPole
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
SQI_BarberPole::SQI_BarberPole(SQI_Interp *interp,SQI_BBarberPole *ptr,int width,int height,int pDirection,uint32 flags)
	:BView(BRect(0,0,width,height), NULL, B_FOLLOW_NONE, flags | B_WILL_DRAW) ,
	SQI_Widget(ptr,HOOKS_BARBERPOLE)
{	
	BRect bounds = Bounds();
	Interpreter = interp;
	MagicPtr = ptr;
	SHooks = Hooks_BarberPole;
	
 	original_width = width;
	original_height = height;
	
	RealBounds = Bounds();	
	
	is_running = false;
	direction = pDirection;
	pattern		lStripes;
	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;
	FillRect(Bounds(), lStripes);
}


/*
 * function    : ~SQI_BarberPole
 * purpose     : BarberPole destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BarberPole::~SQI_BarberPole()
{
	MagicPtr->View = NULL;
} 

void SQI_BarberPole::Start()
{
	resume_thread(spinning_thread_id); 
	is_running = true;
}

void SQI_BarberPole::Stop()
{
	suspend_thread(spinning_thread_id); 
	is_running = false;
}

bool SQI_BarberPole::IsRunning()
{
	return is_running;
}

int32 SQI_BarberPole::spinningThread(void *data)
{
	SQI_BarberPole	*lBarberPole = (SQI_BarberPole*)data;
	pattern		lStripes;

	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;	
	
	while(1==1){
		lBarberPole->Window()->Lock();
		//lBarberPole->LockLooper();	
		lBarberPole->FillRect(lBarberPole->Bounds(), lStripes);
		//lBarberPole->UnlockLooper();
		lBarberPole->Window()->Unlock();
			
		if(lBarberPole->direction == FROM_RIGHT_TO_LEFT){
			uchar tmp = lStripes.data[0];
			for (int j = 0; j < 7; ++j) {
	  			lStripes.data[j] = lStripes.data[j+1];
			}
			lStripes.data[7] = tmp;
		} else {
			uchar tmp = lStripes.data[7];
			for (int j = 7; j > 0; --j) {
	  			lStripes.data[j] = lStripes.data[j-1];
			}
			lStripes.data[0] = tmp;
		
		}
		snooze(50000);
	}
}

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	
	BView::AttachedToWindow();
			
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
		if(!font_set)
			SetFont(&(bw->Font));				
	} 
	
	//ResizeToPreferred();
					
	if(Hooks[HOOK_BARBERPOLE_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}	
	
	spinning_thread_id = spawn_thread(spinningThread,NULL,B_LOW_PRIORITY,this);		
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_BARBERPOLE_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}
	
	MagicPtr->Window = NULL;
	Squirrel = NULL;
	kill_thread(spinning_thread_id);
	spinning_thread_id = 0;		
			
	BView::DetachedFromWindow();	
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
void SQI_BarberPole::Draw(BRect updateRect)
{	
	Window()->Lock();
	pattern		lStripes;
	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;
	FillRect(Bounds(), lStripes);
	Window()->Unlock();
	
	if(Hooks[HOOK_BARBERPOLE_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_BARBERPOLE_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_DRAW].action,args);		
				
		delete args;
	}			
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
void SQI_BarberPole::KeyDown(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_BARBERPOLE_KEYDOWN].action)
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
	
		AddExtra(args,Hooks[HOOK_BARBERPOLE_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_KEYDOWN].action,args);
						
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BView::KeyDown(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BView::KeyDown(bytes,numBytes);		
		}
	}
	else
		BView::KeyDown(bytes,numBytes);
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
void SQI_BarberPole::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_BARBERPOLE_KEYUP].action)
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
	
		AddExtra(args,Hooks[HOOK_BARBERPOLE_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_KEYUP].action,args);
					
		delete args;
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BView::KeyUp(bytes,numBytes);	
		else
		{
			if(ret->IsFalse())
				BView::KeyUp(bytes,numBytes);		
		}
	}
	else
		BView::KeyUp(bytes,numBytes);
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
void SQI_BarberPole::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_BARBERPOLE_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BView::MakeFocus(focused);		
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
void SQI_BarberPole::MouseDown(BPoint point)
{
	if(Hooks[HOOK_BARBERPOLE_MOUSEDOWN].action)
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
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_MOUSEDOWN].action,args);
		
		SQI_Number *ret = IsNumber(out);
		if(!ret)
			BView::MouseDown(point);	
		else
		{
			if(ret->IsFalse())
				BView::MouseDown(point);		
		}	
				
		delete args;
	}
	else
		BView::MouseDown(point);		
}

void SQI_BarberPole::MessageReceived(BMessage *message)
{		
	switch(message->what)
	{
		case B_MOUSE_UP:
		{
			BPoint point;
			uint32 b;
			
			GetMouse(&point,&b);	
			MouseDown(point);
			
			break;
		}
		case B_SIMPLE_DATA:
		case B_MIME_DATA:
		case B_ARCHIVED_OBJECT:
		{
			if(Hooks[HOOK_BARBERPOLE_DROP].action)
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
				
				AddExtra(args,Hooks[HOOK_BARBERPOLE_DROP].extra);	
		
				SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_DROP].action,args);
						
				delete args;
				}
			else
				BView::MessageReceived(message);					
		}		
		default:
			BView::MessageReceived(message);	
	}	
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
void SQI_BarberPole::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_BARBERPOLE_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_BARBERPOLE_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_BARBERPOLE_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_BARBERPOLE_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_EXITED].action,args);
						
			delete args;
		}
	}	
	
	BView::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::Pulse(void)
{	
	if(Hooks[HOOK_BARBERPOLE_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_PULSE].action,args);
				
		delete args;
	}
		
	BView::Pulse();
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
void SQI_BarberPole::WindowActivated(bool active)
{
	if(Hooks[HOOK_BARBERPOLE_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_BARBERPOLE_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_BARBERPOLE_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BView::WindowActivated(active);
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
void SQI_BarberPole::GetPreferredSize(float *width,float *height)
{
	*width = original_width;
	*height = original_height;
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_BarberPole *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::Clone(SQI_BarberPole *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a BarberPole
	
	// the font used
	
	BFont font;
	GetFont(&font);					
	clone->SetFont(&font);
		
	// color
	
	clone->SetHighColor(HighColor());
	clone->SetLowColor(LowColor());
	clone->SetViewColor(ViewColor());
	
	// that's all	
}


