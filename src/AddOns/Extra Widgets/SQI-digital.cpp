/*
 * Squirrel project
 *
 * file ?	SQI-viewer.cpp
 * what	?   viewer object
 * who	?	jlv
 * when	?	02/06/01
 * last	?	02/06/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bdigital.h"
#include "SQI-glue.h"
#include "SQI-digital.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"

#include <math.h>

#include <Bitmap.h>

extern BBitmap *BitmapDigits[10];
     
/*
 * function    : SQI_Digital
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
SQI_Digital::SQI_Digital(SQI_Interp *interp,SQI_BDigital *ptr,int digit,uint32 flags)
	:BView(BRect(0,0,(digit * 15) - 1,19),NULL,B_FOLLOW_NONE,flags | B_WILL_DRAW) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	
	Point = -1;
	Precision = 2;
	Digit = digit;
	
	RealBounds = Bounds();	
	
	original_width =  (digit * 15);
	original_height = 20;
	
	D = new short[Digit];
	Dirty = new bool[Digit];
	
	for(int i=0;i<Digit;i++)
	{
		D[i] 		= 10;				// all digit are empty for now
		Dirty[i] 	= true;	
	}
}


/*
 * function    : ~SQI_Digital
 * purpose     : Digital destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Digital::~SQI_Digital()
{
	MagicPtr->View = NULL;
	delete D;
	delete Dirty;
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Digital::AttachedToWindow(void)
{
	SQI_Widget::AttachedToWindow(this);			
	BView::AttachedToWindow();
	SetViewColor(Black); // default color
	SetHighColor(BeButtonGrey);	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Digital::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);				
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
void SQI_Digital::Draw(BRect updateRect)
{	
	for(int i=0;i<Digit;i++)
		Dirty[i] = true;

	DrawDigits();
				
	// call the widget method now			
	SQI_Widget::Draw(this,updateRect);					
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
void SQI_Digital::FrameMoved(BPoint parentPoint)
{
	SQI_Widget::FrameMoved(this,parentPoint);			
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
void SQI_Digital::FrameResized(float width,float height)
{	
	SQI_Widget::FrameResized(this,width,height);
	BView::FrameResized(width,height);					
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
void SQI_Digital::KeyDown(const char *bytes,int32 numBytes)
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
void SQI_Digital::KeyUp(const char *bytes,int32 numBytes)
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
void SQI_Digital::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BView::MakeFocus(focused);		
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
void SQI_Digital::MessageReceived(BMessage *message)
{			
	if(!SQI_Widget::MessageReceived(this,message))
		BView::MessageReceived(message);					
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
void SQI_Digital::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
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
void SQI_Digital::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
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
void SQI_Digital::Pulse(void)
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
void SQI_Digital::WindowActivated(bool active)
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
void SQI_Digital::GetPreferredSize(float *width,float *height)
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
 * SQI_View *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Digital::Clone(SQI_Digital *clone)
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

void SQI_Digital::Display(SQI_Number *num)
{
	switch(num->IsA())
	{
		case SQI_BOOL:
		{
			Display((int)(num->bData()));
			break;	
		}
		case SQI_INT:
		{
			Display(num->iData());
			break;	
		}			
		case SQI_LONG:
		{
			Display(num->lData());
			break;	
		}
		case SQI_FLOAT:
		{
			Display(num->fData());
			break;	
		}				
		case SQI_DOUBLE:
		{
			Display(num->dData());
			break;	
		}		
	}	
}

void SQI_Digital::Empty()
{
	Point = -1;	
	
	for(int i=0;i<Digit;i++)
		if(D[i]!=10)
		{
			Dirty[i] = true;
			D[i] = 10;	
		}	
}

void SQI_Digital::Display(int num)
{
	char *buffer;
	int size;
	int i,j;
	int ab;
			
	ab = abs(num);
		
	Empty();
	
	if(Point!=-1)	
	{
		Dirty[Point] = true;
		Point = -1;	
	}
			
	if(ab>9)		
		size = ceil(log10(ab)+1);
	else
		size = 1;	

	buffer = new char[size+1];
	
	sprintf(buffer,"%d",ab);
		
	j = strlen(buffer) - 1;	
	
	for(i=Digit-1;i>=0;i--)
	{
		if(buffer[j]!='\0')
		{
			D[i] = buffer[j--] - 48;
			Dirty[i] = true;
			if(j<0)
				break;
		}	
		else
			break;		
	}	
	
	//cout << num << "\n";		
	
	i--;
		
	if(num<0)	
	{
		//cout << "neg " << i << "\n";
		if(i>=0)
		{
			D[i] = 11;					
			Dirty[i] = true;	
		}	
	}
	
					
	delete [] buffer;
	
	DrawDigits();
}

void SQI_Digital::Display(long num)
{
	char *buffer;
	int size;
	int i,j;
	long ab;
	
	//cout << num << "\n";
		
	Empty();
	
	if(isnan(num)) // nan !!
	{
		D[Digit-1] = 12;
		Dirty[Digit-1] = true;	
	}
	else
	{		
		ab = abs(num);	
			
		if(ab>9)			
			size = ceil(log10(ab));
		else
			size = 1;
	
		buffer = new char[size+1];
		
		sprintf(buffer,"%ld",ab);
		
		j = strlen(buffer) - 1;
		
		for(i=Digit-1;i>=0;i--)
		{
			if(buffer[j]!='\0')
			{
				D[i] = buffer[j--] - 48;
				Dirty[i] = true;
				if(j<0)
					break;
			}
			else
				break;			
		}			
		
		i--;
		
		if(Point!=-1)	
		{
			Dirty[Point] = true;
			Point = -1;	
		}		
		
		if(num<0)	
			if(i>=0)
			{
				D[i] = 11;
				Dirty[i] = true;				
			}	
						
		delete [] buffer;
	
	}	
		
	DrawDigits();
}

void SQI_Digital::Display(double num)
{
	char format[7];
	char *buffer;
	int size;
	int i,j;
	int entier;
		
	Empty();
	
	if(Point!=-1)	
	{
		Dirty[Point] = true;
		Point = -1;	
	}		

	if(isnan(num)) // nan !!
	{
		D[Digit-1] = 12;
		Dirty[Digit-1] = true;	
	}
	else
	{
		entier = (int)floor(fabs(num));	
		
		if(entier>9)
			size = ceil(log10(entier)) + Precision + 1;	
		else
			size = Precision + 2;
	
		buffer = new char[size+1];
		
		sprintf(format,"%%.%dlf",Precision);	
		
		sprintf(buffer,format,fabs(num));
		
		j = strlen(buffer) - 1;	
		
		for(i=Digit-1;i>=0;i--)
		{
			if(buffer[j]!='.')
			{
				if(buffer[j]!='\0')
				{				
					D[i] = buffer[j--] - 48;
					Dirty[i] = true;
					if(j<0)
						break;
				}
				else
					break;			
			}
			else
			{
				Point = i;
				j--;
				i++;	
			}	
		}			
		
		i--;
			
		if(num<0)	
			if(i>=0)
			{
				D[i] = 11;
				Dirty[i] = true;				
			}	
						
		delete [] buffer;
		
		if(Point!=-1)
			recp.Set((float)(15 * (Point + 1))-3,(float)17.0,(float)(15 * (Point + 1)-2),(float)18.0);
	}	
	
	Invalidate();	
}

void SQI_Digital::Display(float num)
{
	Display((double)num);
}

void SQI_Digital::DrawDigits()
{
	BPoint p(0,0);
	
	// draw all the digits				
				
	if(BitmapDigits[0])
	{			
		for(int i=0;i<Digit;i++)
		{
			if(Dirty[i])
			{
				DrawBitmapAsync(BitmapDigits[D[i]],p);
				Dirty[i] = false;	
			}	
			p.x += 15;		
		}					
		
		Sync();
		
		// draw the floating point if necessary
		if(Point!=-1)
			FillRect(recp);		
	}		
}
