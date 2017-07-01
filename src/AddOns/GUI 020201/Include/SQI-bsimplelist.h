/*
 * Squirrel project
 *
 * file ?	SQI-bsimplelist.h
 * what	?   bsimplelist object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	12/08/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <ListView.h> 
#include <ScrollBar.h>
#include "SQI-container.h"
  
#define SQI_BSIMPLELIST	18

class SQI_BSimpleList;	// forwar def
class SQI_SimpleList;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

#define SCROLL_LEFT		0
#define SCROLL_RIGHT	1
#define SCROLL_NONE		2

typedef map<string,SQI_Object *(SQI_BSimpleList::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BSimpleListMembers;
  
class SQI_BSimpleList : public SQI_BWidget
{
	public:
		SQI_BSimpleList(SQI_Squirrel *squirrel,SQI_List *items,SQI_Keyword *variable,char scroll,int width,int height,list_view_type type,uint32 flags);
		SQI_BSimpleList(SQI_Heap *target,SQI_List *items,SQI_Keyword *variable,char scroll,int width,int height,BRect frame,list_view_type type,uint32 flags);
		~SQI_BSimpleList();	
		// Member methods	
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		void Select(SQI_Object *value);
		// Member methods
		SQI_Object *AddItem(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AddItemAt(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ItemsList(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Enable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Align(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Pad(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *PadX(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *PadY(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Expand(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Font(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		bool Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra);
		bool UnHook(SQI_Keyword *event);
		BView *GiveBView();
		SQI_Widget *GiveWidget();
		BRect TrueBounds();
		BRect TrueFrame();
		void MoveTo(float y,float y);
		void MoveBy(float w,float h);
		double Area();
		void Expand(float left,float top,float right,float bottom);
		void ResizeToPreferred();
		void SetEnabled(bool state);
		// public variable
		Container *View;
		BScrollBar *Scroll;
		SQI_SimpleList *List;
		SQI_List *Items;
		SQI_Keyword *Variable;	
		char scroll_type;	// define the type of the scroll (left, right , none)
	private:
		float value_range;  // used to keep the position of the scrollbar when the widget is disable
		float max_range;	// used to keep the max range of the scrollbar when the widget is disable
		BSimpleListMembers *Members;
		BSimpleListMembers *Settings;				
};

// functions

void InitBSimpleList();

void SimpleListVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

SQI_BSimpleList *IsSimpleList(SQI_Object *object);
