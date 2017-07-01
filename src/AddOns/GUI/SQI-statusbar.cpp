/*
 * Squirrel project
 *
 * file ?	SQI-statusbar.cpp
 * what	?   statusbar object
 * who	?	jlv
 * when	?	09/19/00
 * last	?	02/05/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bstatusbar.h"
#include "SQI-glue.h"
#include "SQI-statusbar.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_StatusBar
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
SQI_StatusBar::SQI_StatusBar(SQI_Interp *interp,SQI_BStatusBar *ptr,const char *label,const char *trailing,int ttwidth,int trwidth)
	:BStatusBar(BRect(0,0,0,0),NULL,label,trailing) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;

	tt_width = ttwidth;
	tr_width = trwidth;
	
	RealBounds = Bounds();	
}


/*
 * function    : ~SQI_StatusBar
 * purpose     : StatusBar destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_StatusBar::~SQI_StatusBar()
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
void SQI_StatusBar::AttachedToWindow(void)
{		
	BStatusBar::AttachedToWindow();	
	SQI_Widget::AttachedToWindow(this);	
	
	if(Parent())
	{
		if(!lcolor_set)	
			SetLowColor(Parent()->ViewColor()); // !!!!		
	}					
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_StatusBar::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);			
	BStatusBar::DetachedFromWindow();	
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
void SQI_StatusBar::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);					
	BStatusBar::Draw(updateRect);			
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
void SQI_StatusBar::KeyDown(const char *bytes,int32 numBytes)
{	
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))		
		BStatusBar::KeyDown(bytes,numBytes);
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
void SQI_StatusBar::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BStatusBar::KeyUp(bytes,numBytes);
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
void SQI_StatusBar::MessageReceived(BMessage *message)
{			
	if(!SQI_Widget::MessageReceived(this,message))
		BStatusBar::MessageReceived(message);					
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
void SQI_StatusBar::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BStatusBar::MouseDown(point);		
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
void SQI_StatusBar::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);			
	BStatusBar::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_StatusBar::Pulse(void)
{
	SQI_Widget::Pulse(this);	
	BStatusBar::Pulse();
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
void SQI_StatusBar::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);			
	BStatusBar::WindowActivated(active);
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
void SQI_StatusBar::GetPreferredSize(float *width,float *height)
{
	BStatusBar::GetPreferredSize(width,height);	
	
	// we use the text width and the trailing width
	
	string s(tt_width + tr_width + 4,'X');		

	*width = StringWidth(s.c_str());
	
	// and we add it to what we allready have
	
	*width = *width + StringWidth(Label()) + StringWidth(TrailingLabel());
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_StatusBar *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_StatusBar::Clone(SQI_StatusBar *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a StatusBar
	
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
