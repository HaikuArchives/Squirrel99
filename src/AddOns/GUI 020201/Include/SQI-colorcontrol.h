/*
 * Squirrel project
 *
 * file ?	SQI-colorcontrol.h
 * what	?   colorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <ColorControl.h>
#include "Hook.h"

#define WAS_INVOKED 'wiv'

/* hooks */

#define HOOK_COLORCONTROL_WINDOWACTIVATED		0
#define HOOK_COLORCONTROL_ATTACHEDTOWINDOW    	1
#define HOOK_COLORCONTROL_DETACHEDFROMWINDOW	2
#define HOOK_COLORCONTROL_DRAW					3
#define HOOK_COLORCONTROL_DROP					4
#define HOOK_COLORCONTROL_ENTERED				5
#define HOOK_COLORCONTROL_EXITED				6
#define HOOK_COLORCONTROL_MAKEFOCUS				7
#define HOOK_COLORCONTROL_INVOKED				8
#define HOOK_COLORCONTROL_KEYDOWN				9
#define HOOK_COLORCONTROL_KEYUP					10
#define HOOK_COLORCONTROL_MESSAGE				11
#define HOOK_COLORCONTROL_MOUSEDOWN				12
#define HOOK_COLORCONTROL_MOUSEUP				13		// not yet implemented by Be
#define HOOK_COLORCONTROL_PULSE					14

#define HOOKS_COLORCONTROL 15

class SQI_ColorControl : public BColorControl , public SQI_Widget
{
	public:
		SQI_ColorControl(SQI_Interp *interp,SQI_BColorControl *ptr,color_control_layout matrix,float cellSide,BMessage *message,BPoint lefttop);
		~SQI_ColorControl();
		// Hook
		void Invoked();
		void AttachedToWindow(void);
		void AllAttached(void);
		void AllDetached(void);
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
		void ResizeToPreferred();
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
