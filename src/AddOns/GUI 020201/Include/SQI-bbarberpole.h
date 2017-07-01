/*
 * Squirrel project
 *
 * file ?	SQI-bbarberpole.h
 * what	?   barberpole object
 * who	?	jlv
 * when	?	10/14/00
 * last	?	10/14/00
 *
 *
 * (c) electron Technology 2000
 */
 
#include <list>

const int FROM_RIGHT_TO_LEFT = 0;
const int FROM_LEFT_TO_RIGHT = 1;
  
#define SQI_BBARBERPOLE	33 

class SQI_BBarberPole;	// forwar def
class SQI_BarberPole;	// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BBarberPole::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BBarberPoleMembers;
  
class SQI_BBarberPole : public SQI_BWidget
{
	public:
		SQI_BBarberPole(SQI_Squirrel *squirrel,int width,int height,int pDirection,uint32 flags);
		SQI_BBarberPole(SQI_Heap *target,int width,int height,int pDirection,uint32 flags);
		~SQI_BBarberPole();	
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
		SQI_Object *Start(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Stop(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Align(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		SQI_BarberPole *View;
	private: 
		BBarberPoleMembers *Members;
		BBarberPoleMembers *Settings;				
};

// functions

void InitBBarberPole();

SQI_BBarberPole *IsBarberPole(SQI_Object *object);
