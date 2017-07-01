/*
 * Squirrel project
 *
 * file ?	SQI-text.h
 * what	?   text object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/05/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <StringView.h>
#include "Hook.h"

class SQI_Text : public BStringView , public SQI_Widget
{
	public:
		SQI_Text(SQI_Interp *interp,SQI_BText *ptr,BRect frame,const char *text,alignment align,uint32 flags);
		~SQI_Text();
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
		// others methods	
		void GetPreferredSize(float *width,float *height);
		// Cloning methods
		void Clone(SQI_Text *clone);
		int char_width;
		int char_height;
	private:
		BRect RealBounds;
		SQI_BText *MagicPtr;	// Pointer to the SQI_BText
};
