/*
 * Squirrel project
 *
 * file ?	SQI-btext.h
 * what	?   btext object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <StringView.h> 
  
#define SQI_BTEXT	24 

class SQI_BText;	// forwar def
class SQI_Text;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BText : public SQI_BWidget
{
	public:
		SQI_BText(SQI_Squirrel *squirrel,const char *text,int width,int height,alignment align,uint32 flags);
		SQI_BText(SQI_Heap *target,BRect frame,const char *text,alignment align,uint32 flags);
		~SQI_BText();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
 		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Text(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Justify(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		// none
		// legacy from SQI_Widget
		void SetEnabled(bool state);
		// public variable
	private: 
		SQI_Text *text;			
};

// functions

void InitBText();
void UnInitBText();

SQI_BText *IsText(SQI_Object *object);
