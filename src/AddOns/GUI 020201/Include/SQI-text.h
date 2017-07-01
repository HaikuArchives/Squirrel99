/*
 * Squirrel project
 *
 * file ?	SQI-text.h
 * what	?   text object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <StringView.h>
#include "Hook.h"

/* hooks */

#define HOOK_TEXT_WINDOWACTIVATED		0
#define HOOK_TEXT_ATTACHEDTOWINDOW		1
#define HOOK_TEXT_DETACHEDFROMWINDOW	2
#define HOOK_TEXT_DRAW					3
#define HOOK_TEXT_DROP					4
#define HOOK_TEXT_ENTERED				5
#define HOOK_TEXT_EXITED				6
#define HOOK_TEXT_MAKEFOCUS				7
#define HOOK_TEXT_KEYDOWN				8
#define HOOK_TEXT_KEYUP					9
#define HOOK_TEXT_MESSAGE				10
#define HOOK_TEXT_MOUSEDOWN				11
#define HOOK_TEXT_MOUSEUP				12		// not yet implemented by Be
#define HOOK_TEXT_PULSE					13

#define HOOKS_TEXT 14

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
