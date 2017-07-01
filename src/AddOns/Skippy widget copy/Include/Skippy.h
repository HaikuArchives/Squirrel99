/*
 * Squirrel project
 *
 * file ?	FileIO.h
 * what	?   File IO functins
 * who	?	jlv
 * when	?	02/18/00
 * last	?	12/10/00
 *
 *
 * (c) electron Technology 1999-2000
 */
 
#ifndef SKIPPY_H
#define SKIPPY_H  
 
#define SQI_SKIPPY		29
#define CLASS_SKIPPY	7

class Skippy; //forward def
class SQI_Playground;
class SQI_Sheet;

typedef map<string,SQI_Object *(Skippy::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > MemberMap;

class Skippy : public SQI_ThePointer
{
	public:
		Skippy(SQI_Squirrel *squirrel,SQI_Playground *sheet);
		Skippy(SQI_Heap *heap,SQI_Playground *sheet);
		~Skippy();			
		// Member methods
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Hop(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *HopBack(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Left(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Right(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Pen(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *GoTo(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		SQI_Object *GoHome(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Heading(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *String(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *CString(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *StringWidth(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Arc(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Ellipse(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Rect(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *RoundRect(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		SQI_Object *Triangle(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Show(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Record(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Play(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Warp(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Draw(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		// configs methods
		SQI_Object *Color(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Size(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Home(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Font(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
		bool Suicidal(bool force=false);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		bool operator==(SQI_Object *op2);
  		bool operator!=(SQI_Object *op2);
  		// various methods
  		void ShowItSelf(bool inv);
  		void Refresh(BRect updateRect);
  		void Unregistered();
  		void Go(BPoint pos); 		// go to a position
  		void GoHome();				// go to the home position
  		void SetHome(BPoint pos);	// set the home position
  		void SetPosition(BPoint pos) {position = pos;};
  		void StepBy(float heading,int distance); // the skippy must step	
	private:
		bool show;				 // true if the skippy must show it self
		bool set_home;			 // if the home have been set
		rgb_color hcolor,lcolor; // color
		float pen;				 // size of the pen
		bool down;				 // true if the pen is down
		BPoint position;
		BPoint home;
		float heading;
		BFont font; 
		BView *target;
		SQI_Playground *sheet;
		MemberMap *Members;
		MemberMap *Settings;
};

Skippy *IsSkippy(SQI_Object *object);
void InitSkippy();

#endif
