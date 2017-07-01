/*
 * Squirrel project
 *
 * file ?	SQI-bbanner.h
 * what	?   bbanner object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	12/09/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <StringView.h> 
  
#define SQI_BBANNER	25

class SQI_BBanner;	// forwar def
class SQI_Banner;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BBanner::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BBannerMembers;
  
class SQI_BBanner : public SQI_BWidget
{
	public:
		SQI_BBanner(SQI_Squirrel *squirrel,SQI_Keyword *var,int width,int height,alignment align,uint32 flags);
		SQI_BBanner(SQI_Heap *target,BRect frame,SQI_Keyword *var,alignment align,uint32 flags);
		~SQI_BBanner();	
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
		SQI_Object *Text(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Justify(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		SQI_Object *Var(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		SQI_Banner *View;
		SQI_Keyword *Variable;
	private: 
		BBannerMembers *Members;
		BBannerMembers *Settings;				
};

// functions

void InitBBanner();

SQI_BBanner *IsBanner(SQI_Object *object);

void BannerVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);
