/*
 * Squirrel project
 *
 * file ?	SQI-frame.h
 * what	?   frame object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	11/15/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#include <View.h>
#include "Hook.h"

/* hooks */

#define HOOK_FRAME_WINDOWACTIVATED		0
#define HOOK_FRAME_ATTACHEDTOWINDOW		1
#define HOOK_FRAME_DETACHEDFROMWINDOW	2
#define HOOK_FRAME_DRAW					3
#define HOOK_FRAME_DROP					4
#define HOOK_FRAME_ENTERED				5
#define HOOK_FRAME_EXITED				6
#define HOOK_FRAME_MAKEFOCUS			7
#define HOOK_FRAME_KEYDOWN				8
#define HOOK_FRAME_KEYUP				9
#define HOOK_FRAME_MESSAGE				10
#define HOOK_FRAME_MOUSEDOWN			11
#define HOOK_FRAME_MOUSEUP				12		// not yet implemented by Be
#define HOOK_FRAME_FRAMEMOVED			13
#define HOOK_FRAME_PULSE				14
#define HOOK_FRAME_FRAMERESIZED			15

#define HOOKS_FRAME 16

#define RELIEF_LOWERED	0
#define RELIEF_RAISED	1
#define RELIEF_FLATENED	2	
#define RELIEF_BORDERED	3

class SQI_Frame : public BView , public SQI_Widget
{
	public:
		SQI_Frame(SQI_Interp *interp,SQI_BFrame *ptr,char r,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_Frame();
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
		void ResizeToPreferred();
		void GetPreferredSize(float *width,float *height);	
		// Gluing methods
		void Adapt();
		void ReGluing();
		void DynamicGluing();
		void StaticGluing();
		void ExpandChilds();
		float border;			// border thickness
		char relief;				// bevel style
		int level;				// bebel level
		// Cloning methods
		void Clone(SQI_Frame *clone);
		// drawing methods
		void DrawRelief();
		void ColoringBasis(rgb_color basis_color);
	private:
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BFrame *MagicPtr;	// Pointer to the SQI_BView
		rgb_color hi_color,lo_color;
};
