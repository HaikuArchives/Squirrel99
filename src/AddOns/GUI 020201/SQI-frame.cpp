/*
 * Squirrel project
 *
 * file ?	SQI-frame.cpp
 * what	?   frame object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <InterfaceDefs.h> 
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bframe.h"
#include "SQI-glue.h"
#include "SQI-frame.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_Frame[HOOKS_FRAME] = {
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
	string("moved"),
	string("pulse"),
	string("resized")
};

#define STEP	45
     
/*
 * function    : SQI_Frame
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
SQI_Frame::SQI_Frame(SQI_Interp *interp,SQI_BFrame *ptr,char r,BRect frame,uint32 resizingMode,uint32 flags)
	:BView(frame,NULL,resizingMode,flags | B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP) , 
	SQI_Widget(ptr,HOOKS_FRAME)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SetViewColor(BeButtonGrey); // default color
	SHooks = Hooks_Frame;
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();
		
	if(r==RELIEF_FLATENED)
	{
		level = 1;
		border = 0;
		relief = r;		
	}
	else
	{	
		level = 1;
		border = level + 2;	 
		relief = r;	
	}
	
	ColoringBasis(BeButtonGrey);
}


/*
 * function    : ~SQI_Frame
 * purpose     : Frame destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Frame::~SQI_Frame()
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
void SQI_Frame::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
		
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
					
	if(Hooks[HOOK_FRAME_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_FRAME_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}		
			
	BView::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing
		
	DynamicGluing();
			
	BView::AllAttached();	
}

/*
 * function    : AllDetached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::AllDetached(void)
{					
	BView::AllDetached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_FRAME_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_FRAME_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_DETACHEDFROMWINDOW].action,args);
				
		delete args;
	}		
	
	MagicPtr->Window = NULL;
	Squirrel = NULL;
			
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
void SQI_Frame::Draw(BRect updateRect)
{
	if(Hooks[HOOK_FRAME_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_FRAME_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_DRAW].action,args);		
				
		delete args;
	}	
	
	DrawRelief();					
}

/*
 * function    : FrameMoved
 * purpose     : Called when the view position have changed
 * input       :
 *
 * BPoint parentPoint
 *
 * output      : none
 * side effect : none
 */
void SQI_Frame::FrameMoved(BPoint parentPoint)
{
	if(Hooks[HOOK_FRAME_FRAMEMOVED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *x = MagicPtr->Window->Squirrel->LocalHeap->Number(parentPoint.x);
		SQI_Number *y = MagicPtr->Window->Squirrel->LocalHeap->Number(parentPoint.y);
		x->AddRef();
		y->AddRef();	
		
		args->push_back(x);
		args->push_back(y);
	
		AddExtra(args,Hooks[HOOK_FRAME_FRAMEMOVED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_FRAMEMOVED].action,args);
								
		delete args;
	}		
			
	BView::FrameMoved(parentPoint);	
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
void SQI_Frame::FrameResized(float width,float height)
{
	if(!resized_count)
	{
		if(Hooks[HOOK_FRAME_FRAMERESIZED].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
			
			list<SQI_Object *> *args = new 	list<SQI_Object *>;
					
			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			SQI_Number *w = MagicPtr->Window->Squirrel->LocalHeap->Number(width);
			SQI_Number *h = MagicPtr->Window->Squirrel->LocalHeap->Number(height);
			w->AddRef();
			h->AddRef();	
			
			args->push_back(w);
			args->push_back(h);
		
			AddExtra(args,Hooks[HOOK_FRAME_FRAMERESIZED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_FRAME_FRAMERESIZED].action,args);
									
			delete args;
		}
		
		BView::FrameResized(width,height);	
		if(Bounds() != RealBounds)	
			StaticGluing();
	}
	else
	{
		resized_count--;
		if(resized_count<0)
			resized_count = 0;
		BView::FrameResized(width,height);	
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
void SQI_Frame::KeyDown(const char *bytes,int32 numBytes)
{	
	if(Hooks[HOOK_FRAME_KEYDOWN].action)
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
	
		AddExtra(args,Hooks[HOOK_FRAME_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_KEYDOWN].action,args);
						
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
void SQI_Frame::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_FRAME_KEYUP].action)
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
	
		AddExtra(args,Hooks[HOOK_FRAME_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_KEYUP].action,args);
					
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
void SQI_Frame::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_FRAME_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_FRAME_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_MAKEFOCUS].action,args);
						
		delete args;
	}	
	
	BView::MakeFocus(focused);		
}

/*
 * function    : MessageReceived
 * purpose     : A message has been received by the widget
 * input       : 
 *
 * BMessage *message, the message received
 *
 * output      : none
 * side effect : none
 */
void SQI_Frame::MessageReceived(BMessage *message)
{			
		switch(message->what)
		{
			case B_SIMPLE_DATA:
			case B_MIME_DATA:
			case B_ARCHIVED_OBJECT:
			{
				if(Hooks[HOOK_FRAME_DROP].action)
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
					
					AddExtra(args,Hooks[HOOK_FRAME_DROP].extra);	
			
					SQI_Object *out = UseHook(Hooks[HOOK_FRAME_DROP].action,args);
							
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
 * function    : MouseDown
 * purpose     : A button of the mouse has been clicked
 * input       : 
 *
 * BPoint point, position of the mouse
 *
 * output      : none
 * side effect : none
 */
void SQI_Frame::MouseDown(BPoint point)
{
	if(Hooks[HOOK_FRAME_MOUSEDOWN].action)
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
			
		AddExtra(args,Hooks[HOOK_FRAME_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_MOUSEDOWN].action,args);
		
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
void SQI_Frame::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_FRAME_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_FRAME_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_FRAME_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_FRAME_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_FRAME_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_FRAME_EXITED].action,args);
						
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
void SQI_Frame::Pulse(void)
{
	if(Hooks[HOOK_FRAME_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_FRAME_PULSE].extra);	
				
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_PULSE].action,args);
						
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
void SQI_Frame::WindowActivated(bool active)
{
	if(Hooks[HOOK_FRAME_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_FRAME_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_FRAME_WINDOWACTIVATED].action,args);
								
		delete args;
	}
	
	BView::WindowActivated(active);
} 

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it's preferred size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::ResizeToPreferred()
{
	BView::ResizeToPreferred();	
	
	/*
	BFrame * child = ChildAt(0);
	
	// will need more stuff when we add view to a view
	
	while(child)
	{
		child->ResizeToPreferred();	
		child = child->NextSibling();	
	}	
	*/
	
	// Using StaticGluing here may be usefull	
	
	//StaticGluing();
	
	
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
void SQI_Frame::GetPreferredSize(float *width,float *height)
{
		
	// for now we return the original size, but when we will
	// have view attached to it, it will be diferrent	
		
	*width = original_width;
	*height = original_height;
}

/*
 * function    : DynamicGluing
 * purpose     : Glue a new set of views and resize the window according
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::DynamicGluing()
{	
	resized_by = RESIZED_BY_GLUING;	
	
	if(!CountChildren())
		return;	
		
	// first we hide all the view attached to the window	
		
	BView *child = ChildAt(0);
	while(child)
	{
		child->Hide();
		child = child->NextSibling();	
	}
		
	// now we seek through all the view to pack and we pack them	
		
	list<SQI_BWidget *>::const_iterator i;	
	list<SQI_BWidget *> *widgets;
	SQI_BWidget *widget;
	BRect f;
	char align;
	float horizontal,vertical,x,y;
	
	list<TGlue *> *Childs = MagicPtr->Contents;
	list<TGlue *>::const_iterator j;
	
	for(j=Childs->begin();j!=Childs->end();j++)
	{
		widgets = (*j)->elements;	
		align = (*j)->alignment;
	
		for(i=widgets->begin();i!=widgets->end();i++)
		{	
			// first thing, we check if we have already glued something	
												
			if(RealBounds == empty)
			{
				// first gluing so it's rather simple
					
				widget = *i;
																	
				// and we place the view to it position	
			
				widget->MoveTo(border,border);
				
				// we get the frame size of the widget with padding
				
				f = widget->TrueFrame();
											
				f.InsetBy(-border,-border);	
								
				RealBounds = f;		
			}
			else
			{	
				// something has been allready glued so it's more complicated
				
				switch(align)
				{
					case ALIGN_TOP:
					{
						// GLUE on top	
										
						widget = *i;
							
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();
									
						// we going to increase the RealBounds of what is need
						
						if((f.Width()+2*border) > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
						else
							horizontal = 0;
								
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
													
						if(vertical>0 || horizontal>0)
						{ 																																		
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																																
							// we move all the view down
							
							MoveByChilds(MagicPtr->Contents,0,vertical);
							
							// now we add the view to the top	
							
							widget->MoveTo(border,border);
						}
						
						break;	
					}
					case ALIGN_BOTTOM:
					{
						// glue on bottom
						
						widget = *i;
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
										
						if((f.Width()+2*border) > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
						else
							horizontal = 0;
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
						
						if(vertical>0 || horizontal>0)
						{ 			
							y = RealBounds.bottom - border + 1;	
													
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
								
							// we move the view to the bottom
																	
							widget->MoveTo(border,y);
						}
					
						break;	
					}
					case ALIGN_LEFT:
					{
						// glue on left
						
						widget = *i;
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
																
						if((f.Height()+2*border) > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + 2*border;
						else
							vertical = 0;
												
						if(f.Width()>0)
							horizontal = f.Width() + 1;	
						else
							horizontal = 0;
														
						if(horizontal>0 || vertical>0)
						{								
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																					
							// we move all the view right
							
							MoveByChilds(MagicPtr->Contents,f.Width()+1,0);
							
							// now we add the view to the top/left	
							
							widget->MoveTo(border,border);
						}	
						
						break;	
					}
					case ALIGN_RIGHT:
					{
						// glue on right
												
						widget = *i;
											
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
										
						if((f.Height()+2*border) > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + 2*border;
						else
							vertical = 0;
						if(vertical<0)
							vertical = 0;
						if(f.Width()>0)
							horizontal = f.Width() + 1;	
						else
							horizontal = 0;
						
						if(horizontal>0 || vertical>0)
						{							
							x = RealBounds.right - border + 1;
											
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																								
							// now we add the view to the top/left	
													
							widget->MoveTo(x,border);
						}	
												
						break;	
					}				
				}				
		
			}			
		}	
		
	}	
		
	// we resize the view
					
	resized_count++;
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();
				
	original_width = RealBounds.Width();
	original_height = RealBounds.Height();
	
	// Time to see what could be expanded
	
	ExpandChilds();
		
	// now we show all the view	
	
	child = ChildAt(0);
	while(child)
	{
		child->Show();
		child = child->NextSibling();	
	}
	
	resized_by = RESIZED_BY_USER;	
}

/*
 * function    : StaticGluing
 * purpose     : The frame of the view have been changed by the user, we re glue everything
 * input       : 
 *
 * float width, new width of the window
 * float height, new height of the window
 * 
 * output      : none
 * side effect : none
 */
void SQI_Frame::StaticGluing()
{
	// if no children, don't need to do anything
	
	if(!CountChildren())
		return;	
		
	// first we hide all the view attached to the view
					
	BView *child = ChildAt(0);
		
	while(child)
	{
		child->Hide();
		child = child->NextSibling();	
	}
	
	// we get the Bounds of the window	
	
	RealBounds = Bounds();	
	
	// we now glue everything
	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *>::const_iterator j;
	list<SQI_BWidget *> *widgets;
	SQI_BWidget *widget;
	BRect f;	
	float x,y;
	
	Childs = MagicPtr->Contents;	
	
	double area = 0;
	
	for(i=Childs->begin();i!=Childs->end();i++)
	{
		widgets = (*i)->elements;
		switch((*i)->alignment)
		{
			case ALIGN_TOP:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->ResizeToPreferred();	
						
					// we get the bounds of the view to pack	
								
					f = widget->TrueBounds();	
																	
					// we move all the view aligner TOP, down
						
					MoveByChilds(MagicPtr->Contents,0.0,f.Height()+1,(char)ALIGN_BOTTOM);
										
					// now we add the view to the top	
					
					widget->MoveTo(border,border);		
				}
				
				break;
			}	
			case ALIGN_BOTTOM:
			{	
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->ResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->TrueBounds();	
							
					// we move all the view whom are aligned bottom, up
					
					MoveByChilds(MagicPtr->Contents,ALIGN_BOTTOM,0,-f.Height()-1);
										
					y = RealBounds.bottom -f.Height() - border;
															
					widget->MoveTo(border,y);
				}		
				
				break;		
			}	
			case ALIGN_RIGHT:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->ResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->TrueBounds();	
																						
					// now we add the view to the top/left	
					
					MoveByChilds(MagicPtr->Contents,(char)ALIGN_RIGHT,-f.Width()-1,0.0);
						
					x = RealBounds.right - f.Width() - border;
										
					widget->MoveTo(x,border);	
				}
				
				break;	
			}	
			case ALIGN_LEFT:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->ResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->TrueBounds();	
																						
					// we move all the view right
					
					MoveByChilds(MagicPtr->Contents,f.Width() + 1,0,ALIGN_RIGHT);
					
					// now we add the view to the top/left	
					
					widget->MoveTo(border,border);					
				}
				
				break;				
			}	
		}
	}
	
	// Time to see what could be expanded
	
	ExpandChilds();
	
	// now we show all the view	
	
	child = ChildAt(0);
	while(child)
	{
		child->Show();
		child = child->NextSibling();	
	}
}

/*
 * function    : ExpandChilds
 * purpose     : Expand all the childs that could be expanded or aligned
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::ExpandChilds()
{	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *> *widgets;
	list<SQI_BWidget *>::const_iterator j;
	SQI_BWidget *widget;
	SQI_Widget *w;
	float left,right,top,bottom,xcenter,ycenter,width,height;
	int m;
	BRect f;
	bool expandx,expandy;
	
	SQI_BWidget *left_close,*right_close,*top_close,*bottom_close;
	
	Childs = MagicPtr->Contents;
	
	if(Childs)
	{
		// we first compute the area convert by all the view in the Window
	
		BRect z = RealBounds;
		
		z.InsetBy(border,border);
	
		double area = z.Width() * z.Height();
		
		// in the first loop, we make the expand the views
			
		for(i=Childs->begin();i!=Childs->end();i++)
		{
		
			widgets = (*i)->elements;
			for(j=widgets->begin();j!=widgets->end();j++)			
			{	
				widget = *j;	
															
				// we check if we have  some free space
									
				if(area > AreaChilds(MagicPtr->Contents))
				{	
					w = widget->GiveWidget();
					f = widget->TrueFrame();
					
					left = 0;
					right = 0;
					top = 0;
					bottom = 0;
					width = 0;
					height = 0;
					expandx = false;
					expandy = false;
					
					// we need to expand in one direction first and in the other after,
					// but in which order ?
					
					switch((*i)->alignment)
					{
						case ALIGN_TOP:
						case ALIGN_BOTTOM:
						{
							// we expand y first
							if(w->expandy)
							{
								expandy = true;	
								top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
								bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);	
												
								if(!top_close)
								{	
									// we need to expand to the maximum possible that mean to the
									// window size
									
									top = f.top - border; 
								}
								
								if(!bottom_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									bottom = RealBounds.bottom - border - f.bottom;
								}
								
								widget->Expand(0,top,0,bottom);					
							}
							
							// an x after
							
							if(w->expandx)
							{
								expandx = true;
								
								left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
								right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																		
								if(!left_close)
								{	
									// we need to expand to the maximum possible that mean to the
									// window size
									
									left = f.left - border; 
								}
								
								if(!right_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									right = RealBounds.right - border - f.right;
								}
								
								widget->Expand(left,0,right,0);	
							}
							
							break;	
						}
						default:
						{
							// we expand x first
							
							if(w->expandx)
							{
								expandx = true;
								
								left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
								right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																					
								if(!left_close)
								{	
									// we need to expand to the maximum possible that mean to the
									// window size
									
									left = f.left - border; 
								}
								
								if(!right_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									right = RealBounds.right - border - f.right;
								}
								
								widget->Expand(left,0,right,0);	
							}
							
							// and y now
							
							if(w->expandy)
							{
								expandy = true;	
								top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
								bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);	
												 
								if(!top_close)
								{	
									// we need to expand to the maximum possible that mean to the
									// window size
									
									top = f.top - border; 
								}
								
								if(!bottom_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									bottom = RealBounds.bottom - border - f.bottom;
								}
															
								widget->Expand(0,top,0,bottom);					
							}	
							
							break;
						}	
					}		
				}	
				else
					return;
			}	
		}
	
		if(area<=AreaChilds(MagicPtr->Contents))
			return;
		
		// now we going to process the alignement
	
		for(i=Childs->begin();i!=Childs->end();i++)
		{
		
			widgets = (*i)->elements;
			for(j=widgets->begin();j!=widgets->end();j++)			
			{	
				widget = *j;
														
				w = widget->GiveWidget();
				f = widget->TrueFrame();
					
				width = 0;
				height = 0;	
								
				/*if(!expandx)
				{					
				*/	switch(w->halign)
					{
						case ALIGN_CENTER:
						{
							left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
							right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																									
							if(!left_close)
								left = f.left - border; 
							
							if(!right_close)
								right = RealBounds.right - border - f.right;	
										
							if(left == right)
								width = 0;	
							else
							{	
								m = (left + f.Width() + right) / 2;
								width = (m - left) - (f.Width() / 2);	
							}	
								
							break;
						}
						case ALIGN_RIGHT:
						{
							left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
							right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
					
							if(!left_close)
								left = f.left - border; 
							
							if(!right_close)
								right = RealBounds.right - border - f.right;	
																			
							if(left == right)
								width = 0;	
							else
								width = right;	
							
							break;	
						}	
					}
					
					if(width>0)
						widget->MoveBy(width,0);						
				//}
				
				//if(!expandy)
				//{
					switch(w->valign)
					{
						case ALIGN_CENTER:
						{
							top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
							bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
												
							if(!top_close)
								top = f.top - border;
							
							if(!bottom_close)
								bottom = (RealBounds.bottom - border) - f.bottom; 
																				
							if(top == bottom)
								height = 0;
							else		
							{							
								m = (top + f.Height() + bottom) / 2;
								height = (m - top) - (f.Height() / 2);	
							}	
								
							break;	
						}	
						case ALIGN_BOTTOM:
						{
							top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
							bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
					
							if(!top_close)
								top = f.top - border; 
							
							if(!bottom_close)
								bottom = (RealBounds.bottom - border) - f.bottom;	
							
							if(top == bottom)	
								height = 0;
							else
								height = bottom;
							break;	
						}
					}
					
					if(height>0)
						widget->MoveBy(0,height);	
				//}
			}
		}
	}
}

/*
 * function    : ReGluing
 * purpose     : redo the gluing of the view
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::ReGluing()
{	
	resized_by = RESIZED_BY_GLUING;	

	if(!MagicPtr->Contents)
		return;	
		
	// first we hide all the view attached to the window	
		
	BView *child = ChildAt(0);
	while(child)
	{
		child->Hide();
		child = child->NextSibling();	
	}
	
	RealBounds = empty;
			
	// now we seek through all the view to pack and we pack them	
		
	list<SQI_BWidget *>::const_iterator i;	
	list<SQI_BWidget *> *widgets;
	SQI_BWidget *widget;
	BRect f;
	char align;
	float horizontal,vertical,x,y;
	
	list<TGlue *> *Childs = MagicPtr->Contents;
	list<TGlue *>::const_iterator j;
	
	for(j=Childs->begin();j!=Childs->end();j++)
	{
		widgets = (*j)->elements;	
		align = (*j)->alignment;
	
		for(i=widgets->begin();i!=widgets->end();i++)
		{	
			// first thing, we check if we have already glued something	
								
			if(RealBounds == empty)
			{
				// first gluing so it's rather simple
					
				widget = *i;
				
				widget->ResizeToPreferred();
															
				// and we place the view to it position	
			
				widget->MoveTo(0,0);
				
				// we get the frame size of the widget with padding
				
				f = widget->TrueFrame();
				
				RealBounds = f;		
			}
			else
			{	
				// something has been allready glued so it's more compicated
				
				switch(align)
				{
					case ALIGN_TOP:
					{
						// GLUE on top	
										
						widget = *i;
						
						widget->ResizeToPreferred();
							
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();
						
						// we going to increase the RealBounds of what is need
								
						if(f.Width() > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
						else
							horizontal = 0;
								
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
													
						if(vertical>0 || horizontal>0)
						{ 																																		
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																																
							// we move all the view down
							
							MoveByChilds(MagicPtr->Contents,0,vertical);
							
							// now we add the view to the top	
							
							widget->MoveTo(border,border);
						}
						
						break;	
					}
					case ALIGN_BOTTOM:
					{
						// glue on bottom
						
						widget = *i;
						
						widget->ResizeToPreferred();
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						if(f.Width() > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
						else
							horizontal = 0;
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
						
						if(vertical>0 || horizontal>0)
						{ 			
							y = RealBounds.bottom - border + 1;	
													
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
								
							// we move the view to the bottom
																	
							widget->MoveTo(border,y);
						}
					
						break;	
					}
					case ALIGN_LEFT:
					{
						// glue on left
						
						widget = *i;
						
						widget->ResizeToPreferred();
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						if(f.Height() > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + 2*border;
						else
							vertical = 0;
						if(f.Width()>0)
							horizontal = f.Width() + 1;	
						else
							horizontal = 0;
							
						if(horizontal>0 || vertical>0)
						{								
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																					
							// we move all the view right
							
							MoveByChilds(MagicPtr->Contents,f.Width()+1,0);
							
							// now we add the view to the top/left	
							
							widget->MoveTo(border,border);
						}		
						
						break;	
					}
					case ALIGN_RIGHT:
					{
						// glue on right
												
						widget = *i;
						
						widget->ResizeToPreferred();
											
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						// we going to increase the RealBounds of what is need
										
						if(f.Height() > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + 2*border;
						else
							vertical = 0;
						if(vertical<0)
							vertical = 0;
						if(f.Width()>0)
							horizontal = f.Width() + 1;	
						else
							horizontal = 0;
						
						if(horizontal>0 || vertical>0)
						{							
							x = RealBounds.right - border + 1;
											
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																	
							// now we add the view to the top/left	
													
							widget->MoveTo(x,border);
						}	
												
						break;	
					}				
				}				
		
			}			
		}	
		
	}	
		
	// we resize the view
					
	resized_count++;
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();
	
	original_width = RealBounds.Width();
	original_height = RealBounds.Height();
	
	// Time to see what could be expanded
	
	ExpandChilds();
		
	// now we show all the view	
	
	child = ChildAt(0);
	while(child)
	{
		child->Show();
		child = child->NextSibling();	
	}
	
	resized_by = RESIZED_BY_USER;	
}

/*
 * function    : Adapt
 * purpose     : Change RealBounds to the actual bounds
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Frame::Adapt()
{
	// all childs must be resized to preferred
	
	double a1,a2;	
		
	a1 = RealBounds.Width() * RealBounds.Height();
	a2 = Bounds().Width() * Bounds().Height();	
	
	RealBounds = Bounds();
		
	if(a2<a1)
	{	
		list<TGlue *> *Childs;
		list<SQI_BWidget *> *widgets;
		list<TGlue *>::const_iterator i;
		list<SQI_BWidget *>::const_iterator j;	
		
		Childs = MagicPtr->Contents;
		
		if(Childs)
		{	
			for(i=Childs->begin();i!=Childs->end();i++)
			{
			
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
					(*j)->ResizeToPreferred();
			}
		}
	}
	else
		StaticGluing();
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned frame to be the same
 * input       : 
 *
 * SQI_View *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Frame::Clone(SQI_Frame *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a View
	
	clone->border = border;
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
	
	clone->hi_color = hi_color;
	clone->lo_color = lo_color;
	
	// relief info
	
	clone->level = level;
	
	// pen size
	
	clone->SetPenSize(PenSize());

	// that's all	
}

// Drawing methods

void SQI_Frame::DrawRelief()
{
	switch(relief)
	{	
		case RELIEF_RAISED:
		{		
			BRect r = Bounds();
		
			rgb_color hi = HighColor();
			
			SetHighColor(ViewColor());
			
			StrokeRect(BRect(r));	
			
			SetHighColor(hi);	
			
			r.InsetBy(1,1);
							
			BeginLineArray(level * 4);
			for (int i=0; i<level; i++)
			{
				AddLine(r.RightTop(), r.RightBottom(), hi_color);
				AddLine(r.RightBottom(), r.LeftBottom(), hi_color);
				AddLine(r.LeftTop(), r.RightTop(), lo_color);
				AddLine(r.LeftBottom(), r.LeftTop(), lo_color);
				r.InsetBy(1,1);		
			}
			EndLineArray();
			break;
		}
		case RELIEF_LOWERED:
		{		
			BRect r = Bounds();
			
			rgb_color hi = HighColor();
			
			SetHighColor(ViewColor());
			
			StrokeRect(BRect(r));	
			
			SetHighColor(hi);	
			
			r.InsetBy(1,1);
								
			BeginLineArray(level * 4);
			for (int i=0; i<level; i++)
			{
				AddLine(r.RightTop(), r.RightBottom(), lo_color);
				AddLine(r.RightBottom(), r.LeftBottom(), lo_color);
				AddLine(r.LeftTop(), r.RightTop(), hi_color);
				AddLine(r.LeftBottom(), r.LeftTop(), hi_color);
				r.InsetBy(1,1);
			}
			EndLineArray();
			break;
		}
		case RELIEF_BORDERED:
		{
			BRect r = Bounds();
						
			rgb_color lo = lo_color;
			rgb_color hi = hi_color;
			
			int step = STEP / level;
		
			BeginLineArray(level * 4);
			for (int i=0; i<level; i++)
			{
				AddLine(r.RightTop(), r.RightBottom(), hi);
				AddLine(r.RightBottom(), r.LeftBottom(), hi);
				AddLine(r.LeftTop(), r.RightTop(), lo);
				AddLine(r.LeftBottom(), r.LeftTop(), lo);
				r.InsetBy(1,1);
				
				hi.red += step;
				hi.green += step;
				hi.blue += step;
				lo.red -= step;
				lo.green -= step;
				lo.blue -= step;			
			}
			EndLineArray();
			break;
		}		
	}	
}
	

#define min(x,y) ((x<y)?x:y)
#define max(x,y) ((x>y)?x:y)

void SQI_Frame::ColoringBasis(rgb_color basis_color)
{
	SetViewColor(basis_color);
	
	hi_color = basis_color;
	hi_color.red = max(hi_color.red-STEP, 0);
	hi_color.green = max(hi_color.green-STEP, 0);
	hi_color.blue = max(hi_color.blue-STEP, 0);

	lo_color = basis_color;
	lo_color.red = min(lo_color.red+STEP, 255);
	lo_color.green = min(lo_color.green+STEP, 255);
	lo_color.blue = min(lo_color.blue+STEP, 255);
}

