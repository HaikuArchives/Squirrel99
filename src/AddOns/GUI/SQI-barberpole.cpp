/*
 * Squirrel project
 *
 * file ?	SQI-barberpole.cpp
 * what	?   barberpole widget
 * who	?	jlv
 * when	?	10/14/00
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
#include "SQI-bbarberpole.h"
#include "SQI-glue.h"
#include "SQI-barberpole.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_BarberPole
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
SQI_BarberPole::SQI_BarberPole(SQI_Interp *interp,SQI_BBarberPole *ptr,int width,int height,int pDirection,uint32 flags)
	:BView(BRect(0,0,width,height), NULL, B_FOLLOW_NONE, flags | B_WILL_DRAW) ,
	SQI_Widget(ptr)
{	
	BRect bounds = Bounds();
	Interpreter = interp;
	MagicPtr = ptr;
		
 	original_width = width;
	original_height = height;
	
	RealBounds = Bounds();	
	
	is_running = false;
	direction = pDirection;
	pattern		lStripes;
	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;
	FillRect(Bounds(), lStripes);
}


/*
 * function    : ~SQI_BarberPole
 * purpose     : BarberPole destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BarberPole::~SQI_BarberPole()
{
	MagicPtr->View = NULL;
} 

void SQI_BarberPole::Start()
{
	resume_thread(spinning_thread_id); 
	is_running = true;
}

void SQI_BarberPole::Stop()
{
	suspend_thread(spinning_thread_id); 
	is_running = false;
}

bool SQI_BarberPole::IsRunning()
{
	return is_running;
}

int32 SQI_BarberPole::spinningThread(void *data)
{
	SQI_BarberPole	*lBarberPole = (SQI_BarberPole*)data;
	pattern		lStripes;

	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;	
	
	while(1==1){
		lBarberPole->Window()->Lock();
		//lBarberPole->LockLooper();	
		lBarberPole->FillRect(lBarberPole->Bounds(), lStripes);
		//lBarberPole->UnlockLooper();
		lBarberPole->Window()->Unlock();
			
		if(lBarberPole->direction == FROM_RIGHT_TO_LEFT){
			uchar tmp = lStripes.data[0];
			for (int j = 0; j < 7; ++j) {
	  			lStripes.data[j] = lStripes.data[j+1];
			}
			lStripes.data[7] = tmp;
		} else {
			uchar tmp = lStripes.data[7];
			for (int j = 7; j > 0; --j) {
	  			lStripes.data[j] = lStripes.data[j-1];
			}
			lStripes.data[0] = tmp;
		
		}
		snooze(50000);
	}
}

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::AttachedToWindow(void)
{
	// the view is attached to a Window, we set some variable	
	
	BView::AttachedToWindow();
	SQI_Widget::AttachedToWindow(this);	
	// create the thread	
	spinning_thread_id = spawn_thread(spinningThread,NULL,B_LOW_PRIORITY,this);		
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);
	// kill the thread
	kill_thread(spinning_thread_id);
	spinning_thread_id = 0;		
			
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
void SQI_BarberPole::Draw(BRect updateRect)
{	
	Window()->Lock();
	pattern		lStripes;
	lStripes.data[0] = 0x0f;
	lStripes.data[1] = 0x1e;
	lStripes.data[2] = 0x3c;
	lStripes.data[3] = 0x78;
	lStripes.data[4] = 0xf0;
	lStripes.data[5] = 0xe1;
	lStripes.data[6] = 0xc3;
	lStripes.data[7] = 0x87;
	FillRect(Bounds(), lStripes);
	Window()->Unlock();
	
	SQI_Widget::Draw(this,updateRect);			
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
void SQI_BarberPole::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))		
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
void SQI_BarberPole::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
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
void SQI_BarberPole::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
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
void SQI_BarberPole::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))	
		BView::MouseDown(point);		
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
void SQI_BarberPole::MessageReceived(BMessage *message)
{		
	if(!SQI_Widget::MessageReceived(this,message))	
		BView::MessageReceived(message);	
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
void SQI_BarberPole::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);					
	BView::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::Pulse(void)
{	
	SQI_Widget::Pulse(this);	
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
void SQI_BarberPole::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);			
	BView::WindowActivated(active);
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
void SQI_BarberPole::GetPreferredSize(float *width,float *height)
{
	*width = original_width;
	*height = original_height;
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_BarberPole *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_BarberPole::Clone(SQI_BarberPole *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a BarberPole
	
	// the font used
	
	BFont font;
	GetFont(&font);					
	clone->SetFont(&font);
		
	// color
	
	clone->SetHighColor(HighColor());
	clone->SetLowColor(LowColor());
	clone->SetViewColor(ViewColor());
	
	// that's all	
}


