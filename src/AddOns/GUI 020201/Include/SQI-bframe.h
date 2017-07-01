/*
 * Squirrel project
 *
 * file ?	SQI-bframe.h
 * what	?   bframe object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	12/07/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <View.h> 
  
#define SQI_BFRAME	12

class SQI_BFrame;	// forwar def
class SQI_Frame;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BFrame::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BFrameMembers;
  
class SQI_BFrame : public SQI_BWidget
{
	public:
		SQI_BFrame(SQI_Squirrel *squirrel,char r,int width,int height,uint32 resizingMode,uint32 flags);
		SQI_BFrame(SQI_Heap *target,char r,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_BFrame();	
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
		SQI_Object *Relief(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs);  		
		// Config Settings methods
		SQI_Object *Align(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Level(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		// Glue methods (Adding View)
		void Glue(char align,list<SQI_BWidget *> *widgets);
		void UnGlue(SQI_BWidget *widget);
		void Add2Contents(char align,list<SQI_BWidget *> *widgets);	
		// public variable
		SQI_Frame *View;
	private: 
		BFrameMembers *Members;
		BFrameMembers *Settings;				
};

// functions

void InitBFrame();
SQI_BFrame *IsFrame(SQI_Object *object);
