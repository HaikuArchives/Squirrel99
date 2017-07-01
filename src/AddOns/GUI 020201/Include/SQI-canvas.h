/*
 * Squirrel project
 *
 * file ?	SQI-canvas.h
 * what	?   canvas widget
 * who	?	jlv
 * when	?	03/19/00
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 2000
 */
 
#include <View.h>
#include <Bitmap.h>
#include <Point.h>
#include "Hook.h"

/* hooks */

#define HOOK_CANVAS_WINDOWACTIVATED		0
#define HOOK_CANVAS_ATTACHEDTOWINDOW	1
#define HOOK_CANVAS_DETACHEDFROMWINDOW	2
#define HOOK_CANVAS_DRAW				3
#define HOOK_CANVAS_DROP				4
#define HOOK_CANVAS_ENTERED				5
#define HOOK_CANVAS_EXITED				6
#define HOOK_CANVAS_MAKEFOCUS			7
#define HOOK_CANVAS_KEYDOWN				8
#define HOOK_CANVAS_KEYUP				9
#define HOOK_CANVAS_MESSAGE				10
#define HOOK_CANVAS_MOUSEDOWN			11
#define HOOK_CANVAS_MOUSEUP				12		// not yet implemented by Be
#define HOOK_CANVAS_PULSE				13

#define HOOKS_CANVAS 14

class SQI_Canvas : public BView , public SQI_Widget
{
	public:
		SQI_Canvas(SQI_Interp *interp,SQI_BCanvas *ptr,BRect frame,uint32 flags);
		~SQI_Canvas();
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
		// drawing methods
		void Clear();
		BBitmap *Shot();
		// Cloning methods
		void Clone(SQI_Canvas *clone);
		int width;
		int height;
		rgb_color bg;								// background color
		BBitmap *Bitmap;							// background bitmap
	private:
		BRect RealBounds;
		SQI_BCanvas *MagicPtr;	// Pointer to the SQI_BCanvas
};
