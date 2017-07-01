/*
 * Squirrel project
 *
 * file ?	SQI-radiobutton.h
 * what	?   radiobutton object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/05/01
 *
 *
 * (c) electron Technology 1999-2001
 */
 
#include <RadioButton.h>
#include "Hook.h"

class SQI_RadioButton : public BRadioButton , public SQI_Widget
{
	public:
		SQI_RadioButton(SQI_Interp *interp,SQI_BRadioButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_RadioButton();
		// Hook
		void Invoked(BView *view,BMessage *message = NULL);
		void AttachedToWindow(void);
		void DetachedFromWindow(void);
		void Draw(BRect updateRect);
		void KeyDown(const char *bytes,int32 numBytes);
		void KeyUp(const char *bytes,int32 numBytes);
		void MakeFocus(bool focused = true);
		void MessageReceived(BMessage *message);
		void MouseDown(BPoint point);
		void MouseMoved(BPoint point,uint32 transit,const BMessage *message);
		void Pulse(void);	
		void WindowActivated(bool active);
		void GetPreferredSize(float *width,float *height);
		void SetValue(int32 value);	
		// Cloning methods
		void Clone(SQI_RadioButton *clone);
		int char_width;
		int char_height;
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BRadioButton *MagicPtr;	// Pointer to the SQI_BView
};
