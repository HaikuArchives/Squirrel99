/*
 * Squirrel project
 *
 * file ?	SQI-view.h
 * what	?   view object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	11/16/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <MenuBar.h>
#include "Hook.h"

/* hooks */

#define HOOK_MENUBAR_WINDOWACTIVATED		0
#define HOOK_MENUBAR_ATTACHEDTOWINDOW		1
#define HOOK_MENUBAR_DETACHEDFROMWINDOW		2
#define HOOK_MENUBAR_DRAW					3
#define HOOK_MENUBAR_DROP					4
#define HOOK_MENUBAR_ENTERED				5
#define HOOK_MENUBAR_EXITED					6
#define HOOK_MENUBAR_MAKEFOCUS				7
#define HOOK_MENUBAR_KEYDOWN				8
#define HOOK_MENUBAR_KEYUP					9
#define HOOK_MENUBAR_MESSAGE				10
#define HOOK_MENUBAR_MOUSEDOWN				11
#define HOOK_MENUBAR_MOUSEUP				12		// not yet implemented by Be
#define HOOK_MENUBAR_PULSE					13

#define HOOKS_MENUBAR 14

class SQI_MenuBar : public BMenuBar , public SQI_Widget
{
	public:
		SQI_MenuBar(SQI_Interp *interp,SQI_BMenuBar *ptr,BRect frame,menu_layout layout);
		~SQI_MenuBar();
		// Hook
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
		// Cloning methods
		void Clone(SQI_MenuBar *clone);
	private:
		BRect RealBounds;
		SQI_BMenuBar *MagicPtr;	// Pointer to the SQI_BMenuBar
};
