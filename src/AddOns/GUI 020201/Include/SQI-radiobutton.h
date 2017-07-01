/*
 * Squirrel project
 *
 * file ?	SQI-radiobutton.h
 * what	?   radiobutton object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <RadioButton.h>
#include "Hook.h"

#define WAS_INVOKED 'wiv'

/* hooks */

#define HOOK_RADIOBUTTON_WINDOWACTIVATED		0
#define HOOK_RADIOBUTTON_ATTACHEDTOWINDOW    	1
#define HOOK_RADIOBUTTON_DETACHEDFROMWINDOW		2
#define HOOK_RADIOBUTTON_DRAW					3
#define HOOK_RADIOBUTTON_DROP					4
#define HOOK_RADIOBUTTON_ENTERED				5
#define HOOK_RADIOBUTTON_EXITED					6
#define HOOK_RADIOBUTTON_MAKEFOCUS				7
#define HOOK_RADIOBUTTON_INVOKED				8
#define HOOK_RADIOBUTTON_KEYDOWN				9
#define HOOK_RADIOBUTTON_KEYUP					10
#define HOOK_RADIOBUTTON_MESSAGE				11
#define HOOK_RADIOBUTTON_MOUSEDOWN				12
#define HOOK_RADIOBUTTON_MOUSEUP				13		// not yet implemented by Be
#define HOOK_RADIOBUTTON_PULSE					14

#define HOOKS_RADIOBUTTON 15

class SQI_RadioButton : public BRadioButton , public SQI_Widget
{
	public:
		SQI_RadioButton(SQI_Interp *interp,SQI_BRadioButton *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_RadioButton();
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
		void SetValue(int32 value);	
		// Cloning methods
		void Clone(SQI_RadioButton *clone);
		int char_width;
		int char_height;
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BRadioButton *MagicPtr;	// Pointer to the SQI_BView
};
