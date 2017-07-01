/*
 * Squirrel project
 *
 * file ?	SQI-bsimplelist.cpp
 * what	?  	bsimplelist object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	01/30/00
 *
 *
 * (c) electron Technology 1999
 */
   
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bsimplelist.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-simplelist.h"
#include "SQI-utils.h"
#include "SQI-font.h"

BSimpleListMembers SimpleListSettingMap;
BSimpleListMembers SimpleListMemberMap;
  
/*
 * function    : SQI_BSimpleList
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
SQI_BSimpleList::SQI_BSimpleList(SQI_Squirrel *squirrel,SQI_List *items,SQI_Keyword *variable,char scroll,int width,int height,list_view_type type,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BSIMPLELIST)
{	
	View = new Container(this,BRect(0,0,0,0),B_FOLLOW_NONE,B_WILL_DRAW);

	List = new SQI_SimpleList(squirrel->interpreter,this,width,height,BRect(0,0,0,0),type,flags | B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);	

	View->AddChild(List);
	
	scroll_type = scroll;
	
	if(scroll_type != SCROLL_NONE)
	{
		Scroll = new BScrollBar(BRect(0,0,14,100),NULL,List,0,0,B_VERTICAL);
		Scroll->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(Scroll);
	}
	else
		Scroll = NULL;	
	
	
	Variable = variable;
	Variable->Contained();
	Variable->AddRef();
	Items = items;
	Items->Contained();
	Items->AddRef();
	Window = NULL;
	Members = &SimpleListMemberMap;
	Settings = &SimpleListSettingMap;	
	Contents = NULL;
	
	// we need to set up the callback from when the variable is modified
	
	squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,SimpleListVarHook);
}

/*
 * function    : SQI_BSimpleList
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
SQI_BSimpleList::SQI_BSimpleList(SQI_Heap *target,SQI_List *items,SQI_Keyword *variable,char scroll,int width,int height,BRect frame,list_view_type type,uint32 flags)
	:SQI_BWidget(target,SQI_BSIMPLELIST)
{	
	View = new Container(this,frame,B_FOLLOW_NONE,B_WILL_DRAW);	
	
	List = new SQI_SimpleList(NULL,this,width,height,frame,type,flags);
	
	View->AddChild(List);
	
	scroll_type = scroll;
	
	if(scroll_type != SCROLL_NONE)
	{
		Scroll = new BScrollBar(BRect(0,0,14,100),NULL,List,0,0,B_VERTICAL);
		Scroll->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(Scroll);
	}
	else
		Scroll = NULL;
	
	Variable = variable;
	Variable->Contained();
	Variable->AddRef();
	Items = items;
	Items->Contained();
	Items->AddRef();			
	Window = NULL;
	Members = &SimpleListMemberMap;
	Settings = &SimpleListSettingMap;	
	Contents = NULL;
}

/*
 * function    : SQI_BSimpleList
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BSimpleList::~SQI_BSimpleList()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BSimpleList::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View && List)
 			{
 				if(List->Interpreter->GlobalVar)
 					List->Interpreter->GlobalVar->RemoveSystemFunc(Variable->Data(),BIND_SET);
 				Variable->Alone();	
 				Variable->RemRef();
 				Items->Alone();
				Items->RemRef();
				if(Window)
				{
	 				Lock();
	 				View->RemoveSelf();
	 				List->RemoveSelf();
					if(Scroll)
						Scroll->RemoveSelf();
					Unlock();
				}
				if(Scroll)
					delete Scroll;
				delete View;
				delete List;
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
void SQI_BSimpleList::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		// now we need to export all the target
		Variable->Export(nheap,force);
		Items->Export(nheap,force);	
	}
}

/*
 * function    : Clone
 * purpose     : clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Clone(SQI_Heap *target = NULL)
{
	return DeepClone(target);
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
SQI_Object *SQI_BSimpleList::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
						
		SQI_BSimpleList *clone = new SQI_BSimpleList(hp,Items,VariableClone(Variable),scroll_type,0,0,View->Frame(),List->ListType(),List->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		List->Clone(clone->List);
							
		// and it's over
		
		Unlock();		
			
		return clone;		
	}
	
	return NULL;
}

/*
 * function    : Member
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, method name
 * deque<SQI_Object *> *inputs, arguments
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BSimpleList::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
		MemberHook = (*Members)[*member];
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
		{
			Members->erase(*member);
			throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unknow member"));
		}	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unsupported by the object"));
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BSimpleList::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "SimpleList" << '\0';
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
string *SQI_BSimpleList::Show(int prec = 3)
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
string *SQI_BSimpleList::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BSIMPLELIST[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// legacy from SQI_BWidget

/*
 * function    : Hook
 * purpose     : Setup a hook
 * input       : 
 *
 * SQI_Keyword *event, event to hook
 * SQI_Object *action, action to perform
 * list<SQI_Object *> *extra, extra input supplied by the user
 *
 * output      : bool , return true if succes, false else
 * side effect : none
 */
bool SQI_BSimpleList::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	if(View)
		return List->Hook(event,action,extra);
	else
		return false;
}

/*
 * function    : UnHook
 * purpose     : Remove a hook
 * input       : 
 *
 * SQI_Keyword *event, event to unhook
 *
 * output      : bool , return true if succes, false else
 * side effect : none
 */
bool SQI_BSimpleList::UnHook(SQI_Keyword *event)
{
	if(View)
		return List->UnHook(event);
	else
		return false;
}

/*
 * function    : GiveBSimpleList
 * purpose     : Return the BSimpleList
 * input       : none
 * output      : BSimpleList *
 * side effect : none
 */
BView *SQI_BSimpleList::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BSimpleList *
 * side effect : none
 */
SQI_Widget *SQI_BSimpleList::GiveWidget()
{
	return dynamic_cast<SQI_Widget *>(static_cast<BView *>(View));	
}

/*
 * function    : TrueBounds
 * purpose     : Return the Bounds of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BSimpleList::TrueBounds()
{
	BRect b = View->Bounds();
	b.right += 2.0 * View->padx;
	b.bottom += 2.0 * View->pady;	
	return b;
}

/*
 * function    : TrueFrame
 * purpose     : Return the Frame of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BSimpleList::TrueFrame()
{
	BRect b = View->Frame();
	b.InsetBy(-View->padx,-View->pady);		
	return b;
}

/*
 * function    : MoveTo
 * purpose     : Move the view to a new position, take the pading in consideration
 * input       : 
 *
 * float x, new position
 * float y, new position
 *
 * output      : none
 * side effect : none
 */
void SQI_BSimpleList::MoveTo(float x,float y)
{	
	View->MoveTo(x+View->padx,y+View->pady);
}

/*
 * function    : MoveBy
 * purpose     : Move the view to a new position by specifying the amount to move (padding already considerated)
 * input       : 
 *
 * float w, width to add 
 * float h, height to add
 *
 * output      : none
 * side effect : none
 */
void SQI_BSimpleList::MoveBy(float w,float h)
{
	View->MoveBy(w,h);
}


/*
 * function    : Area
 * purpose     : Return the area covered by the view
 * input       : none
 * output      : float
 * side effect : none
 */
double SQI_BSimpleList::Area()
{
	BRect t = TrueBounds();	
	
	return t.Width() * t.Height();
}

/*
 * function    : Expand
 * purpose     : Expand the view
 * input       : 
 *
 * float left, value to add on left
 * float top, value to add on top
 * float right, value to add on right
 * float bottom, value to add on bottom
 *
 * output      : float
 * side effect : none
 */
void SQI_BSimpleList::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
		
	View->MoveBy(-left,-top);
	
	// now we resize-it
	
	View->ResizeBy(right+left,top+bottom);
	
	// we need also to expand the List and the scrollbar

	if(Scroll)
	{
		Scroll->ResizeTo(14,View->Bounds().bottom);
		List->ResizeTo(View->Bounds().Width()-15,View->Bounds().Height());
		switch(scroll_type)
		{
			case SCROLL_RIGHT:
			{
				Scroll->MoveTo(List->Bounds().right+1,0);
				break;	
			}	
			case SCROLL_LEFT:
			{
				List->MoveTo(Scroll->Bounds().right+1,0);
				break;	
			}
		}
	}
	else
		List->ResizeTo(View->Bounds().Width(),View->Bounds().Height());			
}

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it prefered size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BSimpleList::ResizeToPreferred()
{
	List->ResizeToPreferred();
	// we need to set the scroll to the correct size now
	BRect f = List->Frame();
		
	if(Scroll)
	{
		switch(scroll_type)
		{
			case SCROLL_RIGHT:
			{	
				Scroll->MoveTo(f.right+1,f.top);
				break;
			}
			case SCROLL_LEFT:
			{
				List->MoveTo(Scroll->Frame().right+1,Scroll->Frame().top);
				break;	
			}
		}	
	
		Scroll->ResizeTo(14,f.Height());
	}	
	
	View->ResizeToPreferred();		
}

/*
 * function    : SetEnabled
 * purpose     : Set the list enabled or disabled
 * input       : 
 *
 * bool state, enable or disable
 *
 * output      : none
 * side effect : none
 */
void SQI_BSimpleList::SetEnabled(bool state)
{
	enabled = state;	
		
	const BListItem **ptr;
	BListItem *item;
			
	ptr = List->Items();		
			
	for(long i = 0;i<List->CountItems();i++)
	{
		item = const_cast<BListItem *>(ptr[i]);	
		item->SetEnabled(state);
	}	
	
	List->Invalidate();
		
}

/*
 * function    : Select
 * purpose     : Change the selection of the list according to the value
 * input       : 
 *
 * SQI_Object *value, new selected item(s)
 *
 * output      : none
 * side effect : none
 */
void SQI_BSimpleList::Select(SQI_Object *value)
{
	Lock();
	
	List->DeselectAll();	
	
	SQI_List *lst = IsList(value);
	if(lst)
	{
		int pos = -1;
		list<SQI_Object *>::const_iterator *i = lst->Iterator();
		list<SQI_Object *>::const_iterator *e = lst->End();
		
		for(i;*i!=*e;(*i)++)
		{
			if(Items->Find(*(*i),&pos))
				List->Select(pos,true);		
		}
		
		if(pos!=-1)
			List->ScrollToSelection();
		
		delete i;
		delete e;	
	}
	else
	{
		int pos;
		
		if(Items->Find(value,&pos))
		{
			List->Select(pos);
			List->ScrollToSelection();	
		}			
	}	
	
	Unlock();
}

// members function

/*
 * function    : Config
 * purpose     : Set/Get the configuration of the view
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BSimpleList::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
				Setting = (*Settings)[*key];
			
				if(Setting)	
				{
					Lock();
					SQI_Object * ret = (this->*Setting)(squirrel,inputs);
					Unlock();
					return ret;
				}
				else
				{
					Settings->erase(*key);
					throw(new SQI_Exception(SQI_EX_INVALIDE,key->c_str(),"unknow setting"));
				}			
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SimpleList~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"SimpleList~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Enable
 * purpose     : Enable/Disable all the widget in the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Number *state = IsBool((*inputs)[0]);
		if(state)
		{
			bool s = false;
			if(state->IsTrue())
				s = true;
			
			Lock();
			SetEnabled(s);
			Unlock();	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SimpleList~enable","input must be a boolean"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"SimpleList~enable","need one input"));
	
	return NULL;	
}

/*
 * function    : AddItem
 * purpose     : Add items in the list
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::AddItem(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		Lock();
		// if the list had more than one reference, we need to make a copy
		if(Items->REF>1)
		{
			SQI_List *lst = dynamic_cast<SQI_List *>(Items->Clone());
			
			Items->Alone();
			Items->RemRef();
			Items = lst;
			Items->Contained();
			Items->AddRef();
		}		
		
		SQI_Object *item;	
		
		for(int i=0;i<inputs->Length();i++)
		{
			item = (*inputs)[i];
			if(item->Is()==SQI_LIST)
			{
				SQI_List *lst = IsList(item);
				
				if(lst->Length())
				{				
					list<SQI_Object *>::const_iterator *i,*e;
					
					i = lst->Iterator();
					e = lst->End();
					
					for(i;*i!=*e;(*i)++)
					{
						item = *(*i);
						Items->Add2End(item);
						List->AddAnItem(item);	
					}
					
					delete i;
					delete e;
				}			
			}
			else
			{
				Items->Add2End(item);
				List->AddAnItem(item);		
			}	
		}
				
		// that's all
		Unlock();
	}
	
	return NULL;
}

/*
 * function    : AddItemAt
 * purpose     : Add items in the list at an index
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::AddItemAt(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		Lock();
		// if the list had more than one reference, we need to make a copy
		if(Items->REF>1)
		{
			SQI_List *lst = dynamic_cast<SQI_List *>(Items->Clone());
			
			Items->Alone();
			Items->RemRef();
			Items = lst;
			Items->Contained();
			Items->AddRef();
		}		
		
		SQI_Number *index = IsInteger((*inputs)[0]);
		
		if(index)
		{	
			int32 pos = Number2Long(index);
			
			if(pos>=0 && pos<List->CountItems())
			{	
				SQI_Object *item;	
				
				for(int i=inputs->Length()-1;i>0;i--)
				{
					item = (*inputs)[i];
					if(item->Is()==SQI_LIST)
					{
						SQI_List *lst = IsList(item);
						
						if(lst->Length())
						{			
							list<SQI_Object *>::const_iterator *i,*e;
							
							i = lst->End();
							e = lst->Iterator();
							
							for((*i)--;*i!=*e;(*i)--)
							{
								item = *(*i);
								Items->AddAt(item,pos);
								List->AddAnItem(item,pos);	
							}
							
							item = **i;
							Items->AddAt(item,pos);
							List->AddAnItem(item,pos);
								
							delete i;
							delete e;
						}			
					}
					else
					{	
						Items->Add2End(item);
						List->AddAnItem(item,pos);
					}		
				}
			}
		}	
		else
		{
			Unlock();
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~additem.at","first input must be an integer"));	
		}
				
		Unlock();
	}
	
	return NULL;
}

/*
 * function    : Items
 * purpose     : Return the list of items
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::ItemsList(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{	
		
			SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
			
			if(cmd)
			{
				string *str = cmd->Data();
				if(*str == "get")
					return Items;	
				else
				if(*str == "set")
				{
					if(inputs->Length()==2)
					{	
						SQI_List *items = IsList((*inputs)[1]);
						
						if(items)
						{
							Lock();
							Items->Alone();
							Items->RemRef();
							
							Items = items;
							Items->Contained();
							Items->AddRef();
							
							if(!enabled)
								value_range = 0;		
												
							List->AddItems(Items);
														
							Unlock();
															
							return NULL;			
						}
						else	
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~items","second input must be a list"));
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~items","need 2 inputs"));	
				}		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~items","first input must be the word \"get or \"set"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~items","first input must be the word \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~items","need 1 input at least"));	
	}
}

// settings function

/*
 * function    : AlignVertical
 * purpose     : Set/Get the vertical alignment
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();
				
			switch(widget->valign)
			{	
				case ALIGN_TOP:
					return squirrel->LocalHeap->Keyword("top");
				case ALIGN_CENTER:
					return squirrel->LocalHeap->Keyword("center");
				case ALIGN_BOTTOM:
					return squirrel->LocalHeap->Keyword("bottom");
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *align = IsKeyword((*inputs)[2]);
			
			if(align)
			{
				string *a = align->Data();
				SQI_Widget *widget = GiveWidget();	
					
				
				if(*a == "top")
					widget->valign = ALIGN_TOP;
				else
				if(*a == "center")
					widget->valign = ALIGN_CENTER;
				else
				if(*a == "bottom")
					widget->valign = ALIGN_BOTTOM;										
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : AlignHorizontal
 * purpose     : Set/Get the vertical alignment
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();
				
			switch(widget->halign)
			{	
				case ALIGN_LEFT:
					return squirrel->LocalHeap->Keyword("left");
				case ALIGN_CENTER:
					return squirrel->LocalHeap->Keyword("center");
				case ALIGN_RIGHT:
					return squirrel->LocalHeap->Keyword("right");
			}	
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *align = IsKeyword((*inputs)[2]);
			
			if(align)
			{
				string *a = align->Data();
				SQI_Widget *widget = GiveWidget();	
					
				
				if(*a == "left")
					widget->halign = ALIGN_LEFT;
				else
				if(*a == "center")
					widget->halign = ALIGN_CENTER;
				else
				if(*a == "right")
					widget->halign = ALIGN_RIGHT;										
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : PadX
 * purpose     : Set/Get the x padding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->padx);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				SQI_Widget *widget = GiveWidget();
				widget->padx = Number2Float(pad);	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : PadY
 * purpose     : Set/Get the y padding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->pady);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *pad = IsNumber((*inputs)[2]);
			
			if(pad)
			{
				SQI_Widget *widget = GiveWidget();
				widget->pady = Number2Float(pad);
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Pad
 * purpose     : Set/Get the pading (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==4)
	{	
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
					
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Number(widget->padx));
				lst->Add2End(squirrel->LocalHeap->Number(widget->pady));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Number *x = IsNumber((*inputs)[2]);
				SQI_Number *y = IsNumber((*inputs)[3]);	
				
				if(x && y)
				{
					SQI_Widget *widget = GiveWidget();	
					
					widget->padx = Number2Float(x);
					widget->pady = Number2Float(y);	

					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","need 4 inputs"));		
	
}

/*
 * function    : Align
 * purpose     : Set/Get the alignment (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==4)
	{	
		
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
				char *h,*v;
					
				switch(widget->halign)
				{	
					case ALIGN_LEFT:
						h = "left";
					case ALIGN_CENTER:
						h = "center";
					case ALIGN_RIGHT:
						h = "right";
				}	
					
				switch(widget->valign)
				{	
					case ALIGN_TOP:
						v = "left";
					case ALIGN_CENTER:
						v = "center";
					case ALIGN_BOTTOM:
						v = "right";
				}
				
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Keyword(h));
				lst->Add2End(squirrel->LocalHeap->Keyword(v));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Keyword *ho = IsKeyword((*inputs)[2]);
				SQI_Keyword *ve = IsKeyword((*inputs)[3]);	
				
				if(ho && ve)
				{
					string *a = ho->Data();
					SQI_Widget *widget = GiveWidget();	
					
					if(*a == "left")
						widget->halign = ALIGN_LEFT;
					else
					if(*a == "center")
						widget->halign = ALIGN_CENTER;
					else
					if(*a == "right")
						widget->halign = ALIGN_RIGHT;										
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be \"left \"center \"right"));
				
					a = ve->Data();	
						
					if(*a == "top")
						widget->valign = ALIGN_TOP;
					else
					if(*a == "center")
						widget->valign = ALIGN_CENTER;
					else
					if(*a == "bottom")
						widget->valign = ALIGN_BOTTOM;										
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","need 4 inputs"));		
	
}

/*
 * function    : ExpandX
 * purpose     : Set/Get the x expand
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->expandx);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *ex = IsBool((*inputs)[2]);
			
			if(ex)
			{
				SQI_Widget *widget = GiveWidget();
				widget->expandx = Number2Bool(ex);	
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : ExpandY
 * purpose     : Set/Get the y expand
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Widget *widget = GiveWidget();		
			return squirrel->LocalHeap->Number(widget->expandy);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *ex = IsBool((*inputs)[2]);
			
			if(ex)
			{
				SQI_Widget *widget = GiveWidget();
				widget->expandy = Number2Bool(ex);
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Expand
 * purpose     : Set/Get the expand (both vertical and horizontal)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=2)
	{	
		SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
		
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				SQI_Widget *widget = GiveWidget();
					
				SQI_List *lst = squirrel->LocalHeap->List();
				
				lst->Add2End(squirrel->LocalHeap->Number(widget->expandx));
				lst->Add2End(squirrel->LocalHeap->Number(widget->expandy));	
				
				return lst;
			}	
			else
			if(*str == "set")
			{
				SQI_Number *x = IsBool((*inputs)[2]);
				SQI_Number *y = IsBool((*inputs)[3]);	
				
				if(x && y)
				{
					SQI_Widget *widget = GiveWidget();	
					
					widget->expandx = Number2Bool(x);
					widget->expandy = Number2Bool(y);	

					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","need 2 inputs at least"));		
	
}

/*
 * function    : BgColor
 * purpose     : Set/Get the background color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = List->ViewColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
			GetListFromColor(squirrel,lst,&color);	
			
			return lst;
		}	
		else
		if(*str == "set")
		{
			SQI_List *lst = IsList((*inputs)[2]);
			
			if(lst)
			{
				rgb_color color;
				
				GetColorFromList(lst,&color);
				
				List->SetViewColor(color);	
				
				List->bgcolor_set = true;
				
				List->Invalidate();
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : HighColor
 * purpose     : Set/Get the high color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = List->HighColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
			GetListFromColor(squirrel,lst,&color);	
			
			return lst;
		}	
		else
		if(*str == "set")
		{
			SQI_List *lst = IsList((*inputs)[2]);
			
			if(lst)
			{
				rgb_color color;
				
				GetColorFromList(lst,&color);
				
				List->SetHighColor(color);	
				
				List->hcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : LowColor
 * purpose     : Set/Get the low color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = List->LowColor();
			SQI_List *lst = squirrel->LocalHeap->List();
			
			GetListFromColor(squirrel,lst,&color);	
			
			return lst;
		}	
		else
		if(*str == "set")
		{
			SQI_List *lst = IsList((*inputs)[2]);
			
			if(lst)
			{
				rgb_color color;
				
				GetColorFromList(lst,&color);
				
				List->SetLowColor(color);	
				
				List->lcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Font
 * purpose     : Set/Get the font
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BSimpleList::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{	
			BFont font;
			
			List->GetFont(&font);	
			
			return new SQI_Font(squirrel,&font);	
		}	
		else
		if(*str == "set")
		{
			SQI_Font *font = IsFont((*inputs)[2]);
			
			if(font)
			{
				List->SetFont(font);
				
				List->font_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SIMPLELIST~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBSimpleList()
{	
	SimpleListMemberMap[string("add")] = &SQI_BSimpleList::AddItem;
	SimpleListMemberMap[string("add.at")] = &SQI_BSimpleList::AddItemAt;
	SimpleListMemberMap[string("config")] = &SQI_BSimpleList::Config;
	SimpleListMemberMap[string("enable")] = &SQI_BSimpleList::Enable;
	SimpleListMemberMap[string("is.enable")] = &SQI_BSimpleList::IsEnable;
	SimpleListMemberMap[string("items")] = &SQI_BSimpleList::ItemsList;
	SimpleListMemberMap[string("is.focus")] = &SQI_BSimpleList::IsFocus;	
	
	SimpleListSettingMap[string("align")] = &SQI_BSimpleList::Align;
	SimpleListSettingMap[string("align.v")] = &SQI_BSimpleList::AlignVertical;
	SimpleListSettingMap[string("align.h")] = &SQI_BSimpleList::AlignHorizontal;
	SimpleListSettingMap[string("bgcolor")] = &SQI_BSimpleList::BgColor;
	SimpleListSettingMap[string("expand")] = &SQI_BSimpleList::Expand;
	SimpleListSettingMap[string("expand.x")] = &SQI_BSimpleList::ExpandX;
	SimpleListSettingMap[string("expand.y")] = &SQI_BSimpleList::ExpandY;
	SimpleListSettingMap[string("font")] = &SQI_BSimpleList::Font;
	SimpleListSettingMap[string("low.color")] = &SQI_BSimpleList::LowColor;
	SimpleListSettingMap[string("high.color")] = &SQI_BSimpleList::HighColor;
	SimpleListSettingMap[string("pad")] = &SQI_BSimpleList::Pad;
	SimpleListSettingMap[string("pad.x")] = &SQI_BSimpleList::PadX;
	SimpleListSettingMap[string("pad.y")] = &SQI_BSimpleList::PadY;			
}


void SimpleListVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel)
{
	SQI_BSimpleList *cb = static_cast<SQI_BSimpleList *>(data);
	
	cb->Select(value);	
}

/*
 * function    : IsSimpleList
 * purpose     : Test if an object is a bsimplelist
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BSimpleList *, the bsimplelist or NULL else
 * side effect : none
 */
SQI_BSimpleList *IsSimpleList(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BSIMPLELIST)
    	return dynamic_cast<SQI_BSimpleList *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

