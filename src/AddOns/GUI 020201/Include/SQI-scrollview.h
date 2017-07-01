/*
 * Squirrel project
 *
 * file ?	SQI-scrollview.h
 * what	?   scrollview object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	12/07/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <ScrollView.h>
#include "Hook.h"

/* hooks */

#define HOOK_SCROLLVIEW_WINDOWACTIVATED		0
#define HOOK_SCROLLVIEW_ATTACHEDTOWINDOW	1
#define HOOK_SCROLLVIEW_DETACHEDFROMWINDOW	2
#define HOOK_SCROLLVIEW_DRAW				3
#define HOOK_SCROLLVIEW_ENTERED				4
#define HOOK_SCROLLVIEW_EXITED				5
#define HOOK_SCROLLVIEW_MAKEFOCUS			6
#define HOOK_SCROLLVIEW_KEYDOWN				7
#define HOOK_SCROLLVIEW_KEYUP				8
#define HOOK_SCROLLVIEW_MESSAGE				9
#define HOOK_SCROLLVIEW_MOUSEDOWN			10
#define HOOK_SCROLLVIEW_MOUSEUP				11		// not yet implemented by Be
#define HOOK_SCROLLVIEW_FRAMEMOVED			12
#define HOOK_SCROLLVIEW_PULSE				13
#define HOOK_SCROLLVIEW_FRAMERESIZED		14

#define HOOKS_SCROLLVIEW 15

class SQI_ScrollView : public BScrollView , public SQI_Widget
{
	public:
		SQI_ScrollView(SQI_Interp *interp,SQI_BScrollView *ptr,BView *target,border_style border,bool h,bool v,uint32 flags);
		~SQI_ScrollView();
		// Hook
		void AttachedToWindow(void);
		void AllAttached(void);
		void AllDetached(void);
		void DetachedFromWindow(void);
		void Draw(BRect updateRect);
		void FrameMoved(BPoint parentPoint);
		void FrameResized(float width,float height);
		void KeyDown(const char *bytes,int32 numBytes);
		void KeyUp(const char *bytes,int32 numBytes);
		void MakeFocus(bool focused = true);
		void MouseDown(BPoint point);
		void MouseMoved(BPoint point,uint32 transit,const BMessage *message);
		void Pulse(void);	
		void WindowActivated(bool active);
		void ResizeToPreferred();
		void GetPreferredSize(float *width,float *height);	
		// Cloning methods
		void Clone(SQI_ScrollView *clone);
	private:
		BRect RealBounds;
		float original_width;
		float original_height;
		SQI_BScrollView *MagicPtr;	// Pointer to the SQI_BScrollView
};
