/*
 * Squirrel project
 *
 * file ?	SQI-bmenubar.cpp
 * what	?  	bmenu object
 * who	?	jlv
 * when	?	12/08/99
 * last	?	02/13/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bmenu.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-menu.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include "SQI-menuitem.h"

MethodTable *MenuMethods;		// methods hash table
MethodTable *MenuSettings;	// settings hash table

#define MENU_CALLBACK	'MenU'
  
/*
 * function    : SQI_BMenu
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
SQI_BMenu::SQI_BMenu(SQI_Squirrel *squirrel,const char *label)
	:SQI_BWidget(squirrel,SQI_BMENU)
{	
	imenu 	= NULL;
	menu 	= new SQI_Menu(squirrel->interpreter,this,label);		
			
	SetClass(CLASS_BMENU);
	
	View   = dynamic_cast<BView *>(menu);
	Widget = dynamic_cast<SQI_Widget *>(View);
	
	if(!MenuMethods)
		InitBMenu();
				
	Methods		= MenuMethods;
	Settings 	= MenuSettings;		
}

/*
 * function    : SQI_BMenu
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
SQI_BMenu::SQI_BMenu(SQI_Squirrel *squirrel,SQI_Image *bit)
	:SQI_BWidget(squirrel,SQI_BMENU)
{	
	menu 	= new SQI_Menu(squirrel->interpreter,this,"a");		
	imenu 	= new IconMenu(bit->GetBitmap(),menu);

	SetClass(CLASS_BMENU);
	
	View   = dynamic_cast<BView *>(menu);
	Widget = dynamic_cast<SQI_Widget *>(View);
	
	if(!MenuMethods)
		InitBMenu();	
		
	Methods		= MenuMethods;
	Settings 	= MenuSettings;	
}

/*
 * function    : SQI_BMenu
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
SQI_BMenu::SQI_BMenu(SQI_Heap *target,const char *label)
	:SQI_BWidget(target,SQI_BMENU)
{	
	imenu 	= NULL;
	menu 	= new SQI_Menu(NULL,this,label);		
			
	SetClass(CLASS_BMENU);
	
	View   = dynamic_cast<BView *>(menu);
	Widget = dynamic_cast<SQI_Widget *>(View);
	
	if(!MenuMethods)
		InitBMenu();	
		
	Methods		= MenuMethods;
	Settings 	= MenuSettings;	
}

/*
 * function    : SQI_BMenu
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
SQI_BMenu::SQI_BMenu(SQI_Heap *target,SQI_Image *bit)
	:SQI_BWidget(target,SQI_BMENU)
{	
	menu 	= new SQI_Menu(NULL,this,"a");		
	imenu 	= new IconMenu(bit->GetBitmap(),menu);
			
	SetClass(CLASS_BMENU);
	
	View   = dynamic_cast<BView *>(menu);
	Widget = dynamic_cast<SQI_Widget *>(View);
	
	if(!MenuMethods)
		InitBMenu();	
		
	Methods		= MenuMethods;
	Settings 	= MenuSettings;	
}

/*
 * function    : SQI_BMenu
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BMenu::~SQI_BMenu()
{		
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BMenu::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{			 			
 			if(View)
 			{ 				
 				// removeself the submenu
 				
 				BMenu *submenu;
 				SQI_Menu *item;
 				 				
 				if(imenu)
 				{
 					BMenu *jmenu = imenu->Submenu();
 				 					
 					for(int32 i=0;i<jmenu->CountItems();i++)
 					{
 						submenu = jmenu->SubmenuAt(i);
 						if(submenu)
 						{
 							jmenu->RemoveItem(submenu); // remove the item	
 							item = dynamic_cast<SQI_Menu *>(submenu);
 							if(item)
 							{
 								item->MagicPtr->Alone();
 								item->MagicPtr->RemRef();		
 							}
 							i--; // decrease as we have remove an element		
 						}		
 					}
 					
 					if(imenu->Menu())
 						imenu->Menu()->RemoveItem(imenu);	
 					
 					delete imenu;
 				}	
				else
				{
					for(int32 i=0;i<menu->CountItems();i++)
 					{
 						submenu = menu->SubmenuAt(i);
 						if(submenu)
 						{
 							menu->RemoveItem(submenu); // remove the item	
 							item = dynamic_cast<SQI_Menu *>(submenu);
 							if(item)
 							{
 								item->MagicPtr->Alone();
 								item->MagicPtr->RemRef();		
 							}
 							i--; // decrease as we have remove an element		
 						}		
 					}
 					
 					if(imenu->Menu())
 						imenu->Menu()->RemoveItem(imenu); 					
 											
					delete View;
				}		
 			}
 		 			 			
 			delete this;
			return true;
 		}
 		
 	return false;
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
SQI_Object *SQI_BMenu::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BMenu *clone = new SQI_BMenu(hp,"");
		
		// now we need to "clone" the view itself (set the same way)
		
		menu->Clone(clone->menu);
					
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
string *SQI_BMenu::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Menu" << '\0';
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
string *SQI_BMenu::Show(int prec = 3)
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
string *SQI_BMenu::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BMENU[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

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
void SQI_BMenu::SetEnabled(bool state)
{
	menu->SetEnabled(state);
	SQI_BWidget::SetEnabled(state);		
}

// members function

/*
 * function    : Add
 * purpose     : Add an item to the menu
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenu::Add(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			uint32 modifiers = NULL;
			SQI_String *shortcut = NULL;
			SQI_String *label = IsString((*inputs)[0]);
			if(!label)
				label = IsKeyword((*inputs)[0]);
			
			if(!label)
			{
				SQI_List *lst = IsList((*inputs)[0]);
				if(lst)
				{
					if(lst->Length()>=2)
					{
						list<SQI_Object *> *l;
						list<SQI_Object *>::const_iterator i;
						
						l = lst->Data();
						
						i = l->begin();	
						
						label = IsString(*i);
						if(!label)
							label = IsKeyword(*i);
							
						if(!label)
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","the label must be a string or a word"));						
							
						i++;
						shortcut = IsString(*i);
						if(!shortcut)
							shortcut = IsKeyword(*i);
							
						if(!shortcut)
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","the shortcut must be a string or a word"));		
					
						// the modifier
						if(lst->Length()>=3)
						{
							SQI_Keyword *word;	
							string *str;	
							
							for(i++;i!=l->end();i++)
							{
								cout << *((*i)->Dump()) << "\n";
								word = IsKeyword(*i);
								if(word)
								{
									str = word->Data();
									
									if(*str == "shift")
										modifiers = modifiers | B_SHIFT_KEY;
									else							
									if(*str == "control")
										modifiers = modifiers | B_CONTROL_KEY;
									else										
									if(*str == "option")
										modifiers = modifiers | B_OPTION_KEY;
									else							
									if(*str == "alt")
										modifiers = modifiers | B_COMMAND_KEY;
									else
										throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","modifiers must be : \"shift \"control \"option \"alt"));		
								}
								else
									throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","the list must containt : string words..."));		
							}	
						}
							
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","the list must containt 2 elements at least"));							
				}
				else
				{	
					SQI_BMenu *submenu;
					
					if(submenu = IsMenu((*inputs)[0]))
					{
						SQI_MenuItem *item = new SQI_MenuItem(NULL,dynamic_cast<BMenu *>(submenu->GiveBView()));
						menu->AddItem(item);
						submenu->Export(squirrel->interpreter->GlobalHeap);
						submenu->Contained();
						submenu->AddRef();
						return new SQI_Number(squirrel->LocalHeap,(long)(menu->IndexOf(item)));		
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~add","first input must be word/string/list or another menu"));					
				}	
			}
			
			if(inputs->Length()>1)
			{	
				if(IsKeyword((*inputs)[1]))
				{
					SQI_Keyword *name = IsKeyword((*inputs)[1]);
					
					list<SQI_Object *> *extra = NULL;
				
					// if we have more args , it's extra inputs for the hook
				
					if(inputs->Length()>2)
					{
						SQI_Object *obj;
						extra = new list<SQI_Object *>;
						for(int i=2;i<inputs->Length();i++)
						{
							obj = (*inputs)[i];
							obj->Export(squirrel->interpreter->GlobalHeap);
							obj->Contained();
							obj->AddRef();
							extra->push_back(obj);	
						}	
					} 	
					
					THook *hook = new THook;	
					
					hook->action = new SQI_nUfunc(squirrel->interpreter->GlobalHeap,name->Data());
					hook->action->AddRef();
					hook->action->Contained();
					hook->extra = extra;
					
					char s = NULL;
				
					if(shortcut)
					{
						string *str = shortcut->Data();
						s = (*str)[0];	
					}
				
					BMessage *message = new BMessage(MENU_CALLBACK);
					
					message->AddPointer("callback",hook);
						
					BMenuItem *item = new SQI_MenuItem(hook,label->Data()->c_str(),message,s,modifiers);
					menu->AddItem(item);
					return new SQI_Number(squirrel->LocalHeap,(long)(menu->IndexOf(item)));							
				}
				else
				if(IsBlock((*inputs)[1]))
				{
					SQI_nBlock *block = IsBlock((*inputs)[1]);
					
					char s = NULL;
				
					if(shortcut)
					{
						string *str = shortcut->Data();
						s = (*str)[0];	
					}
				
					BMessage *message = new BMessage(MENU_CALLBACK);
					
					THook *hook = new THook;
					
					hook->action = block;
					hook->extra = NULL;
					
					block->Contained();
					block->AddRef();	
					
					message->AddPointer("callback",hook);
						
					BMenuItem *item = new SQI_MenuItem(hook,label->Data()->c_str(),message,s,modifiers);
					menu->AddItem(item);
					return new SQI_Number(squirrel->LocalHeap,(long)(menu->IndexOf(item)));	
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Menu~add","second input must be a word or a block"));					
			}	
			else
			{
				// time to ADD this item but without any action
				
				char s = NULL;
				
				if(shortcut)
				{
					string *str = shortcut->Data();
					s = (*str)[0];	
				}
				
				string *l = label->Data();
				
				if(*l == "separator")
				{
					menu->AddSeparatorItem();
					return NULL;	
				}
				else	
				{
					//BMenuItem *item = new BMenuItem(label->Data()->c_str(),NULL,s,modifiers);
					BMenuItem *item = new SQI_MenuItem(NULL,label->Data()->c_str(),NULL,s,modifiers);	
					menu->AddItem(item);
					return new SQI_Number(squirrel->LocalHeap,(long)(menu->IndexOf(item)));	
				}	
			}
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Menu~add","need 1 input at least"));	
	}
	
	return NULL;	
}

/*
 * function    : IEnable
 * purpose     : Enable/Disable an item of the menu
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenu::IEnable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==2)
	{
		SQI_Number *item = IsInteger((*inputs)[0]);
		SQI_Number *state = IsBool((*inputs)[1]);
		if(state && item)
		{
			bool s = false;
			if(state->IsTrue())
				s = true;
		
			BMenuItem *i = menu->ItemAt(Number2Int(item));
			if(i)
			{	
				Lock();
				i->SetEnabled(s);
				Unlock();
			}	
			
			return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~i.enable","input must be a boolean"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MENU~i.enable","need one input"));
		
	return NULL;	
}

/*
 * function    : IMarked
 * purpose     : Mark/unmark an item of the menu
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenu::IMark(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==2)
	{
		SQI_Number *item = IsInteger((*inputs)[0]);
		SQI_Number *state = IsBool((*inputs)[1]);
		if(state && item)
		{
			bool s = false;
			if(state->IsTrue())
				s = true;
		
			BMenuItem *i = menu->ItemAt(Number2Int(item));
			if(i)
			{	
				Lock();
				i->SetMarked(s);
				Unlock();
			}	
			
			return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~i.mark","input must be a boolean"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MENU~i.mark","need one input"));
		
	return NULL;	
}

/*
 * function    : IFont
 * purpose     : Set/get the font of a menu item
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenu::IFont(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=2)
	{
		SQI_Number *item = IsInteger((*inputs)[0]);
		SQI_Keyword *action = IsKeyword((*inputs)[1]);
		if(action && item)
		{	
			SQI_MenuItem *i = (SQI_MenuItem *)(menu->ItemAt(Number2Int(item)));
			if(i)
			{
				string *str = action->Data();
				
				if(*str == "get")
					return new SQI_Font(squirrel,i->GetFont());
				else
				if(*str == "set")
				{
					if(inputs->Length()==3)
					{
						SQI_Font *font = IsFont((*inputs)[2]);
						if(font)
						{		
							Lock();
							i->SetFont(font);	
							Unlock();
							return NULL;
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~i.font","third input must be a font"));	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MENU~i.font","need a third input"));		
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~i.font","input must be a boolean"));	
			}	
			
			return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~i.font","inputs must be : number word"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MENU~i.font","need two input at least"));
		
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
SQI_Object *SQI_BMenu::Remove(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_BMenu *menu2;	
		
			// we need to keep in mind the frame of the menubar before
			
			BRect frame = View->Frame();	
			
			Lock();
			for(int i=0;i<inputs->Length();i++)
			{	
				if(menu2 = IsMenu((*inputs)[i]))
				{
					// need to check if the menu exists
					if(menu->RemoveItem(dynamic_cast<BMenu *>(menu2->GiveBView())))
					{
						menu2->Alone();
						menu2->RemRef();
					}
				}
				else
				{
					SQI_String *label = IsString((*inputs)[i]);
					if(!label)
						label = IsKeyword((*inputs)[i]);
						
					if(label)
					{
						BMenuItem *item = menu->FindItem(label->Data()->c_str());
						if(item)
						{
							if(item->Submenu())
							{
								BMenu *menu2 = item->Submenu();
								SQI_Menu *m = dynamic_cast<SQI_Menu *>(menu2);
								m->MagicPtr->Alone();
								m->MagicPtr->RemRef();					
							}
									
							menu->RemoveItem(item);	
						}	
					}	
				}
			}
			Unlock();
			
			// we need to resize the menubar to the previous size now
			
			View->ResizeTo(frame.Width(),frame.Height());			

		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Menu~remove","need 1 argument at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Find
 * purpose     : Find an item in the menu
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BMenu::Find(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				item = menu->FindItem(str->c_str());
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
					{
						new SQI_Number(squirrel->LocalHeap,(long)(menu->IndexOf(item)));	
					}
				}
				else
					return new SQI_Number(squirrel->LocalHeap,-1);	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu~find","input must be a string/word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Menu~find","need 1 input"));	
	}
	
	return new SQI_Number(squirrel->LocalHeap,-1);	
}

// settings function

/*
 * function    : Radio
 * purpose     : Set/Get the radio mode
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMenu::Radio(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_Number(squirrel->LocalHeap,menu->IsRadioMode());	
		else
		if(*str == "set")
		{
			SQI_Number *mode = IsBool((*inputs)[2]);
			
			if(mode)
			{
				menu->SetRadioMode(Number2Bool(mode));	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MENU~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBMenu()
{	
	MenuMethods 	= new MethodTable();
	MenuSettings 	= new MethodTable();
	
	MenuMethods->Add("add",(OMeth)(&SQI_BMenu::Add));
	MenuMethods->Add("find",(OMeth)(&SQI_BMenu::Find));
	MenuMethods->Add("remove",(OMeth)(&SQI_BMenu::Remove));
	MenuMethods->Add("i.enable",(OMeth)(&SQI_BMenu::IEnable));
	MenuMethods->Add("i.font",(OMeth)(&SQI_BMenu::IFont));
	MenuMethods->Add("i.mark",(OMeth)(&SQI_BMenu::IMark));
	
	MenuSettings->Add("radio",(OMeth)(&SQI_BMenu::Radio));
}

void UnInitBMenu()
{
	if(MenuMethods)
	{
		delete MenuMethods;
		delete MenuSettings;	
	}	
}

/*
 * function    : IsMenu
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BMenu *, the bview or NULL else
 * side effect : none
 */
SQI_BMenu *IsMenu(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BMENU)
    	return dynamic_cast<SQI_BMenu *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
