/*
 * Squirrel project
 *
 * file ?	SQI-memo.h
 * what	?   memo object
 * who	?	jlv
 * when	?	02/20/00
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <TextView.h>
#include "Hook.h"

#define WAS_INVOKED 'wiv'
#define WAS_SELECTED 'wsl'

/* hooks */

#define HOOK_MEMO_WINDOWACTIVATED		0
#define HOOK_MEMO_ATTACHEDTOWINDOW		1
#define HOOK_MEMO_DETACHEDFROMWINDOW	2
#define HOOK_MEMO_DRAW					3
#define HOOK_MEMO_DROP					4
#define HOOK_MEMO_ENTERED				5
#define HOOK_MEMO_EXITED				6
#define HOOK_MEMO_MAKEFOCUS				7
#define HOOK_MEMO_INVOKED				8
#define HOOK_MEMO_KEYDOWN				9
#define HOOK_MEMO_KEYUP					10
#define HOOK_MEMO_MESSAGE				11
#define HOOK_MEMO_MOUSEDOWN				12
#define HOOK_MEMO_MOUSEUP				13		// not yet implemented by Be
#define HOOK_MEMO_FRAMEMOVED			14
#define HOOK_MEMO_PULSE					15
#define HOOK_MEMO_FRAMERESIZED			16

#define HOOKS_MEMO 17

class SQI_Memo : public BTextView , public SQI_Widget
{
	public:
		SQI_Memo(SQI_Interp *interp,SQI_BMemo *ptr,int cw,int l,BRect frame,uint32 flags);
		~SQI_Memo();
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
	private:
		int Chars;		// number of char on each line (if defined by the user)
		int Lines;	// number of visible line (if definied by the user)
		BRect RealBounds;
		float original_width;
		float original_height;
		SQI_BMemo *MagicPtr;	// Pointer to the SQI_BMemo
};
