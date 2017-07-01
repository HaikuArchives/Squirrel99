/*
 * Squirrel project
 *
 * file ?	SQI-bbutton.h
 * what	?   bbutton object
 * who	?	jlv
 * when	?	12/01/99
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Button.h> 
  
#define SQI_BBUTTON	14  

class SQI_BButton;	// forwar def
class SQI_Button;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BButton : public SQI_BWidget
{
	public:
		SQI_BButton(SQI_Squirrel *squirrel,const char *label,int width,int height,uint32 resizingMode,uint32 flags);
		SQI_BButton(SQI_Heap *target,const char *label,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_BButton();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *IsDefault(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MakeDefault(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs); 
		// Config Settings methods
		SQI_Object *Label(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		void ComputeSize(float *width,float *height);
		void SetEnabled(bool state);
	private:
		SQI_Button *button;				
};

// functions

void InitBButton();
void UnInitBButton();
SQI_BButton *IsButton(SQI_Object *object);
