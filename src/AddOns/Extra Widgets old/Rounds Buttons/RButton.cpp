/*
 * Extra Widgets
 *
 * Some more cool widgets
 *
 * file ?	RButton.cpp
 * what	?	Round Button class 
 * who	?	jlv
 * when	?	02/04/00
 * last	?	02/24/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Font.h> 
#include "RButton.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

/*
 * function    : RButton
 * purpose     : Create a RButton
 * input       : 
 *
 * BRect frame, frame of the RButton
 * char *name, name of the widget
 * BMessage *message, message to send when the widget is invoked
 * uint32 resizingMode, resizing mode
 * int32 flags, flags of the widget
 *
 * output      : none
 * side effect : none
 */
RButton::RButton(BRect frame,const char *name,const char *label,BMessage *message,uint32 resizingMode,uint32 flags)
	:BButton(frame,name,label,message,resizingMode, flags | B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS)
{
	// we set the default color
	ShadowColor = BeShadow;
	ButtonColor = BeButtonGrey;
	OnColor = ButtonColor;
	OnColor.red -= 30;
	OnColor.green -= 30;
	OnColor.blue -= 30;
	// and level
	Level = 3;
	// and the status
	Status = false;
	NbLabels = 0;
	Labels = NULL;	
	
	SubLabel(label);
}

/*
 * function    : ~RButton
 * purpose     : destroy a RButton
 * input       : none
 * output      : none
 * side effect : none
 */
RButton::~RButton()
{
	if(NbLabels)
	{
		for(int i=0;i<NbLabels;i++)
			free(Labels[i]);
		free(Labels);
		free(Labelx);
		free(Labely);	
	}
}

/*
 * function    : FrameResized
 * purpose     : the widget is resized
 * input       :
 *
 * float width, width of the widget
 * float height, height of the widget
 *
 * output      : none
 * side effect : none
 */
void RButton::FrameResized(float width,float height)
{
	//BButton::FrameResized(width,height);
	LabelPos();
	Invalidate();
}

/*
 * function    : GetPreferredSize
 * purpose     : compute the preferred size of the widget
 * input       :
 *
 * float *width, width of the widget
 * float *height, height of the widget
 *
 * output      : none
 * side effect : none
 */
void RButton::GetPreferredSize(float *width,float *height)
{
	font_height fh;			
	GetFontHeight(&fh);
	int h = ceil(fh.ascent)+ceil(fh.descent)+ceil(fh.leading);	
	float max = 0,w;

	*height = 10;
	
	
	for(int i=0;i<NbLabels;i++)
	{
		*height += h;	
		w = StringWidth(Labels[i]);
		if(w>max)
			max = w;
	}
	
	*width = max + 10;
}

/*
 * function    : AttachedToWindow
 * purpose     : the widget is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::AttachedToWindow()
{
	// if there's a parent, we use the same viewcolor
	// and the same font
	
	if(Parent())
	{
		SetViewColor(Parent()->ViewColor());
		BFont font;
		Parent()->GetFont(&font);
		SetFont(&font);	
	}		
	
	LabelPos();		
	
	BButton::AttachedToWindow();
}


/*
 * function    : DrawOff
 * purpose     : Draw the button not clicked
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::DrawOff()
{
	rgb_color high;
	BRect frame = Bounds();
	BRect rect = frame;
		
	rect.left += 2 + Level;
	rect.top += 2 + Level;
	rect.right -= 2;
	rect.bottom -= 2;	
		
	// we save the previous high color	
	high = HighColor();
	// we draw the shadow
	SetHighColor(ShadowColor);
	FillRoundRect(rect,8,8);
	// and now the button	
	SetHighColor(ButtonColor);
	rect.OffsetBy(-Level,-Level);
	FillRoundRect(rect,8,8);
	
	SetHighColor(White);

	FillArc(BPoint(6,6),6,6,0,360);

	SetHighColor(Red);

	FillArc(BPoint(6,6),4,4,0,360);
	
	// and we set back the high color
	SetHighColor(high);
	
	Status = false;
}

/*
 * function    : DrawOn
 * purpose     : Draw the button clicked
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::DrawOn()
{
	rgb_color high;
	BRect frame = Bounds();
	BRect rect = frame;
		
	rect.left += 2 + Level;
	rect.top += 2 + Level;
	rect.right -= 2;
	rect.bottom -= 2;	
		
	// we save the previous high color	
	high = HighColor();
	// we draw the shadow
	//SetHighColor(ShadowColor);
	//FillRoundRect(rect,8,8);
	// and now the button
	SetHighColor(OnColor);
	rect.OffsetBy(-Level,-Level);
	FillRoundRect(rect,8,8);
	
	// and we set back the high color
	SetHighColor(high);	
}

/*
 * function    : Draw
 * purpose     : Draw the button on-screen
 * input       :
 *
 * BRect updateRect, update frame
 *
 * output      : none
 * side effect : none
 */
void RButton::Draw(BRect updateRect)
{		
	if(Value()==B_CONTROL_OFF)
	{
		DrawOff();
		DrawLabel();
		if(IsFocus())
			DrawFocus();
	}	
	else
	{
		DrawOn();
		// we set the high color to white
		rgb_color high = HighColor();
		rgb_color low = LowColor();
		SetHighColor(White);
		SetLowColor(OnColor);
		DrawLabel();
		SetHighColor(high);
		SetLowColor(low);
	}
}

/*
 * function    : DrawLabel
 * purpose     : Draw the label of the button
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::DrawLabel()
{		
	for(int i=0;i<NbLabels;i++)
	{
		// we need to set the pen position
		MovePenTo(Labelx[i],Labely[i]);
		// and we draw the string
		DrawString(Labels[i]);
	}
}

/*
 * function    : MakeFocus
 * purpose     : Set the focus on the widget or off the widget
 * input       :
 *
 * bool focused, true if focused
 *
 * output      : none
 * side effect : none
 */
void RButton::MakeFocus(bool focused = true)
{
	if(focused)
		DrawFocus();
	else
		ClearFocus();
		
	BView::MakeFocus(focused);		
}

/*
 * function    : DrawFocus
 * purpose     : Draw the focus around the widget
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::DrawFocus()
{	
/*
	// we save the previous high color	
	rgb_color high = HighColor();
	// we draw the shadow
	SetHighColor(BeFocusBlue);
	//MovePenTo(Labelx,Labely+2);
	//StrokeLine(BPoint(Labelx +StringWidth(Label()),Labely+2));
	
	// and we set back the high color
	SetHighColor(high);	
*/

	rgb_color high = HighColor();
	// we draw the shadow
	SetHighColor(BeFocusBlue);
	StrokeRoundRect(Bounds(),8,8);
	
	SetHighColor(high);
}

/*
 * function    : ClearFocus
 * purpose     : Clear the focus around the widget
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::ClearFocus()
{
	/*
	// we save the previous high color	
	rgb_color high = HighColor();
	// we draw the shadow
	SetHighColor(ButtonColor);
	//MovePenTo(Labelx,Labely+2);
	//StrokeLine(BPoint(Labelx +StringWidth(Label()),Labely+2));
	
	// and we set back the high color
	SetHighColor(high);
	*/
	
	rgb_color high = HighColor();
	// we draw the shadow
	SetHighColor(ViewColor());
	StrokeRoundRect(Bounds(),8,8);
	
	SetHighColor(high);	
}

/*
 * function    : SetEnabled
 * purpose     : Set if the widget is enabled or not
 * input       :
 *
 * bool enabled, true if the widget is enabled
 *
 * output      : none
 * side effect : none
 */
void RButton::SetEnabled(bool enabled)
{
		
}

/*
 * function    : SetLabel
 * purpose     : Set the label of the widget
 * input       :
 *
 * char *string, new label
 *
 * output      : none
 * side effect : none
 */
void RButton::SetLabel(const char *string)
{	
	BButton::SetLabel(string);
	LabelPos();		
}

/*
 * function    : SetValue
 * purpose     : Set the value of the widget
 * input       :
 *
 * int32 value, value of the widget (on/off)
 *
 * output      : none
 * side effect : none
 */
void RButton::SetValue(int32 value)
{			
	BButton::SetValue(value);		
}

/*
 * function    : Invoke
 * purpose     : The widget is invoked
 * input       :
 *
 * BMessage *message, message to send
 *
 * output      : none
 * side effect : none
 */
status_t RButton::Invoke(BMessage *message = NULL)
{
	BButton::Invoke(message);	
}

/*
 * function    : LabelPos
 * purpose     : Calculate the position of the labels
 * input       : none
 * output      : none
 * side effect : update Labelx and Labely
 */
void RButton::LabelPos()
{
	BRect frame = Bounds();
	font_height fh;			
	GetFontHeight(&fh);
	int h = ceil(fh.ascent)+ceil(fh.descent)+ceil(fh.leading);
	int mx,my;
	int w;
		
	frame.left += 2;
	frame.top += 2;
	frame.right -= 2 + Level;
	frame.bottom -= 2 + Level;
	
	int wneeded = (h+1) * NbLabels - 1;	
	
	my = (frame.Height() / 2) - (wneeded/2);
	
	for(int i=0;i<NbLabels;i++)
	{	
		w = StringWidth(Labels[i]);
	
		mx = (frame.Width() / 2) - (w/2);
		my += h - 1;	
			
		// we need to set the pen position
	
		Labelx[i] = frame.left + mx + 1;
		Labely[i] = frame.top + my - 1;
	}
}


// extract a sub string from a string
char *substr(char *src,int from,int to)
{
	int j=0;
	char *s = (char *)malloc(sizeof(char)*(to-from+2));	
	
	for(int i=from;i<=to;i++)
		s[j++] = src[i];
	
	s[j] = '\0';
	return s;
}

/*
 * function    : SubLabel
 * purpose     : Divide the label in sub part
 * input       : none
 * output      : none
 * side effect : none
 */
void RButton::SubLabel(char *label)
{	
	int i,j = 0,k = 0;
	
	// we delete the old labels if any
	
	if(NbLabels)
	{
		for(int i=0;i<NbLabels;i++)
			free(Labels[i]);
		free(Labels);
		free(Labelx);
		free(Labely);	
	}	
	
	NbLabels = 0;
	
	// How many labels ?
	
	for(i=0;i<strlen(label);i++)
	{
		if(label[i]=='\n')
			NbLabels++;
	}	
	
	NbLabels++;
	
	Labels = (char **)malloc(sizeof(char *)*NbLabels);
	Labelx = (int *)malloc(sizeof(int)*NbLabels);
	Labely = (int *)malloc(sizeof(int)*NbLabels);
	
	// we take the labels now
	
	for(i=0;i<strlen(label);i++)
	{
		if(label[i]=='\n')
		{
			Labels[k++] = substr(label,j,i-1); 
			j = i+1;	
		}
	}
	
	Labels[k++] = substr(label,j,i-1); 	
}













