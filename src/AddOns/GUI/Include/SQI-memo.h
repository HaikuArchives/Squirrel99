/*
 * Squirrel project
 *
 * file ?	SQI-memo.h
 * what	?   memo object
 * who	?	jlv
 * when	?	02/20/00
 * last	?	03/13/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <TextView.h>
#include <Font.h>
#include "Hook.h"

class SQI_Memo : public BTextView , public SQI_Widget
{
	public:
		SQI_Memo(SQI_Interp *interp,SQI_BMemo *ptr,int cw,int l,BRect frame,uint32 flags);
		~SQI_Memo();
		// Hook
		void AttachedToWindow(void);
		void AllAttached(void);
		void DetachedFromWindow(void);
		void Draw(BRect updateRect);
		void FrameMoved(BPoint parentPoint);
		void FrameResized(float width,float height);
		void KeyDown(const char *bytes,int32 numBytes);
		void KeyUp(const char *bytes,int32 numBytes);
		void MakeFocus(bool focused = true);
		void MessageReceived(BMessage *message);
		void MouseDown(BPoint point);
		void MouseMoved(BPoint point,uint32 transit,const BMessage *message);
		void Pulse(void);	
		void WindowActivated(bool active);
		void GetPreferredSize(float *width,float *height);
		void SetPreferredSize();	
		// Cloning methods
		void Clone(SQI_Memo *clone);
		// some usefull methods	
		void XTextRect();
	public:	
		float extra;	
	private:
		BRect RealBounds;
		float original_width;
		float original_height;
		SQI_BMemo *MagicPtr;	// Pointer to the SQI_BMemo
};

BRect RectFromCharSize(BFont *font,int Chars,int Lines);
