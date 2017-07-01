/*
 * Squirrel project
 *
 * file ?	Cursor.cpp
 * what	?  	Widget cursors
 * who	?	jlv
 * when	?	02/09/01
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "SQI-AddOn.h"
#include "Cursors.h"
#include "Cursor.h"

SQI_Cursor c_hand(HAND,"hand");
SQI_Cursor c_ibeam(IBEAM,"ibeam");
SQI_Cursor *c_cross = NULL;
SQI_Cursor *c_hcross = NULL;
SQI_Cursor *c_pencil = NULL;
SQI_Cursor *c_cut = NULL;
SQI_Cursor *c_hourglass = NULL;
SQI_Cursor *c_macwatch = NULL;
SQI_Cursor *c_zoom = NULL;
SQI_Cursor *c_timer = NULL;
SQI_Cursor *c_sarrow = NULL;
SQI_Cursor *c_larrow = NULL;
SQI_Cursor *c_rarrow = NULL;
SQI_Cursor *c_uarrow = NULL;
SQI_Cursor *c_darrow = NULL;
SQI_Cursor *c_lhand = NULL;

void FreeCursors()
{
	if(c_hcross)
		delete c_hcross;
	if(c_cross)
		delete c_cross;		
	if(c_pencil)
		delete c_pencil;
	if(c_cut)
		delete c_cut;
	if(c_hourglass)
		delete c_hourglass;							
	if(c_macwatch)
		delete c_macwatch;
	if(c_zoom)
		delete c_zoom;
	if(c_timer)
		delete c_timer;
	if(c_sarrow)
		delete c_sarrow;
	if(c_larrow)
		delete c_larrow;
	if(c_rarrow)
		delete c_rarrow;
	if(c_uarrow)
		delete c_uarrow;
	if(c_darrow)
		delete c_darrow;
	if(c_lhand)
		delete c_lhand;												
}


SQI_Cursor *GetCursor(string *name)
{
	if(*name == "hand")	
		return &c_hand;
	else
	if(*name == "ibeam")
		return &c_ibeam;	
	else
	if(*name == "cross")
	{
		if(!c_cross)
			c_cross = new SQI_Cursor(Cross,"cross");
		return c_cross;
	}	
	else
	if(*name == "hcross")
	{
		if(!c_hcross)
			c_hcross = new SQI_Cursor(HCross,"hcross");
		return c_hcross;
	}	
	else
	if(*name == "pencil")
	{
		if(!c_pencil)
			c_pencil = new SQI_Cursor(Pencil,"pencil");
		return c_pencil;
	}	
	else
	if(*name == "cut")
	{
		if(!c_cut)
			c_cut = new SQI_Cursor(Cut,"cut");
		return c_cut;
	}	
	else
	if(*name == "hourglass")
	{
		if(!c_hourglass)
			c_hourglass = new SQI_Cursor(HourGlass,"hourglass");
		return c_hourglass;
	}	
	else
	if(*name == "macwatch")
	{
		if(!c_macwatch)
			c_macwatch = new SQI_Cursor(MacWatch,"macwatch");
		return c_macwatch;
	}	
	else
	if(*name == "zoom")
	{
		if(!c_zoom)
			c_zoom = new SQI_Cursor(Zoom,"zoom");
		return c_zoom;
	}	
	else
	if(*name == "timer")
	{
		if(!c_timer)
			c_timer = new SQI_Cursor(Timer,"timer");
		return c_timer;
	}	
	else
	if(*name == "arrow")
	{
		if(!c_sarrow)
			c_sarrow = new SQI_Cursor(SimpleArrow,"arrow");
		return c_sarrow;
	}	
	else
	if(*name == "leftarrow")
	{
		if(!c_larrow)
			c_larrow = new SQI_Cursor(LeftArrow,"leftarrow");
		return c_larrow;
	}
	else
	if(*name == "rightarrow")
	{
		if(!c_rarrow)
			c_rarrow = new SQI_Cursor(RightArrow,"rightarrow");
		return c_rarrow;
	}
	else
	if(*name == "uparrow")
	{
		if(!c_uarrow)
			c_uarrow = new SQI_Cursor(UpArrow,"uparrow");
		return c_uarrow;
	}	else
	if(*name == "downarrow")
	{
		if(!c_darrow)
			c_darrow = new SQI_Cursor(DownArrow,"downarrow");
		return c_darrow;
	}	else
	if(*name == "linkhand")
	{
		if(!c_lhand)
			c_lhand = new SQI_Cursor(NetPositive,"linkhand");
		return c_lhand;
	}	
	else
		return &c_hand;							
}

SQI_Cursor::SQI_Cursor(const void *cursorData,const char *n)
	: BCursor(cursorData)
{
	name = n;	
}
