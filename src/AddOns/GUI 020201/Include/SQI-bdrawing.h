/*
 * Squirrel project
 *
 * file ?	SQI-bdrawing.h
 * what	?   bdrawing object
 * who	?	jlv
 * when	?	03/20/00
 * last	?	02/01/01
 *
 *
 * (c) Kirilla 2000-2001
 */
   
class SQI_BWindow; 	// forward def

#define CLASS_BDRAWING	10

class SQI_BDrawing;	// forward def

typedef map<string,SQI_Object *(SQI_BDrawing::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > DrawingMembers;

class SQI_BDrawing : public SQI_ThePointer
{
	public:
		SQI_BDrawing(SQI_Squirrel *squirrel,SQI_BWindow *w,char type);
		SQI_BDrawing(SQI_Heap *target,SQI_BWindow *w,char type);
		~SQI_BDrawing();	
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force);
  		SQI_Object *Clone(SQI_Heap *target);
  		SQI_Object *DeepClone(SQI_Heap *target);
  		// methods	
		SQI_Object *MoveBy(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MoveTo(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MoveToC(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Fill(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// configs
		SQI_Object *Movable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Layer(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Colors(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		// BDrawing methods
		virtual BView *GiveBView() {};
		virtual SQI_Drawing *GiveDrawing() {};
		bool Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra);
		bool UnHook(SQI_Keyword *event);
		virtual void MoveTo(float x,float y) {};
		virtual void MoveBy(float w,float h) {}; 
		virtual void Expand(float left,float top,float right,float bottom) {};
		virtual void ResizeToPreferred() {};
  		virtual void Invalidate() {};
  		virtual void MakeFocus(bool focused = true);
		SQI_Object *IsFocus(SQI_Squirrel *squirrel,SQI_Args *inputs);  	
		// view function
		void SetView(SQI_Drawing *v);	
		// Window Locking
  		void Lock();
  		void Unlock();
  		// variables
  		SQI_BWindow *Window;	 // window to which the view is attached (NULL if not attached)
  		SQI_Drawing *View;		 // View	
  	private:
  		DrawingMembers *Members;
		DrawingMembers *Settings;		
};

SQI_BDrawing *IsDrawing(SQI_Object *object);

void InitBDrawing();
