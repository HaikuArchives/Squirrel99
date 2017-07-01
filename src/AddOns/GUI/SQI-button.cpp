/*
 * Squirrel project
 *
 * file ?	SQI-view.cpp
 * what	?   view object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbutton.h"
#include "SQI-glue.h"
#include "SQI-button.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_Button
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
SQI_Button::SQI_Button(SQI_Interp *interp,SQI_BButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags)
	:BButton(frame,NULL,label,message,resizingMode,flags) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();	
	
	makedefault = false; 
}


/*
 * function    : ~SQI_Button
 * purpose     : Button destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Button::~SQI_Button()
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
void SQI_Button::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	
	BButton::AttachedToWindow();	
	
	SetTarget(this,NULL);
			
	if(makedefault)
		Window()->SetDefaultButton(this);
	
	SQI_Widget::AttachedToWindow(this);	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::DetachedFromWindow(void)
{				
 	SQI_Widget::DetachedFromWindow(this);					
	BButton::DetachedFromWindow();	
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
void SQI_Button::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);			
	BButton::Draw(updateRect);			
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
void SQI_Button::KeyDown(const char *bytes,int32 numBytes)
{	
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))
		BButton::KeyDown(bytes,numBytes);
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
void SQI_Button::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BButton::KeyUp(bytes,numBytes);
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
void SQI_Button::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BButton::MakeFocus(focused);		
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
void SQI_Button::MessageReceived(BMessage *message)
{
	if(!SQI_Widget::MessageReceived(this,message))
		BButton::MessageReceived(message);			
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
void SQI_Button::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BButton::MouseDown(point);		
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
void SQI_Button::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);	
	BButton::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Button::Pulse(void)
{
	SQI_Widget::Pulse(this);
	BButton::Pulse();
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
void SQI_Button::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);
	BButton::WindowActivated(active);
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
void SQI_Button::GetPreferredSize(float *width,float *height)
{
	BButton::GetPreferredSize(width,height);	
	
	if(char_width>0 || char_height>0)
	{
		if(char_width>0)
		{	
			string s(char_width,'X');		
			*width = StringWidth(s.c_str()) + 20;
		}
		
		if(char_height>0)
		{
			font_height h;
			GetFontHeight(&h);
		
			*height = char_height * (ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1) - 1 + 10;	
		}	
	}
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_Button *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Button::Clone(SQI_Button *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a Button
	
	clone->RealBounds = RealBounds;
	clone->original_width = original_width;
	clone->original_height = original_height;
	clone->char_width = char_width;
	clone->char_height = char_height;

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
	
	// default button ?
	
	clone->makedefault = makedefault;
	
	// that's all	
}
