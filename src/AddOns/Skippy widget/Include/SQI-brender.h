/*
 * Squirrel project
 *
 * file ?	SQI-brender.h
 * what	?   Skippy render in a bitmap
 * who	?	jlv
 * when	?	01/24/01
 * last	?	01/24/01
 *
 *
 * (c) Kirilla 2001
 */

#include <Bitmap.h>
#include <View.h>

class SQI_Image; // forward def
 
class SQI_BRender : public SQI_Playground 
{
	public:
		SQI_BRender(SQI_Image *img);
		~SQI_BRender();
		
		BView *GetView();
		void Register(Skippy *skippy);
		void Unregister(Skippy *skippy);
		void SetHome(BPoint pos);
		void Reserve();
		void Terminate(BPoint from,BPoint to,int pen);
		
	private:
		BBitmap *bitmap;
		BView *View;
		SQI_Image *Img;		
};
 
