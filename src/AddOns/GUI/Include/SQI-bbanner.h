/*
 * Squirrel project
 *
 * file ?	SQI-bbanner.h
 * what	?   bbanner object
 * who	?	jlv
 * when	?	12/09/99
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <StringView.h> 
  
#define SQI_BBANNER	25

class SQI_BBanner;	// forwar def
class SQI_Banner;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def
  
class SQI_BBanner : public SQI_BWidget
{
	public:
		SQI_BBanner(SQI_Squirrel *squirrel,SQI_Keyword *var,int width,int height,alignment align,uint32 flags);
		SQI_BBanner(SQI_Squirrel *squirrel,SQI_Heap *target,BRect frame,SQI_Keyword *var,alignment align,uint32 flags);
		~SQI_BBanner();	
		// Member methods	
		// legacy member from SQI_ThePointer
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Text(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Justify(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Var(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// public variable
		SQI_Keyword *Variable;
	private: 
		SQI_Banner *banner;	
		
	friend void BannerVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);				
};

// functions

void InitBBanner();
void UnInitBBanner();

SQI_BBanner *IsBanner(SQI_Object *object);

void BannerVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel);
