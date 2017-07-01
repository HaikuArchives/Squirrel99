/*
 * Squirrel project
 *
 * file ?	SQI-drawimg.cpp
 * what	?   Image object
 * who	?	jlv
 * when	?	01/29/01
 * last	?	02/01/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-drawing.h"
#include "SQI-drawimg.h"
  
/*
 * function    : DrawImg
 * purpose     : Constructor
 * input       : 
 *
 * int hooks, number of hooks for this widget
 *
 * output      : none
 * side effect : none
 */
DrawImg::DrawImg(SQI_BDrawing *widget,BRect frame,SQI_Image *img)
	:SQI_Drawing(widget,frame,0)
{	
	image = img;
	image->AddRef();
	image->Contained();	
}

/*
 * function    : ~DrawImg
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : RemRef the image
 */
DrawImg::~DrawImg()
{
	image->IsAlone();
	image->RemRef();			
}

// virtual methods

/*
 * function    : Clone 
 * purpose     : Clone the drawing
 * input       : 
 *
 * SQI_BDrawing *draw, draw object
 *
 * output      : SQI_Drawing *, the clone
 * side effect : none
 */
SQI_Drawing *DrawImg::Clone(SQI_BDrawing *draw)
{
	SQI_Drawing *clone = new DrawImg(draw,Frame(),image);	
	
	CloneHook(clone);
	
	return clone;
}

/*
 * function    : Print
 * purpose     : Print the drawing
 * input       : 
 *
 * int prec, float precision
 *
 * output      : string *, the output
 * side effect : none
 */
string *DrawImg::Print(int prec = 3)
{
  	return new string("");	
}

/*
 * function    : Dump
 * purpose     : Dump the drawing
 * input       : none
 * output      : string *, the output
 * side effect : none
 */
string *DrawImg::Dump(long ID,long REF,int status)
{
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "DRWIMG[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;	
}

void DrawImg::BuildPicture()
{
	if(!picture)
		BeginPicture(new BPicture);
	else
		BeginPicture(picture);
		
	DrawBitmap(image->GetBitmap());
	picture = EndPicture();
}
