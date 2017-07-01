/*
 * Squirrel project
 *
 * file ?	SQI-button.h
 * what	?   button object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	02/03/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Button.h>
#include "Hook.h"

class SQI_Button : public BButton , public SQI_Widget
{
	public:
		SQI_Button(SQI_Interp *interp,SQI_BButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_Button();
		// Hook
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
		// Cloning methods
		void Clone(SQI_Button *clone);
		// variable
		bool makedefault;			// indicate if we need to make the button the default when attached to a window
		int char_width;
		int char_height;
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BButton *MagicPtr;	// Pointer to the SQI_BView
};
