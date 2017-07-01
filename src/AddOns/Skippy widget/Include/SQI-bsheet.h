/*
 * Squirrel project
 *
 * file ?	SQI-bsheet.h
 * what	?   bsheet object
 * who	?	jlv
 * when	?	02/18/00
 * last	?	02/07/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include <View.h> 
#include "Skippy.h"
#include "SQI-playground.h"

#define SQI_BSHEET	28 

class SQI_BSheet;	// forwar def
class SQI_Sheet;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
 
class SQI_BSheet : public SQI_BWidget , public SQI_Playground
{
	public:
		SQI_BSheet(SQI_Squirrel *squirrel,int width,int height,uint32 flags);
		SQI_BSheet(SQI_Heap *target,BRect frame,uint32 flags);
		~SQI_BSheet();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Clear(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Save(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *LSkippies(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Origin(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget
		void SetEnabled(bool state);
		// methods skippy stuff	
		BView *GetView();
		void Register(Skippy *skippy);
		void Unregister(Skippy *skippy);
		void SetHome(BPoint pos);
		void ShowThem();
		void RefreshThem(BRect update);
		void ShowSkippy(bool invalidate,BRect rect,BPoint a,BPoint b,BPoint c);
		// lock view
		void Reserve();
		void Terminate(BPoint from,BPoint to,int pen);
		// public variable
	private: 
		SQI_Sheet *asheet;				
};

// functions

void InitBSheet();
void UnInitBSheet();

SQI_BSheet *IsSheet(SQI_Object *object);
