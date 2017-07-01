/*
 * Squirrel project
 *
 * file ?	SQI-checkbox.h
 * what	?   checkbox object
 * who	?	jlv
 * when	?	12/02/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <CheckBox.h>
#include "Hook.h"

#define WAS_INVOKED 'wiv'

/* hooks */

#define HOOK_CHECKBOX_WINDOWACTIVATED		0
#define HOOK_CHECKBOX_ATTACHEDTOWINDOW    	1
#define HOOK_CHECKBOX_DETACHEDFROMWINDOW	2
#define HOOK_CHECKBOX_DRAW					3
#define HOOK_CHECKBOX_DROP					4
#define HOOK_CHECKBOX_ENTERED				5
#define HOOK_CHECKBOX_EXITED				6
#define HOOK_CHECKBOX_MAKEFOCUS				7
#define HOOK_CHECKBOX_INVOKED				8
#define HOOK_CHECKBOX_KEYDOWN				9
#define HOOK_CHECKBOX_KEYUP					10
#define HOOK_CHECKBOX_MESSAGE				11
#define HOOK_CHECKBOX_MOUSEDOWN				12
#define HOOK_CHECKBOX_MOUSEUP				13		// not yet implemented by Be
#define HOOK_CHECKBOX_PULSE					14

#define HOOKS_CHECKBOX 15

class SQI_CheckBox : public BCheckBox , public SQI_Widget
{
	public:
		SQI_CheckBox(SQI_Interp *interp,SQI_BCheckBox *ptr,const char *label,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_CheckBox();
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
		void Clone(SQI_CheckBox *clone);
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BCheckBox *MagicPtr;	// Pointer to the SQI_BView
};
