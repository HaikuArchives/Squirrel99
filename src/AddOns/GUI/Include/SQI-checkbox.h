/*
 * Squirrel project
 *
 * file ?	SQI-checkbox.h
 * what	?   checkbox object
 * who	?	jlv
 * when	?	12/02/99
 * last	?	12/18/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <CheckBox.h>
#include "Hook.h"

class SQI_CheckBox : public BCheckBox , public SQI_Widget
{
	public:
		SQI_CheckBox(SQI_Interp *interp,SQI_BCheckBox *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_CheckBox();
		// Hook
		void Invoked(BView *view,BMessage *message);
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
		void Clone(SQI_CheckBox *clone);
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BCheckBox *MagicPtr;	// Pointer to the SQI_BView
};
