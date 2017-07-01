/*
 * Squirrel project
 *
 * file ?	SQI-playground.h
 * what	?   Skippy playground
 * who	?	jlv
 * when	?	01/24/01
 * last	?	01/24/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <list> 

class Skippy; // forward def
 
class SQI_Playground 
{
	public:
		SQI_Playground();
		~SQI_Playground();
		
		SQI_Object *ListSki(SQI_Squirrel *squirrel);
		virtual BView *GetView() {return NULL;};	
		
		// methods skippy stuff	
		virtual void Register(Skippy *skippy);
		virtual void Unregister(Skippy *skippy);
		virtual void SetHome(BPoint pos);
		virtual void ShowThem() {};
		virtual void RefreshThem(BRect update) {};
		virtual void ShowSkippy(bool invalidate,BRect rect,BPoint a,BPoint b,BPoint c) {};
		// lock view
		virtual void Reserve();
		virtual void Terminate(BPoint from,BPoint to,int pen);	
	protected:		
		list<Skippy *> Skippies;
}; 
