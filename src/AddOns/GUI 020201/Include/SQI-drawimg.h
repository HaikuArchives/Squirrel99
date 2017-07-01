/*
 * Squirrel project
 *
 * file ?	SQI-drawimg.h
 * what	?   Image object
 * who	?	jlv
 * when	?	01/29/01
 * last	?	02/01/01
 *
 *
 * (c) Kirilla 2001
 */
  
#include "Hook.h"

class SQI_Image; // forward def

#define DRAW_IMAGE 2

class DrawImg : public SQI_Drawing
{
	public:
		DrawImg(SQI_BDrawing *widget,BRect frame,SQI_Image *img);
		~DrawImg();
		// virtual methods
		// virtual other methods
		SQI_Drawing *Clone(SQI_BDrawing *draw);
		string *Print(int prec = 3);
		string *Dump(long ID,long REF,int status);
		void BuildPicture();
	private:
		BRect rect;
		SQI_Image *image;
};

void InitDrawImage();
