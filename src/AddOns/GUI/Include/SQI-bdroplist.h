/*
 * Squirrel project
 *
 * file ?	SQI-bdroplist.h
 * what	?   bdroplist object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	08/29/02
 *
 *
 * (c) Kirilla 1999-2002
 */
 
#include <MenuField.h>
  
#define SQI_BDROPLIST	22

class SQI_BDropList;	// forwar def
class SQI_DropList;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BDropList : public SQI_BWidget
{
	public:
		SQI_BDropList(SQI_Squirrel *squirrel,const char *label,SQI_Keyword *var,SQI_List *items,int width,int height,uint32 flags);
		SQI_BDropList(SQI_Heap *target,const char *label,SQI_Keyword *var,SQI_List *items,BRect frame,uint32 flags);
		~SQI_BDropList();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		// none
		// Config Settings methods
		SQI_Object *Label(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Value(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Var(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *List(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		void ComputeSize(float *width,float *height);
		// public variable
		SQI_Keyword *Variable;
		SQI_List *Items;
	private: 
		SQI_DropList *droplist;
		
	friend void DropListVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);			
};

// functions

void InitBDropList();
void UnInitBDropList();

void DropListVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

SQI_BDropList *IsDropList(SQI_Object *object);

void BuildMenuFromList(BPopUpMenu *menu,SQI_List *items,SQI_Object *init);
