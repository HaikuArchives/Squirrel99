/*
 * Squirrel project
 *
 * file ?	SQI-view.h
 * what	?   view object
 * who	?	jlv
 * when	?	11/24/99
 * last	?	12/07/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <View.h>
#include "Hook.h"

/* hooks */

#define HOOK_VIEW_WINDOWACTIVATED		0
#define HOOK_VIEW_ATTACHEDTOWINDOW		1
#define HOOK_VIEW_DETACHEDFROMWINDOW	2
#define HOOK_VIEW_DRAW					3
#define HOOK_VIEW_ENTERED				4
#define HOOK_VIEW_EXITED				5
#define HOOK_VIEW_MAKEFOCUS				6
#define HOOK_VIEW_KEYDOWN				7
#define HOOK_VIEW_KEYUP					8
#define HOOK_VIEW_MESSAGE				9
#define HOOK_VIEW_MOUSEDOWN				10
#define HOOK_VIEW_MOUSEUP				11		// not yet implemented by Be
#define HOOK_VIEW_FRAMEMOVED			12
#define HOOK_VIEW_PULSE					13
#define HOOK_VIEW_FRAMERESIZED			14

#define HOOKS_VIEW 15

class SQI_View : public BView , public SQI_Widget
{
	public:
		SQI_View(SQI_Interp *interp,SQI_BView *ptr,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_View();
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
		// Gluing methods
		void ReGluing();
		void DynamicGluing();
		void StaticGluing();
		void ExpandChilds();
		// Cloning methods
		void Clone(SQI_View *clone);
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BView *MagicPtr;	// Pointer to the SQI_BView
};
