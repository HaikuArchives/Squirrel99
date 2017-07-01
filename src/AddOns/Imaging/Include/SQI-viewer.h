/*
 * Squirrel project
 *
 * file ?	SQI-view.h
 * what	?   view object
 * who	?	jlv
 * when	?	02/06/01
 * last	?	03/15/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <View.h>
#include "Hook.h"

typedef enum DRAW_STYLE {
	ADAPT,
	CENTER,
	SCALE,
	PSCALE,
	SCROLL	
};

class SQI_Viewer : public BView , public SQI_Widget
{
	public:
		SQI_Viewer(SQI_Interp *interp,SQI_BViewer *ptr,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_Viewer();
		// Hook
		void AttachedToWindow(void);
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
		// Cloning methods
		void Clone(SQI_Viewer *clone);
		// usefull functions
		void ComputePos();
		// public variable
		BBitmap *bitmap;	// bitmap to display
		BPoint pos;			// position where to draw
	private:
		bool resized;
		DRAW_STYLE style;
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		float original_width;
		float original_height;
		SQI_BViewer *MagicPtr;	// Pointer to the SQI_BView
		
	friend class SQI_BViewer;	
};
