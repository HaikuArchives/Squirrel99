/*
 * Squirrel project
 *
 * file ?	SQI-menuitem.cpp
 * what	?  	menuitem object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	02/16/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-menuitem.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bmenu.h"
#include "SQI-menu.h"


SQI_MenuItem::SQI_MenuItem(THook *hook,const char *label,BMessage *message,char shortcut = NULL, uint32 modifiers = NULL)
	:BMenuItem(label,message,shortcut,modifiers)
{
	callback 	= hook;
	have_font 	= false;		
}

SQI_MenuItem::SQI_MenuItem(THook *hook,BMenu *submenu,BMessage *message = NULL)
	:BMenuItem(submenu,message)
{
	callback 	= hook;
	have_font 	= false;
}	

void SQI_MenuItem::DelCallBack()
{
	if(callback)
	{
		callback->action->Alone();
		callback->action->RemRef();
		if(callback->extra)
		{
			list<SQI_Object *>::const_iterator i = callback->extra->begin();
			for(i;i!=callback->extra->end();i++)
				(*i)->RemRef();		
			delete callback->extra;
		}	
		delete callback;
	}	
}

SQI_MenuItem::~SQI_MenuItem()
{	
	/*
	BMenu *sub = Submenu();
	if(sub)
	{
		SQI_Menu *menu = dynamic_cast<SQI_Menu *>(sub);
		menu->MagicPtr->View = NULL;
		menu->MagicPtr->Alone();
		menu->MagicPtr->RemRef();	
	}
	*/	
}

BFont *SQI_MenuItem::GetFont()
{
	if(have_font)
		return &font;
	else
	{
		Menu()->GetFont(&font);
		return &font;
	}
}

void SQI_MenuItem::SetFont(SQI_Font *f)
{
	have_font = true;
	font = *f;
	
	font_height he;
	font.GetHeight(&he);
	
	size = font.Size();
	h 	 = size; //ceil(he.ascent)+ceil(he.descent)+ceil(he.leading);
	w 	 = font.StringWidth(Label());	
}

void SQI_MenuItem::GetContentSize(float* width, float* height)
{
	if (have_font) {
		*height = size;
		*width = w;
	}
	else
		BMenuItem::GetContentSize(width, height);
}

void SQI_MenuItem::DrawContent()
{		
	if (have_font) 
	{						
		BPoint p = Menu()->PenLocation();
		BFont f;
		
		Menu()->GetFont(&f);
		Menu()->SetFont(&font);
		Menu()->MovePenTo(ContentLocation());
		if(size-f.Size() < 0)
			Menu()->MovePenBy(0,size - f.Size() - 2); // smaller
		else
			Menu()->MovePenBy(0,size - f.Size() - 5); // bigger
		BMenuItem::DrawContent();	
		Menu()->SetFont(&f);
	}
	else
		BMenuItem::DrawContent();
}
