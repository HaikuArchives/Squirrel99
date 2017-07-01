/*
 * Squirrel project
 *
 * file ?	SQI-colorcontrol.h
 * what	?   colorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	02/04/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <ColorControl.h>
#include "Hook.h"

class SQI_ColorControl : public BColorControl , public SQI_Widget
{
	public:
		SQI_ColorControl(SQI_Interp *interp,SQI_BColorControl *ptr,color_control_layout matrix,float cellSide,BMessage *message,BPoint lefttop);
		~SQI_ColorControl();
		// Hook
		void Invoked(BView *view);
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
		void Clone(SQI_ColorControl *clone);
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BColorControl *MagicPtr;	// Pointer to the SQI_BView
};
