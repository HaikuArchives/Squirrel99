/*
 * Squirrel project
 *
 * file ?	2DPlot.cpp
 * what	?   Plot2D object
 * who	?	jlv
 * when	?	11/03/99
 * last	?	11/03/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"  
#include "2DPlot.h"

MemberMap Plot2DMember;

/*
 * function    : Plot2D
 * purpose     : Constructor
 * input       : 
 *
 * int with, width of the view
 * int height, height of the view
 *
 * output      : none
 * side effect : none
 */
Plot2D::Plot2D(int width,int height)
{
	BRect frame(100,100,100+width,100+height);
	window = new Plot2DWin(frame,"test");
	window->Show();
	window->plot2d = this;	
}

/*
 * function    : Plot2D
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
Plot2D::~Plot2D()
{
	if(window)
		window->Quit();
}

/*
 * function    : DeletePlot2D
 * purpose     : Hook for when the object is destroyed
 * input       : 
 *
 * SQI_ThePointer *obj, object pointer
 * void *ptr, pointer to the data
 *
 * output      : none
 * side effect : none
 */
void DeletePlot2D(SQI_ThePointer *obj,void *ptr)
{
	if(ptr)
	{
		Plot2D *This = static_cast<Plot2D *>(ptr);		
		delete This;		
	}	
}


SQI_ThePointer *NewPlot2D(SQI_Squirrel *squirrel,int width,int height)
{
	Plot2D *plot = new Plot2D(width,height);	
		
	SQI_ThePointer *ptr = squirrel->LocalHeap->ThePointer(plot,SQI_PLOT2D);
	
	//ptr->PrintHook = PrintArray;
	//ptr->DumpHook = DumpArray;
	ptr->DeleteHook = DeletePlot2D;
	
	ptr->SetMembers(&Plot2DMember);	
	
	return ptr;	
}

// Fill up the member map
void InitPlot2D()
{
	/*
	ArrayObjectMember[string("get")] = ArrayGetHook;
	ArrayObjectMember[string("set")] = ArraySetHook;
	ArrayObjectMember[string("size")] = ArraySizeHook;
	*/
}
