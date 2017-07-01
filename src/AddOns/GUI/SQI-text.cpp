/*
 * Squirrel project
 *
 * file ?	SQI-text.cpp
 * what	?   text object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/05/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-btext.h"
#include "SQI-glue.h"
#include "SQI-text.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_Text
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
SQI_Text::SQI_Text(SQI_Interp *interp,SQI_BText *ptr,BRect frame,const char *text,alignment align,uint32 flags)
	:BStringView(frame,NULL,text,B_FOLLOW_NONE,flags) , 
	SQI_Widget(ptr)
{	
	SetAlignment(align);
	Interpreter = interp;
	MagicPtr = ptr;	 
}


/*
 * function    : ~SQI_Text
 * purpose     : Text destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Text::~SQI_Text()
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
void SQI_Text::AttachedToWindow(void)
{
	SQI_Widget::AttachedToWindow(this);		
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Text::DetachedFromWindow(void)
{		
	SQI_Widget::DetachedFromWindow(this);			
	BStringView::DetachedFromWindow();	
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
void SQI_Text::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);			
	BStringView::Draw(updateRect);			
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
void SQI_Text::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))	
		BStringView::KeyDown(bytes,numBytes);
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
void SQI_Text::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BStringView::KeyUp(bytes,numBytes);
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
void SQI_Text::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);
	BStringView::MakeFocus(focused);		
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
void SQI_Text::MessageReceived(BMessage *message)
{			
	if(!SQI_Widget::MessageReceived(this,message))
		BStringView::MessageReceived(message);					
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
void SQI_Text::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BStringView::MouseDown(point);		
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
void SQI_Text::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);				
	BStringView::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Text::Pulse(void)
{	
	SQI_Widget::Pulse(this);
	BStringView::Pulse();
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
void SQI_Text::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);		
	BStringView::WindowActivated(active);
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
void SQI_Text::GetPreferredSize(float *width,float *height)
{
	BStringView::GetPreferredSize(width,height);
		
	if(char_width>0 || char_height>0)
	{
		if(char_width>0)
		{	
			string s(char_width,'X');		
			*width = StringWidth(s.c_str());
		}
		
		if(char_height>0)
		{
			font_height h;
			GetFontHeight(&h);
			
			//cout << ceil(h.ascent)+ceil(h.descent) << "\n";
			
			*height = char_height * (ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1); // - 1;// + 10;	
		}	
	}	
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_Text *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Text::Clone(SQI_Text *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a Text
	
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
	
	// size in chars
	
	clone->char_width = char_width;
	clone->char_height = char_height;	
	
	// that's all	
}


