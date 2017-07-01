/*
 * Squirrel project
 *
 * file ?	SQI-colorcontrol.cpp
 * what	?   colorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	02/04/01
 *
 *
 * (c) Kirilla 1999-2001
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
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
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
	BColorControl::AttachedToWindow();		
	SetTarget(this,NULL);
	SQI_Widget::AttachedToWindow(this);	
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
	SQI_Widget::DetachedFromWindow(this);			
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
	SQI_Widget::Draw(this,updateRect);			
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
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))			
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
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
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
	SQI_Widget::MakeFocus(this,focused);	
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
	if(!SQI_Widget::MessageReceived(this,message))
		BColorControl::MessageReceived(message);			
}

/*
 * function    : Invoked
 * purpose     : the widget is invoked
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_ColorControl::Invoked(BView *view)
{
	SQI_List *color = new SQI_List(Squirrel->LocalHeap);
	rgb_color c = ValueAsColor();	
	
	GetListFromColor(Squirrel,color,&c);	
		 	
	Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),color,NO_SYSTEM_HOOK);	
	
	SQI_Widget::Invoked(view);	
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
	if(!SQI_Widget::MouseDown(this,point))
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
	SQI_Widget::MouseMoved(this,point,transit,message);				
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
	SQI_Widget::Pulse(this);
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
	SQI_Widget::WindowActivated(this,active);			
	BColorControl::WindowActivated(active);
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
