/*
 * Squirrel project
 *
 * file ?	Plot2D.h
 * what	?   Plot2D object
 * who	?	jlv
 * when	?	11/03/99
 * last	?	11/03/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Locker.h>
#include "Plot2DWin.h"

#define SQI_PLOT2D		8

class Plot2D : public BLocker
{
	public:
		Plot2D(int width,int height);
		~Plot2D();	
	private:
		Plot2DWin *window;
};

// functions

SQI_ThePointer *NewPlot2D(SQI_Squirrel *squirrel,int width,int height);

void InitPlot2D();
