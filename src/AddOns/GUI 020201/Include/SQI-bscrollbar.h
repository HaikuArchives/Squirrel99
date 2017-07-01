/*
 * Squirrel project
 *
 * file ?	SQI-bscrollbar.h
 * what	?   bscrollbar object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	12/06/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <ScrollBar.h> 
  
#define SQI_BSCROLLBAR	19

class SQI_BScrollBar;	// forwar def
class SQI_ScrollBar;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BScrollBar::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BScrollBarMembers;
  
class SQI_BScrollBar : public SQI_BWidget
{
	public:
		SQI_BScrollBar(SQI_Squirrel *squirrel,SQI_BWidget *target,int width,int height,orientation posture);
		SQI_BScrollBar(SQI_Heap *target,SQI_BWidget *cible,BRect frame,orientation posture);
		~SQI_BScrollBar();	
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
		// Member methods
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *TargetOf(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		void Invalidate();
		// public variable
		SQI_ScrollBar *View;
		SQI_BWidget *Target;	
	private: 
		BScrollBarMembers *Members;
		BScrollBarMembers *Settings;				
};

// functions

void InitBScrollBar();

SQI_BScrollBar *IsScrollBar(SQI_Object *object);
