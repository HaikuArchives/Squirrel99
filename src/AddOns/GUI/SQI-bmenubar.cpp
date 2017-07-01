/*
 * Squirrel project
 *
 * file ?	SQI-bmenubar.cpp
 * what	?  	bmenubar object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bmenubar.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-menubar.h"
#include "SQI-bmenu.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include "SQI-menu.h"
#include <MenuItem.h>

MethodTable *MenuBarMethods = NULL;	// methods hash table
MethodTable *MenuBarSettings= NULL;	// settings hash table

  
/*
 * function    : SQI_BMenuBar
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BMenuBar::SQI_BMenuBar(SQI_Squirrel *squirrel,menu_layout layout)
	:SQI_BWidget(squirrel,SQI_BMENUBAR)
{	
	menubar = new SQI_MenuBar(squirrel->interpreter,this,BRect(0,0,0,0),layout);		
	
	Layout = layout;
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(menubar);
	Widget = dynamic_cast<SQI_Widget *>(View);
		
	if(!MenuBarMethods)
		InitBMenuBar();	
		
	Methods		= MenuBarMethods;
	Settings 	= MenuBarSettings;
}

/*
 * function    : SQI_BMenuBar
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, the heap to use
 * int width, width of the view
 * int height, height of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BMenuBar::SQI_BMenuBar(SQI_Heap *target,BRect frame,menu_layout layout)
	:SQI_BWidget(target,SQI_BMENUBAR)
{	
	menubar = new SQI_MenuBar(NULL,this,frame,layout);		
	
	Layout = layout;		
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(menubar);
	Widget = dynamic_cast<SQI_Widget *>(View);
		
	if(!MenuBarMethods)
		InitBMenuBar();	
		
	Methods		= MenuBarMethods;
	Settings 	= MenuBarSettings;	
}

/*
 * function    : SQI_BMenuBar
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BMenuBar::~SQI_BMenuBar()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BMenuBar::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{	 		
 			if(View)
 			{	
 				BMenu *item;	
 				SQI_Menu *menu;		
									
				for(int32 j=0;j<menubar->CountItems();j++)
				{
					item = menubar->SubmenuAt(j);
					if(item)
					{	
						menu = dynamic_cast<SQI_Menu *>(item);
						menu->DelCallBack();
						menu->MagicPtr->View = NULL;
						menu->MagicPtr->imenu = NULL;
						menu->MagicPtr->menu = NULL;
						menu->MagicPtr->Alone();
						menu->MagicPtr->RemRef();
					}					
				}
				
 				if(Window)
 				{	
 					Lock();	
					View->RemoveSelf();
					Unlock();	
 				}						
									
				delete View;
 			}
 			delete this; 			
			return true;
 		}
 		
 	return false;
}

/*
 * function    : Export
 * purpose     : The object is aked to export himself to another heap
 * input       : 
 *
 * SQI_Heap *nheap, the target heap
 * char force, force the export even if the object is eternal
 *
 *
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
void SQI_BMenuBar::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		// now we need to export all the widget 
		
		if(Contents)
		{
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			SQI_BWidget *widget;
	
			for(i=Contents->begin();i!=Contents->end();i++)
			{	
				for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				{
					widget = (*j);
					widget->Export(nheap,force);
				}
			}
		}	
	}
}

/*
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BMenuBar *clone = new SQI_BMenuBar(hp,View->Frame(),Layout);
		
		// now we need to "clone" the view itself (set the same way)
		
		menubar->Clone(clone->menubar);
		
		// now we need to clone all the view attached to this view
		
		if(Contents)
		{
			list<TGlue *>::const_iterator i;
			list<SQI_BWidget *>::const_iterator j;
			SQI_BWidget *widget;
			TGlue *glue;
			
			clone->Contents = new list<TGlue *>;		
				
			for(i=Contents->begin();i!=Contents->end();i++)
			{
				glue = new TGlue;
				
				glue->alignment = (*i)->alignment;
				glue->elements = new list<SQI_BWidget *>;
				
				for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
				{
					widget = dynamic_cast<SQI_BWidget *>((*j)->DeepClone(hp));
					widget->AddRef();	
					glue->elements->push_back(widget);
					
					// we add the view to the window	
					
					clone->View->AddChild(widget->GiveBView());
				}		
				
				clone->Contents->push_back(glue);	
			}
		}	
			
		// and it's over
		
		if(View->Window())
			View->Window()->Unlock();		
			
		return clone;		
	}
	
	return NULL;
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BMenuBar::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "MenuBar" << '\0';
  	else 
   		out << "NONE" << '\0';
   
  	string *ret = new string(out.str());
  	out.freeze(false);
  	return ret;
}	
	
/*
 * function    : Show
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BMenuBar::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BMenuBar::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BMENUBAR[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// members function

/*
 * function    : Add
 * purpose     : Add a new menu to the menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Add(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_BMenu *menu;	
			
			for(int i=0;i<inputs->Length();i++)
			{
				if(menu = IsMenu((*inputs)[i]))
				{
					if(menu->HadIcon())
						menubar->AddItem(menu->IMenu());	
					else
						menubar->AddItem(dynamic_cast<BMenu *>(menu->GiveBView()));
					
					menu->Contained();
					menu->AddRef();
					menu->Export(squirrel->interpreter->GlobalHeap);
				}			
			}
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~add","need 1 argument at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Remove
 * purpose     : remove a menu from menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Remove(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_BMenu *menu;	
		
			// we need to keep in mind the frame of the menubar before
			
			BRect frame = View->Frame();	
			
			Lock();
			for(int i=0;i<inputs->Length();i++)
			{	
				if(menu = IsMenu((*inputs)[i]))
				{
					// need to check if the menu exists
					if(menubar->RemoveItem(dynamic_cast<BMenu *>(menu->GiveBView())))
					{
						menu->Alone();
						menu->RemRef();
					}
				}
			}
			Unlock();
			
			// we need to resize the menubar to the previous size now
			
			View->ResizeTo(frame.Width(),frame.Height());			

		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~remove","need 1 argument at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Find
 * purpose     : Find a menu in the menubar
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenuBar::Find(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_String *label = IsString((*inputs)[0]);
			if(!label)
				label = IsKeyword((*inputs)[0]);					
					
			if(label)
			{
				string *str = label->Data();
				BMenuItem *item;	
							
				Lock();			
				item = menubar->FindItem(str->c_str());
				Unlock();
			
				if(item)
				{
					if(item->Submenu())
					{
						BMenu *menu = item->Submenu();
						SQI_Menu *m = dynamic_cast<SQI_Menu *>(menu);
						return m->MagicPtr;		
					}
					else
						return new SQI_Number(squirrel->LocalHeap,-1);
				}
				else
					return new SQI_Number(squirrel->LocalHeap,-1);	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MenuBar~find","input must be a string/word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MenuBar~find","need 1 input"));	
	}
	
	return new SQI_Number(squirrel->LocalHeap,-1);	
}

// settings function

/*
 * function    : SetEnabled
 * purpose     : Set if the widget is enabled or not
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
void SQI_BMenuBar::SetEnabled(bool state)
{
	menubar->SetEnabled(state);	
}

// none

// some used function

// Fill up the member map
void InitBMenuBar()
{	
	MenuBarMethods 	= new MethodTable();

	MenuBarMethods->Add("add",(OMeth)(&SQI_BMenuBar::Add));
	MenuBarMethods->Add("find",(OMeth)(&SQI_BMenuBar::Find));
	MenuBarMethods->Add("remove",(OMeth)(&SQI_BMenuBar::Remove));				
}

void UnInitBMenuBar()
{
	if(MenuBarMethods)
		delete MenuBarMethods;	
}

/*
 * function    : IsMenuBar
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BMenuBar *, the bview or NULL else
 * side effect : none
 */
SQI_BMenuBar *IsMenuBar(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BMENUBAR)
    	return dynamic_cast<SQI_BMenuBar *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
