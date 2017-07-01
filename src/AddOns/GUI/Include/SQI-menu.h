/*
 * Squirrel project
 *
 * file ?	SQI-menu.h
 * what	?   menu object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	02/16/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Menu.h>

class SQI_Menu : public BMenu , public SQI_Widget
{
	public:
		SQI_Menu(SQI_Interp *interp,SQI_BMenu *ptr,const char *label);
		~SQI_Menu();
		void DelCallBack();
		// Hook
		void AttachedToWindow(void);
		// Cloning methods
		void Clone(SQI_Menu *clone);
		SQI_BMenu *MagicPtr;	// Pointer to the SQI_BMenu
};
