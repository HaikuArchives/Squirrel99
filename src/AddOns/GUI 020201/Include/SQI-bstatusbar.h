/*
 * Squirrel project
 *
 * file ?	SQI-bstatusbar.h
 * what	?   bstatusbar object
 * who	?	jlv
 * when	?	09/19/00
 * last	?	09/19/00
 *
 *
 * (c) electron Technology 2000
 */
 
#include <StatusBar.h> 
  
#define SQI_BSTATUSBAR	34  

class SQI_BStatusBar;	// forwar def
class SQI_StatusBar;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BStatusBar::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BStatusBarMembers;
  
class SQI_BStatusBar : public SQI_BWidget
{
	public:
		SQI_BStatusBar(SQI_Squirrel *squirrel,const char *label,const char *trailing,int twidth,int trwidth,float max);
		SQI_BStatusBar(SQI_Heap *target,const char *label,const char *trailing,int twidth,int trwidth,float max);
		~SQI_BStatusBar();	
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
		SQI_Object *Update(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Reset(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		// Config Settings methods
		SQI_Object *Align(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *BarColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Pad(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *PadX(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *PadY(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Expand(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Font(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Text(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Trailing(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Value(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Max(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		// public variable
		SQI_StatusBar *View;
	private: 
		BStatusBarMembers *Members;
		BStatusBarMembers *Settings;				
};

// functions

void InitBStatusBar();
SQI_BStatusBar *IsStatusBar(SQI_Object *object);
