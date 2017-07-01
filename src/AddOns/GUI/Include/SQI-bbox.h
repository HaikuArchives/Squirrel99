/*
 * Squirrel project
 *
 * file ?	SQI-bbox.h
 * what	?   bbox object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	02/09/00
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <View.h> 
  
#define SQI_BBOX	13

class SQI_BBox;	// forwar def
class SQI_Box;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
 
class SQI_BBox : public SQI_BWidget
{
	public:
		SQI_BBox(SQI_Squirrel *squirrel,const char *label,int width,int height,uint32 resizingMode,uint32 flags,border_style border);
		SQI_BBox(SQI_Heap *target,const char *label,BRect frame,uint32 resizingMode,uint32 flags,border_style border);
		SQI_BBox(SQI_Squirrel *squirrel,SQI_BWidget *label,int width,int height,uint32 resizingMode,uint32 flags,border_style border);
		SQI_BBox(SQI_Heap *target,SQI_BWidget *label,BRect frame,uint32 resizingMode,uint32 flags,border_style border);
		~SQI_BBox();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Style(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs);  		
		// Config Settings methods
		SQI_Object *Border(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *LLabel(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		void Expand(float left,float top,float right,float bottom);
		// public variable
		SQI_BWidget *Label;
		//SQI_BWindow *Window;	 // window to which the view is attached (NULL if not attached)
		//list<TGlue *> *Contents; // list of gluing sequence	
	private: 
		SQI_Box *box;				
};

// functions

void InitBBox();
void UnInitBBox();
SQI_BBox *IsBox(SQI_Object *object);
