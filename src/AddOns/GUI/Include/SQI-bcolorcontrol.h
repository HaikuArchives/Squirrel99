/*
 * Squirrel project
 *
 * file ?	SQI-bcolorcontrol.h
 * what	?   bcolorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <ColorControl.h> 
  
#define SQI_BCOLORCONTROL	16

class SQI_BColorControl;	// forwar def
class SQI_ColorControl;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BColorControl : public SQI_BWidget
{
	public:
		SQI_BColorControl(SQI_Squirrel *squirrel,color_control_layout matrix,float cellSide,SQI_Keyword *var);
		SQI_BColorControl(SQI_Heap *target,color_control_layout matrix,float cellSide,SQI_Keyword *var,BPoint lefttop);
		~SQI_BColorControl();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs); 		
		// Config Settings methods
		SQI_Object *CellSize(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Layout(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Value(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Var(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		void ComputeSize(float *width,float *height);
		// public variable
		SQI_Keyword *Variable;
	private: 
		SQI_ColorControl *control;	
		
	friend void ColorControlVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);				
};

// functions

void InitBColorControl();
void UnInitBColorControl();

void ColorControlVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

SQI_BColorControl *IsColorControl(SQI_Object *object);
