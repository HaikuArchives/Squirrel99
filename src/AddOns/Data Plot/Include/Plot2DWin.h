/*
 * Squirrel project
 *
 * file ?	Plot2DWin.h
 * what	?   2D Plot window
 * who	?	jlv
 * when	?	11/03/99
 * last	?	11/03/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Window.h>

class Plot2D;

class Plot2DWin : public BWindow
{
	public:
		Plot2DWin(BRect frame,const char *title);
		void Quit();
		void MessageReceived(BMessage *message);
		Plot2D *plot2d;	
	private:
		
};
