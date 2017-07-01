/*
 * Squirrel project
 *
 * file ?	SQI-bmenu.h
 * what	?   bmenu object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	02/16/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Menu.h> 
  
#define SQI_BMENU	21  

class SQI_BMenu;	// forwar def
class SQI_Menu;		// forward def
class SQI_BWindow; 	// forward def
class TGlue;		// forward def

#define CLASS_BMENU		7

typedef map<string,SQI_Object *(SQI_BMenu::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > BMenuMembers;
  
class SQI_BMenu : public SQI_BWidget
{
	public:
		SQI_BMenu(SQI_Squirrel *squirrel,const char *label);
		SQI_BMenu(SQI_Heap *target,const char *label);
		~SQI_BMenu();	
		// Member methods	
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		bool Suicidal(bool force=false);
  		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		void SetEnabled(bool state);
		// Member methods
		SQI_Object *Add(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Config(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Enable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IEnable(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IMark(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Find(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Remove(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// Config Settings methods
		SQI_Object *Radio(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy from SQI_Widget	
		BView *GiveBView();
		SQI_Widget *GiveWidget();
		// public variable
		SQI_Menu *View;
	private: 
		BMenuMembers *Members;
		BMenuMembers *Settings;				
};

// functions

void InitBMenu();

SQI_BMenu *IsMenu(SQI_Object *object);
