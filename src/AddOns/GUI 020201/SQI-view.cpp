/*
 * Squirrel project
 *
 * file ?	SQI-view.cpp
 * what	?   view object
 * who	?	jlv
 * when	?	11/24/99
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
#include "SQI-bview.h"
#include "SQI-glue.h"
#include "SQI-view.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

string Hooks_View[HOOKS_VIEW] = {
	string("activated"),
	string("attached"),
	string("detached"),
	string("draw"),
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
     
/*
 * function    : SQI_View
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
SQI_View::SQI_View(SQI_Interp *interp,SQI_BView *ptr,BRect frame,uint32 resizingMode,uint32 flags)
	:BView(frame,NULL,resizingMode,flags) , 
	SQI_Widget(ptr,HOOKS_VIEW)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SetViewColor(BeButtonGrey); // default color
	SHooks = Hooks_View;
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	 
}


/*
 * function    : ~SQI_View
 * purpose     : View destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_View::~SQI_View()
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
void SQI_View::AttachedToWindow(void)
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
					
	if(Hooks[HOOK_VIEW_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_VIEW_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_ATTACHEDTOWINDOW].action,args);
				
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
void SQI_View::AllAttached(void)
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
void SQI_View::AllDetached(void)
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
void SQI_View::DetachedFromWindow(void)
{				
	if(Hooks[HOOK_VIEW_DETACHEDFROMWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_VIEW_DETACHEDFROMWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_DETACHEDFROMWINDOW].action,args);
				
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
void SQI_View::Draw(BRect updateRect)
{
	if(Hooks[HOOK_VIEW_DRAW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_List *frame = Frame2List(MagicPtr->Window->Squirrel,updateRect);
		
		frame->AddRef();	
		
		args->push_back(frame);
				
		AddExtra(args,Hooks[HOOK_VIEW_DRAW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_DRAW].action,args);		
				
		delete args;
	}		
			
	BView::Draw(updateRect);			
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
void SQI_View::FrameMoved(BPoint parentPoint)
{
	if(Hooks[HOOK_VIEW_FRAMEMOVED].action)
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
	
		AddExtra(args,Hooks[HOOK_VIEW_FRAMEMOVED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_FRAMEMOVED].action,args);
								
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
void SQI_View::FrameResized(float width,float height)
{
	if(!resized_count)
	{
		if(Hooks[HOOK_VIEW_FRAMERESIZED].action)
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
		
			AddExtra(args,Hooks[HOOK_VIEW_FRAMERESIZED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_VIEW_FRAMERESIZED].action,args);
									
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
void SQI_View::KeyDown(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_VIEW_KEYDOWN].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		uint32 c = uint32(*bytes);
		
		SQI_String *key = MagicPtr->Window->Squirrel->LocalHeap->String(bytes);
		SQI_Number *code = MagicPtr->Window->Squirrel->LocalHeap->Number((long)c);
		key->AddRef();
		code->AddRef();	
		
		args->push_back(key);
		args->push_back(code);
	
		AddExtra(args,Hooks[HOOK_VIEW_KEYDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_KEYDOWN].action,args);
						
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
void SQI_View::KeyUp(const char *bytes,int32 numBytes)
{
	if(Hooks[HOOK_VIEW_KEYUP].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		uint32 c = uint32(*bytes);
		
		SQI_String *key = MagicPtr->Window->Squirrel->LocalHeap->String(bytes);
		SQI_Number *code = MagicPtr->Window->Squirrel->LocalHeap->Number((long)c);
		key->AddRef();
		code->AddRef();	
		
		args->push_back(key);
		args->push_back(code);
	
		AddExtra(args,Hooks[HOOK_VIEW_KEYUP].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_KEYUP].action,args);
					
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
void SQI_View::MakeFocus(bool focused = true)
{
	if(Hooks[HOOK_VIEW_MAKEFOCUS].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *focus = MagicPtr->Window->Squirrel->LocalHeap->Number(focused);
		focus->AddRef();
		
		args->push_back(focus);
			
		AddExtra(args,Hooks[HOOK_VIEW_MAKEFOCUS].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_MAKEFOCUS].action,args);
						
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
void SQI_View::MouseDown(BPoint point)
{
	if(Hooks[HOOK_VIEW_MOUSEDOWN].action)
	{
		// we need to know which button was used
		
		uint32 button = Window()->CurrentMessage()->FindInt32("buttons");	
		
		// we need to create a list of value for the Hook which contain the input of the function	
				
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *x = MagicPtr->Window->Squirrel->LocalHeap->Number(point.x);
		SQI_Number *y = MagicPtr->Window->Squirrel->LocalHeap->Number(point.y);
		SQI_Number *b = MagicPtr->Window->Squirrel->LocalHeap->Number((int)button);
		x->AddRef();
		y->AddRef();
		b->AddRef();
		
		args->push_back(x);
		args->push_back(y);
		args->push_back(b);
			
		AddExtra(args,Hooks[HOOK_VIEW_MOUSEDOWN].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_MOUSEDOWN].action,args);
		
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
void SQI_View::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	if(transit==B_ENTERED_VIEW)
	{	
		if(Hooks[HOOK_VIEW_ENTERED].action)
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
				
			AddExtra(args,Hooks[HOOK_VIEW_ENTERED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_VIEW_ENTERED].action,args);
						
			delete args;
		}
	}
	else
	if(transit==B_EXITED_VIEW)
	{	
		if(Hooks[HOOK_VIEW_EXITED].action)
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
				
			AddExtra(args,Hooks[HOOK_VIEW_EXITED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_VIEW_EXITED].action,args);
						
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
void SQI_View::Pulse(void)
{
	if(Hooks[HOOK_VIEW_PULSE].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_VIEW_PULSE].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_PULSE].action,args);
				
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
void SQI_View::WindowActivated(bool active)
{
	if(Hooks[HOOK_VIEW_WINDOWACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Number *a = MagicPtr->Window->Squirrel->LocalHeap->Number(active);
		a->AddRef();
		
		args->push_back(a);
			
		AddExtra(args,Hooks[HOOK_VIEW_WINDOWACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_VIEW_WINDOWACTIVATED].action,args);
								
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
void SQI_View::ResizeToPreferred()
{
	BView::ResizeToPreferred();	
	
	/*
	BView * child = ChildAt(0);
	
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
void SQI_View::GetPreferredSize(float *width,float *height)
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
void SQI_View::DynamicGluing()
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
							
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height();
																						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
											
						// we move all the view down
						
						MoveByChilds(MagicPtr->Contents,0,f.Height());
						
						// now we add the view to the top	
						
						widget->MoveTo(0,0);
						
						break;	
					}
					case ALIGN_BOTTOM:
					{
						// glue on bottom
						
						widget = *i;
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height();				
					
						y = RealBounds.bottom;	
												
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
							
						// we move the view to the bottom
																
						widget->MoveTo(0,y);
					
						break;	
					}
					case ALIGN_LEFT:
					{
						// glue on left
						
						widget = *i;
						
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width();					
						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																				
						// we move all the view right
						
						MoveByChilds(MagicPtr->Contents,f.Width(),0);
						
						// now we add the view to the top/left	
						
						widget->MoveTo(0,0);	
						
						break;	
					}
					case ALIGN_RIGHT:
					{
						// glue on right
												
						widget = *i;
											
						// we get the bounds of the view to pack	
									
						f = widget->TrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width();
						
						x = RealBounds.right;
										
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																
						// now we add the view to the top/left	
												
						widget->MoveTo(x,0);	
												
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
void SQI_View::StaticGluing()
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
						
					MoveByChilds(MagicPtr->Contents,0.0,f.Height(),(char)ALIGN_BOTTOM);
										
					// now we add the view to the top	
					
					widget->MoveTo(0,0);		
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
					
					MoveByChilds(MagicPtr->Contents,ALIGN_BOTTOM,0,-f.Height());
										
					y = RealBounds.bottom -f.Height();
															
					widget->MoveTo(0,y);
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
					
					MoveByChilds(MagicPtr->Contents,(char)ALIGN_RIGHT,-f.Width(),0.0);
						
					x = RealBounds.right - f.Width();
										
					widget->MoveTo(x,0);	
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
					
					MoveByChilds(MagicPtr->Contents,f.Width(),0,ALIGN_RIGHT);
					
					// now we add the view to the top/left	
					
					widget->MoveTo(0,0);					
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
void SQI_View::ExpandChilds()
{	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *> *widgets;
	list<SQI_BWidget *>::const_iterator j;
	SQI_BWidget *widget;
	SQI_Widget *w;
	float left,right,top,bottom,xcenter,ycenter,width,height;
	float l,r,c,m;
	BRect f;
	bool expandx,expandy;
	
	SQI_BWidget *left_close,*right_close,*top_close,*bottom_close;
	
	Childs = MagicPtr->Contents;
	
	// we first compute the area convert by all the view in the Window

	double area = RealBounds.Width() * RealBounds.Height();
	
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
								
								top = f.top; 
							}
							
							if(!bottom_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								bottom = RealBounds.bottom - f.bottom;
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
								
								left = f.left; 
							}
							
							if(!right_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								right = RealBounds.right - f.right;
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
								
								left = f.left; 
							}
							
							if(!right_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								right = RealBounds.right - f.right;
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
								
								top = f.top; 
							}
							
							if(!bottom_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								bottom = RealBounds.bottom - f.bottom;
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
				
			
			if(!expandx)
			{					
				switch(w->halign)
				{
					case ALIGN_CENTER:
					{
						left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
						right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																
						if(!left_close)
							left = f.left; 
						
						if(!right_close)
							right = RealBounds.right - f.right;	
									
						if(left == right)
							width = 0;	
						else
						{	
							m = (f.left + f.right) / 2.0;
							l = f.left - left;
							r = f.right + right;	
							c = (l+r) / 2.0;
							
							width = c - m;	
						}	
							
						break;
					}
					case ALIGN_RIGHT:
					{
						left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
						right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
				
						if(!left_close)
							left = f.left; 
						
						if(!right_close)
							right = RealBounds.right - f.right;	
																		
						if(left == right)
							width = 0;	
						else
							width = right;	
						
						break;	
					}	
				}
				
				if(width>0)
					widget->MoveBy(width,0);						
			}
			
			if(!expandy)
			{
				switch(w->valign)
				{
					case ALIGN_CENTER:
					{
						top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
						bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
								
						if(!top_close)
							top = f.top; 
						
						if(!bottom_close)
							bottom = RealBounds.bottom - f.bottom;
														
						if(top == bottom)
							height = 0;
						else		
						{							
							m = (f.top + f.bottom) / 2.0;
							l = f.top - top;
							r = f.bottom + bottom;	
							c = (l+r) / 2.0;
							
							height = c - m;	
						}	
							
						break;	
					}	
					case ALIGN_BOTTOM:
					{
						top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
						bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
				
						if(!top_close)
							top = f.top; 
						
						if(!bottom_close)
							bottom = RealBounds.bottom - f.bottom;	
						
						if(top == bottom)	
							height = 0;
						else
							height = bottom;
						break;	
					}
				}
				
				if(height>0)
					widget->MoveBy(0,height);	
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
void SQI_View::ReGluing()
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
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height();
																						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
											
						// we move all the view down
						
						MoveByChilds(MagicPtr->Contents,0,f.Height());
						
						// now we add the view to the top	
						
						widget->MoveTo(0,0);
						
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
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height();				
					
						y = RealBounds.bottom;	
												
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
							
						// we move the view to the bottom
																
						widget->MoveTo(0,y);
					
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
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width();					
						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																				
						// we move all the view right
						
						MoveByChilds(MagicPtr->Contents,f.Width(),0);
						
						// now we add the view to the top/left	
						
						widget->MoveTo(0,0);	
						
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
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width();
						
						x = RealBounds.right;
										
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																
						// now we add the view to the top/left	
												
						widget->MoveTo(x,0);	
												
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

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_View *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_View::Clone(SQI_View *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a View
	
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
	
	// that's all	
}


