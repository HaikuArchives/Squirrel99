/*
 * Squirrel project
 *
 * file ?	SQI-widget.h
 * what	?   widget object
 * who	?	jlv
 * when	?	11/24/99
 * last	?	12/06/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "Hook.h"

// the structure TGlue define the basic element of a Gluing
// primitive

class SQI_BWidget; // forward def

class SQI_Widget
{
	public:
		SQI_Widget(SQI_BWidget *widget,int hooks);
		~SQI_Widget();
		// Methods
		bool Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra);
		bool UnHook(SQI_Keyword *event);
		SQI_Object *UseHook(SQI_Node *node,list<SQI_Object *> *args);
		void Clone(SQI_Widget *clone);
		// some variable
		char valign;
		char halign;
		float padx;
		float pady;
		bool expandx;
		bool expandy;
		// some flags
		bool font_set;
		bool bgcolor_set;
		bool lcolor_set;
		bool hcolor_set;
		SQI_Interp *Interpreter;
		SQI_BWidget *BWidget;	// pointer to the bwidget object 	
	protected:
		SQI_Squirrel *Squirrel; // Squirrel of the window to use with the hook
		int Size;				// number of hook possible	
		THook *Hooks; 			// Table of hooks
		string *SHooks;			// Table of hook string
};
