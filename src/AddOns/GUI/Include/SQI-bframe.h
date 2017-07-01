/*
 * Squirrel project
 *
 * file ?	SQI-bframe.h
 * what	?   bframe object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	03/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <View.h> 
  
#define SQI_BFRAME	12

class SQI_BFrame;	// forwar def
class SQI_Frame;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

#define RELIEF_LOWERED	0
#define RELIEF_RAISED	1
#define RELIEF_FLATENED	2	
#define RELIEF_BORDERED	3
  
class SQI_BFrame : public SQI_BWidget
{
	public:
		SQI_BFrame(SQI_Squirrel *squirrel,char r,int width,int height,uint32 resizingMode,uint32 flags);
		SQI_BFrame(SQI_Heap *target,char r,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_BFrame();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods 
		SQI_Object *Relief(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs); 
		SQI_Object *Level(SQI_Squirrel *squirrel,SQI_Args *inputs); 		
		// legacy from SQI_Widget	
		void Expand(float left,float top,float right,float bottom);
		// public variable
	private: 
		SQI_Frame *frame;				
};

// functions

void InitBFrame();
void UnInitBFrame();
SQI_BFrame *IsFrame(SQI_Object *object);
