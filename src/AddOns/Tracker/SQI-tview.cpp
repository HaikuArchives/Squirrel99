/*
 * Squirrel project
 *
 * file ?	SQI-tview.cpp
 * what	?   tview object
 * who	?	jlv
 * when	?	02/08/01
 * last	?	02/08/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-btview.h"
#include "SQI-glue.h"
#include "SQI-tview.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_TView
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
SQI_TView::SQI_TView(SQI_Interp *interp,SQI_BTView *ptr,Model *model,uint32 mode,BRect frame,uint32 resizingMode,uint32 flags)
	:BPoseView(model,frame,mode) ,
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	 
}


/*
 * function    : ~SQI_TView
 * purpose     : TView destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_TView::~SQI_TView()
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
void SQI_TView::AttachedToWindow(void)
{
	SQI_Widget::AttachedToWindow(this);			
	BPoseView::AttachedToWindow();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_TView::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);				
	BPoseView::DetachedFromWindow();	
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
void SQI_TView::Draw(BRect updateRect)
{			
	SQI_Widget::Draw(this,updateRect);
	BPoseView::Draw(updateRect);					
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
void SQI_TView::FrameMoved(BPoint parentPoint)
{
	SQI_Widget::FrameMoved(this,parentPoint);			
	BPoseView::FrameMoved(parentPoint);	
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
void SQI_TView::FrameResized(float width,float height)
{	
	SQI_Widget::FrameResized(this,width,height);
	BPoseView::FrameResized(width,height);					
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
void SQI_TView::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))	
		BPoseView::KeyDown(bytes,numBytes);
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
void SQI_TView::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BPoseView::KeyUp(bytes,numBytes);
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
void SQI_TView::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BPoseView::MakeFocus(focused);		
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
void SQI_TView::MessageReceived(BMessage *message)
{			
	if(!SQI_Widget::MessageReceived(this,message))
		BPoseView::MessageReceived(message);					
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
void SQI_TView::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BPoseView::MouseDown(point);		
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
void SQI_TView::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);					
	BPoseView::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_TView::Pulse(void)
{
	SQI_Widget::Pulse(this);	
	BPoseView::Pulse();
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
void SQI_TView::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);	
	BPoseView::WindowActivated(active);
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
void SQI_TView::GetPreferredSize(float *width,float *height)
{
		
	// for now we return the original size, but when we will
	// have view attached to it, it will be diferrent	
		
	*width = original_width;
	*height = original_height;
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
void SQI_TView::Clone(SQI_TView *clone)
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
