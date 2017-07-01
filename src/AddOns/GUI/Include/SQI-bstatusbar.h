/*
 * Squirrel project
 *
 * file ?	SQI-bstatusbar.h
 * what	?   bstatusbar object
 * who	?	jlv
 * when	?	09/19/00
 * last	?	02/05/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include <StatusBar.h> 
  
#define SQI_BSTATUSBAR	34  

class SQI_BStatusBar;	// forwar def
class SQI_StatusBar;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BStatusBar : public SQI_BWidget
{
	public:
		SQI_BStatusBar(SQI_Squirrel *squirrel,const char *label,const char *trailing,int twidth,int trwidth,float max);
		SQI_BStatusBar(SQI_Heap *target,const char *label,const char *trailing,int twidth,int trwidth,float max);
		~SQI_BStatusBar();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Update(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Reset(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		// Config Settings methods
		SQI_Object *BarColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Text(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Trailing(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Value(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Max(SQI_Squirrel *squirrel,SQI_Args *inputs);
	private: 
		SQI_StatusBar *sbar;				
};

// functions

void InitBStatusBar();
void UnInitBStatusBar();
SQI_BStatusBar *IsStatusBar(SQI_Object *object);
