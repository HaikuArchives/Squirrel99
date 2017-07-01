/*
 * Squirrel project
 *
 * file ?	SQI-scrollview.h
 * what	?   scrollview object
 * who	?	jlv
 * when	?	02/12/01
 * last	?	03/12/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <View.h>
#include <ScrollBar.h>

#ifndef _SCROLLVIEW
#define _SCROLLVIEW

class SQI_Widget;
class SQI_BWidget;

class ScrollView : public BView , public SQI_Widget
{
	public:
		ScrollView(SQI_BWidget *ptr,BRect frame,BView *t,bool present);
		~ScrollView();
		// Hook
		void AttachedToWindow(void);
		void AllAttached(void);
		void GetPreferredSize(float *width,float *height);
		void FrameResized(float width,float height);
		// methods
		void Scroll(bool s);
		void Refresh();
		void AdjustTo();
		float 		o_w;
		float 		o_h;		
	private:
		bool		scroll;
		BView		*client;
		BScrollBar 	*right;
		BScrollBar 	*bottom;
		BView 		*target;
};

#endif
