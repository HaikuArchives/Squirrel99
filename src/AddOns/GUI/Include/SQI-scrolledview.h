/*
 * Squirrel project
 *
 * file ?	SQI-scrolledview.h
 * what	?   scrollviewed object
 * who	?	jlv
 * when	?	03/13/01
 * last	?	03/13/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <ScrollView.h>

#ifndef _SCROLLEDVIEW
#define _SCROLLEDVIEW

class SQI_Widget;
class SQI_BWidget;

class ScrolledView : public BScrollView , public SQI_Widget
{
	public:
		ScrolledView(SQI_BWidget *ptr,BView *t);
		~ScrolledView();
		// Hook
		void AttachedToWindow(void);
		void AllAttached(void);
		void GetPreferredSize(float *width,float *height);
		void FrameResized(float width,float height);
	public:
		float o_w;
		float o_h;	
};

#endif
