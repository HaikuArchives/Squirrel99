/*
 * Squirrel project
 *
 * file ?	SQI-container.cpp
 * what	?   container object
 * who	?	jlv
 * when	?	12/05/99
 * last	?	03/05/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-container.h"
#include "Colors.h"
     
/*
 * function    : Container
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
Container::Container(SQI_BWidget *ptr,BView *t,BRect frame,uint32 resizingMode,uint32 flags)
	:BView(frame,NULL,resizingMode,flags | B_FRAME_EVENTS) , SQI_Widget(ptr)
{	
	SetViewColor(BeButtonGrey);
	target = t;
	AddChild(target);
	
	w = 0;
	h = 0;
}


/*
 * function    : ~Container
 * purpose     : View destructor
 * input       : none
 * output      : none
 * side effect : none
 */
Container::~Container()
{
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void Container::AttachedToWindow(void)
{			
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
					
	BView::AttachedToWindow();	
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void Container::AllAttached(void)
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
void Container::GetPreferredSize(float *width,float *height)
{
	if(w == 0 && h == 0)
	{	
		if(CountChildren())
		{	
			BView *child = ChildAt(0);
			BRect frame(0,0,0,0); 
			while(child)
			{
				frame = frame | child->Frame();
				child = child->NextSibling();	
			}
			
			w = frame.Width();
			h = frame.Height();
		}
	}

	*width = w;
	*height = h;
}

void Container::FrameResized(float width,float height)
{		
	BWidget->Adapt(width,height);		
}
