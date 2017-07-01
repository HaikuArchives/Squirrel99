/*
 * Squirrel project
 *
 * file ?	SQI-bentry.h
 * what	?   bentry object
 * who	?	jlv
 * when	?	12/10/99
 * last	?	02/19/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <TextControl.h> 
  
#define SQI_BENTRY	26

class SQI_BEntry;	// forwar def
class SQI_Entry;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BEntry : public SQI_BWidget
{
	public:
		SQI_BEntry(SQI_Squirrel *squirrel,const char *label,SQI_Keyword *var,int lwidth,int ewidth,uint32 resizingMode,uint32 flags);
		SQI_BEntry(SQI_Heap *target,const char *label,SQI_Keyword *var,BRect frame,uint32 resizingMode,uint32 flags);
		~SQI_BEntry();	
		// Member methods	
		void SetEnabled(bool state);
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs); 
		SQI_Object *Label(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Entry(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		// Config Settings methods
		SQI_Object *Var(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// public variable
		SQI_Keyword *Variable;
		bool isNUM;
	private: 
		SQI_Entry *entry;				

	friend void EntryVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

};

// functions

void InitBEntry();
void UnInitBEntry();

void EntryVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);

SQI_BEntry *IsEntry(SQI_Object *object);
