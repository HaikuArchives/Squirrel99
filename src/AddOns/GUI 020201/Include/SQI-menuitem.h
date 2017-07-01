/*
 * Squirrel project
 *
 * file ?	SQI-menuitem.h
 * what	?  	menuitem object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	12/08/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Menu.h>
#include <MenuItem.h>
#include "Hook.h"

class SQI_MenuItem : public BMenuItem
{
	public:
		SQI_MenuItem(THook *hook,const char *label,BMessage *message,char shortcut = NULL, uint32 modifiers = NULL);
		SQI_MenuItem(THook *hook,BMenu *submenu,BMessage *message = NULL);	
		~SQI_MenuItem();
	private:
		THook *callback;
};
