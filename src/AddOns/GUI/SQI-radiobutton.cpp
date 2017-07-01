/*
 * Squirrel project
 *
 * file ?	SQI-radiobutton.cpp
 * what	?   radiobutton object
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
#include "SQI-bradiobutton.h"
#include "SQI-glue.h"
#include "SQI-radiobutton.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_RadioButton
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
SQI_RadioButton::SQI_RadioButton(SQI_Interp *interp,SQI_BRadioButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags)
	:BRadioButton(frame,NULL,label,message,resizingMode,flags) , 
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
 * function    : ~SQI_RadioButton
 * purpose     : RadioButton destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_RadioButton::~SQI_RadioButton()
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
void SQI_RadioButton::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	BRadioButton::AttachedToWindow();
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
void SQI_RadioButton::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);			
	BRadioButton::DetachedFromWindow();	
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
void SQI_RadioButton::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);			
	BRadioButton::Draw(updateRect);			
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
void SQI_RadioButton::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))	
		BRadioButton::KeyDown(bytes,numBytes);
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
void SQI_RadioButton::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BRadioButton::KeyUp(bytes,numBytes);
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
void SQI_RadioButton::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BRadioButton::MakeFocus(focused);		
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
void SQI_RadioButton::MessageReceived(BMessage *message)
{
	if(!SQI_Widget::MessageReceived(this,message))
		BRadioButton::MessageReceived(message);		
}

/*
 * function    : Invoked
 * purpose     : The widget is invoked
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_RadioButton::Invoked(BView *view,BMessage *message = NULL)
{
	SQI_Object *old; 
	
	if(Interpreter->GlobalVar->Exist(MagicPtr->Variable->Data()))
		old = Interpreter->GlobalVar->Get(Squirrel,MagicPtr->Variable->Data(),NO_SYSTEM_HOOK);	
	else
		old = Squirrel->interpreter->False;
		
	old->AddRef();	

	Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),MagicPtr->Val,NO_SYSTEM_HOOK);	

	if(Hooks[HOOK_WIDGET_INVOKED].action)
	{
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
			
		args->push_back(BWidget);
		BWidget->AddRef();
		args->push_back(old);
		old->AddRef();
		args->push_back(MagicPtr->Val);
		MagicPtr->Val->AddRef();				
		
		AddExtra(args,Hooks[HOOK_WIDGET_INVOKED].extra);	
				
		UseHook(Hooks[HOOK_WIDGET_INVOKED].action,args,true);
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
void SQI_RadioButton::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BRadioButton::MouseDown(point);		
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
void SQI_RadioButton::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);				
	BRadioButton::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_RadioButton::Pulse(void)
{
	SQI_Widget::Pulse(this);	
	BRadioButton::Pulse();
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
void SQI_RadioButton::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);	
	BRadioButton::WindowActivated(active);
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
void SQI_RadioButton::GetPreferredSize(float *width,float *height)
{
	BRadioButton::GetPreferredSize(width,height);
		
	if(char_width>0 || char_height>0)
	{
		if(char_width>0)
		{	
			string s(char_width,'X');		
			*width = StringWidth(s.c_str()) + 8;
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
 * SQI_RadioButton *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_RadioButton::Clone(SQI_RadioButton *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
		
	// now we set the specific setting of a RadioButton
	
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

void SQI_RadioButton::SetValue(int32 value)
{
	if(value != Value())
		BRadioButton::SetValue(value);	
}
