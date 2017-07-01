/*
 * Squirrel project
 *
 * file ?	SQI-bwindow.h
 * what	?   BWindow object
 * who	?	jlv
 * when	?	11/23/99
 * last	?	09/20/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Window.h>
#include <Font.h>
#include "SQI-window.h"

#define SQI_BWINDOW		10
#define CLASS_BWINDOW	3

class SQI_BWindow; // forwar def

typedef map<string,SQI_Object *(SQI_BWindow::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BWindowMembers;

class SQI_BWindow : public SQI_ThePointer
{
	public:
		SQI_BWindow(SQI_Squirrel *squirrel,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags);
		SQI_BWindow(SQI_Heap *target,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags);
		~SQI_BWindow();	
		// Member methods	
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
		// Member methods
		SQI_Object *Activate(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Add2Subset(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Rem2Subset(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Bounds(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Close(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *DeActivate(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Enable(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		SQI_Object *Frame(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Hide(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IsActive(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IsFront(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IsHidden(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Minimize(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MoveBy(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MoveTo(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ResizeBy(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ResizeTo(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *UnMinimize(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Show(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Center(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Focus(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *DefaultFont(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *DefaultButton(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *PulseRate(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Title(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Look(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Feel(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Workspace(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Zoom(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Hook methods
		bool Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra);
		bool UnHook(SQI_Keyword *event);
		// Glue methods (Adding View)
		void Glue(char align,list<SQI_BWidget *> *widgets);
		void UnGlue(SQI_BWidget *widget);
		void Add2Contents(char align,list<SQI_BWidget *> *widgets);
		// locking methods
		void Lock();
		void Unlock();	
		// public variable
		SQI_Window *Window;			// the window
		SQI_Interp *Interpreter;   	// Interpreter
		SQI_Squirrel *Squirrel;    	// Squirrel of the window
		list<TGlue *> *Contents;	// list of gluing sequence
		BFont Font;					// default font	
	private: 
		BWindowMembers *Members;
		BWindowMembers *Settings;
};

// functions

void InitBWindow();

// Return the font if the object is a font
SQI_BWindow *IsWindow(SQI_Object *object);
