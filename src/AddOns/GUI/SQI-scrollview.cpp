/*
 * Squirrel project
 *
 * file ?	SQI-scrollview.cpp
 * what	?   scrollview object
 * who	?	jlv
 * when	?	02/12/01
 * last	?	02/13/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <ScrollView.h> 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-scrollview.h"
#include "Colors.h"
     
/*
 * function    : ScrollView
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
ScrollView::ScrollView(SQI_BWidget *ptr,BRect frame,BView *t,bool present)
	:BView(frame,NULL,B_FOLLOW_NONE,B_WILL_DRAW | B_FRAME_EVENTS) , SQI_Widget(ptr)
{	
	scroll = present;
	
	o_w = frame.Width();
	o_h = frame.Height();
	
	SetViewColor(BeButtonGrey);
	
	target = t;
	client = new BView(frame,NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	client->AddChild(target);
	client->SetViewColor(BeButtonGrey);
	bottom = NULL;
	right = NULL;
			
	AddChild(client);
	
	if(scroll)
		Scroll(scroll);				
}

/*
 * function    : ~ScrollView
 * purpose     : View destructor
 * input       : none
 * output      : none
 * side effect : none
 */
ScrollView::~ScrollView()
{
	target->RemoveSelf();
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void ScrollView::AttachedToWindow(void)
{	
/*		
	if(Parent())
	{
		// now we set the bgcolor and the font to the same that the
		// parent if not allready set by the user
		SetHighColor(Parent()->HighColor());
		SetLowColor(Parent()->LowColor());
		SetViewColor(Parent()->ViewColor());
		BFont font;
		Parent()->GetFont(&font);
		SetFont(&font);		
	} 

*/
					
	BView::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void ScrollView::AllAttached(void)
{
	ResizeToPreferred();		
	BView::AllAttached();
	BWidget->vframe = Frame();	
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
void ScrollView::GetPreferredSize(float *width,float *height)
{	
	*width 	= o_w;
	*height	= o_h;
}

void ScrollView::FrameResized(float width,float height)
{		
	if(!scroll)
	{
		client->ResizeTo(width,height);
		target->ResizeTo(width,height);	
	}
	else
	{
		BRect trg = target->Bounds(); 	
		BRect rect = client->Bounds();
		
		right->SetRange(0,target->Bounds().Height() - client->Bounds().Height());
		right->SetProportion(rect.Width()/trg.Width());
		bottom->SetRange(0,target->Bounds().Width() - client->Bounds().Width());
		bottom->SetProportion(rect.Height()/trg.Height());	
	}			
}

void ScrollView::Scroll(bool s)
{	
	if(s)
	{
		BRect rect = Bounds();
		// resize client to left space to the scrollbar
		client->ResizeBy(-B_V_SCROLL_BAR_WIDTH-1,-B_H_SCROLL_BAR_HEIGHT-1);
		// target don't have to be resized
		
		// set or create the right scrollbar
		if(!right)
		{
			rect.left = rect.right - B_V_SCROLL_BAR_WIDTH; 
			rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
			right = new BScrollBar(rect,NULL,target,0,target->Bounds().Height() - client->Bounds().Height(),B_VERTICAL);
			AddChild(right);	
		}
		else
		{
			rect.left = rect.right - B_V_SCROLL_BAR_WIDTH; 
			rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
			right->MoveTo(rect.left,0);
			right->ResizeTo(rect.Width(),rect.Height());
			right->Show();	
		}
		
		rect = Bounds();	
		
		// set or create the bottom scrollbar
		if(!bottom)
		{
			rect.top 	= rect.bottom - B_H_SCROLL_BAR_HEIGHT;
			rect.right -= B_V_SCROLL_BAR_WIDTH; 
			bottom = new BScrollBar(rect,NULL,target,0,target->Bounds().Width() - client->Bounds().Width(),B_HORIZONTAL);
			AddChild(bottom);	
		}
		else
		{
			rect.top = rect.bottom - B_H_SCROLL_BAR_HEIGHT;
			rect.right -= B_V_SCROLL_BAR_WIDTH; 
			bottom->MoveTo(0,rect.top);
			bottom->ResizeTo(rect.Width(),rect.Height());
			bottom->Show();				
		}
		
		// reset the range and so one for the  scrollbar
	
		BRect trg = target->Bounds(); 	
		rect = client->Bounds();
		
		//trg.PrintToStream();
		//rect.PrintToStream();
			
		right->SetRange(0,target->Bounds().Height() - client->Bounds().Height());
		right->SetProportion(rect.Width()/trg.Width());
		bottom->SetRange(0,target->Bounds().Width() - client->Bounds().Width());
		bottom->SetProportion(rect.Height()/trg.Height());
				
		// force redraw of the target view (mandatory)
		target->Invalidate(rect);				
	}
	else
	{
		if(scroll!=s)
		{
			// hide the scroll bar and resize the client / target
			right->Hide();
			bottom->Hide();
			client->ResizeBy(B_V_SCROLL_BAR_WIDTH,B_H_SCROLL_BAR_HEIGHT);
			target->ResizeTo(client->Bounds().Width(),client->Bounds().Height());
		}		
	}
	
	scroll = s;		
}

void ScrollView::AdjustTo()
{
	client->ResizeTo(target->Bounds().Width(),target->Bounds().Height());		
}

void ScrollView::Refresh()
{
	Invalidate();	
}
