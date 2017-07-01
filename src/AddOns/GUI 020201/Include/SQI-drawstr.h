/*
 * Squirrel project
 *
 * file ?	SQI-drawstr.h
 * what	?   String object
 * who	?	jlv
 * when	?	03/21/00
 * last	?	03/21/00
 *
 *
 * (c) electron Technology 2000
 */
  
#include "Hook.h"
#include "SQI-drawing.h"

#define DRAW_STR 2

class DrawString : public SQI_Drawing
{
	public:
		DrawString(SQI_BDrawing *widget,BRect frame,char *str);
		// virtual methods
		virtual void Draw(BRect updateRect);
		// virtual other methods
		SQI_Drawing *Clone(SQI_BDrawing *draw);
		string *Print(int prec = 3);
		string *Dump(long ID,long REF,int status);
	private:
		BRect rect;
};

void InitDrawRect();
