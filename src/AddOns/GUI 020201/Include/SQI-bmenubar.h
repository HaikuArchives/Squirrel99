/*
 * Squirrel project
 *
 * file ?	SQI-bmenubar.h
 * what	?   bmenubar object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	12/08/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include <MenuBar.h> 
  
#define SQI_BMENUBAR	20  

class SQI_BMenuBar;	// forwar def
class SQI_MenuBar;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BMenuBar::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BMenuBarMembers;
  
class SQI_BMenuBar : public SQI_BWidget
{
	public:
		SQI_BMenuBar(SQI_Squirrel *squirrel,menu_layout layout);
		SQI_BMenuBar(SQI_Heap *target,BRect frame,menu_layout layout);
		~SQI_BMenuBar();	
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
		SQI_Object *Add(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Enable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Find(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Remove(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		SQI_MenuBar *View;
		//list<TGlue *> *Contents; // list of gluing sequence	
	private: 
		menu_layout Layout;
		BMenuBarMembers *Members;
		BMenuBarMembers *Settings;				
};

// functions

void InitBMenuBar();

SQI_BMenuBar *IsMenuBar(SQI_Object *object);
