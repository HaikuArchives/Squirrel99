/*
 * Squirrel project
 *
 * file ?	SQI-entry.h
 * what	?   entry object
 * who	?	jlv
 * when	?	12/10/99
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <TextControl.h>
#include "Hook.h"

#define WAS_CHANGED 'wch'

/* hooks */

#define HOOK_ENTRY_WINDOWACTIVATED		0
#define HOOK_ENTRY_ATTACHEDTOWINDOW    	1
#define HOOK_ENTRY_CHANGED				2
#define HOOK_ENTRY_DETACHEDFROMWINDOW	3
#define HOOK_ENTRY_DRAW					4
#define HOOK_ENTRY_DROP					5
#define HOOK_ENTRY_ENTERED				6
#define HOOK_ENTRY_EXITED				7
#define HOOK_ENTRY_MAKEFOCUS			8
#define HOOK_ENTRY_KEYDOWN				9
#define HOOK_ENTRY_KEYUP				10
#define HOOK_ENTRY_MESSAGE				11
#define HOOK_ENTRY_MOUSEDOWN			12
#define HOOK_ENTRY_MOUSEUP				13		// not yet implemented by Be
#define HOOK_ENTRY_PULSE				14

#define HOOKS_ENTRY 15

class SQI_Entry : public BTextControl , public SQI_Widget
{
	public:
		SQI_Entry(SQI_Interp *interp,SQI_BEntry *ptr,const char *label,const char *txt,BMessage *message,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_Entry();
		// Hook
		void Invoked(void);
		void AttachedToWindow(void);
		void AllAttached(void);
		void AllDetached(void);
		void DetachedFromWindow(void);
		void Draw(BRect updateRect);
		void FrameResized(float width,float height);
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
		void GetPreferredWidth(float *width);	
		// Cloning methods
		void Clone(SQI_Entry *clone);
		int label_width;
		int entry_width;
	private:
		float pref_divider;
		float pref_width;
		SQI_BEntry *MagicPtr;	// Pointer to the SQI_BView
};
