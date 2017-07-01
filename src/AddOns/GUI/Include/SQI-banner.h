/*
 * Squirrel project
 *
 * file ?	SQI-banner.h
 * what	?   banner object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/03/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <StringView.h>
#include "Hook.h"

class SQI_Banner : public BStringView , public SQI_Widget
{
	public:
		SQI_Banner(SQI_Interp *interp,SQI_BBanner *ptr,BRect frame,char *text,alignment align,uint32 flags);
		~SQI_Banner();
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
		void Clone(SQI_Banner *clone);
		int char_width;
		int char_height;
	private:
		BRect RealBounds;
		SQI_BBanner *MagicPtr;	// Pointer to the SQI_BBanner
};
