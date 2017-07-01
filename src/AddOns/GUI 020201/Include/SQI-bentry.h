/*
 * Squirrel project
 *
 * file ?	SQI-bentry.h
 * what	?   bentry object
 * who	?	jlv
 * when	?	12/10/99
 * last	?	09/21/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <TextControl.h> 
  
#define SQI_BENTRY	26

class SQI_BEntry;	// forwar def
class SQI_Entry;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

typedef map<string,SQI_Object *(SQI_BEntry::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BEntryMembers;
  
class SQI_BEntry : public SQI_BWidget
{
	public:
		SQI_BEntry(SQI_Squirrel *squirrel,const char *label,SQI_Keyword *var,int lwidth,int ewidth,uint32 resizingMode,uint32 flags);
		SQI_BEntry(SQI_Heap *target,const char *label,SQI_Keyword *var,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_BEntry();	
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
		SQI_Object *IsEnable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs); 
		SQI_Object *Enable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Label(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Entry(SQI_Squirrel *squirrel,SQI_Args *inputs);		
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
		SQI_Object *Value(SQI_Squirrel *squirrel,SQI_Args *inputs);
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
		SQI_Entry *View;
		SQI_Keyword *Variable;
		bool isNUM;
	private: 
		BEntryMembers *Members;
		BEntryMembers *Settings;				
};

// functions

void InitBEntry();

void EntryVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

SQI_BEntry *IsEntry(SQI_Object *object);
