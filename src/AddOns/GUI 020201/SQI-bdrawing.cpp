/*
 * Squirrel project
 *
 * file ?	SQI-bdrawing.cpp
 * what	?   drawing object
 * who	?	jlv
 * when	?	03/20/00
 * last	?	01/30/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-drawing.h"
#include "SQI-bdrawing.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bwindow.h"

DrawingMembers DWMembers;
DrawingMembers DWConfigs;
  
/*
 * function    : SQI_BDrawing
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * char type, object type
 *
 * output      : none
 * side effect : none
 */
SQI_BDrawing::SQI_BDrawing(SQI_Squirrel *squirrel,SQI_BWindow *w,char type)
	:SQI_ThePointer(squirrel,type)
{	
	SetClass(CLASS_BDRAWING);
	
	Members = &DWMembers;
	Settings = &DWConfigs;
	
	Window = w;
}

/*
 * function    : SQI_BDrawing
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to use
 * char type, object type
 *
 * output      : none
 * side effect : none
 */
SQI_BDrawing::SQI_BDrawing(SQI_Heap *target,SQI_BWindow *w,char type)
	:SQI_ThePointer(target,type)
{
	SetClass(CLASS_BDRAWING);
	
	Window = w;	
}

/*
 * function    : ~SQI_BDrawing
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BDrawing::~SQI_BDrawing()
{	
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BDrawing::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View)
 			{
				View->RemoveSelf();
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
void SQI_BDrawing::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
		nheap->Import(this);	
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
SQI_Object *SQI_BDrawing::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BDrawing::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;	
		
		SQI_BDrawing *clone = new SQI_BDrawing(hp,Window,stype);
		
		clone->SetView(View->Clone(clone));
		
		return clone;
	}
	else
		return this;
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_BDrawing::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << View->Print(prec) << '\0';
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
string *SQI_BDrawing::Show(int prec = 3)
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
string *SQI_BDrawing::Dump()
{ 
	if(View)
		return View->Dump(ID,REF,status);
	else
		return NULL;
}

/*
 * function    : Lock
 * purpose     : Lock the window in which the widget is attached (if so)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BDrawing::Lock()
{
	if(Window)
		Window->Lock();	
}

/*
 * function    : Unlock
 * purpose     : Unlock the window in which the widget is attached (if so)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BDrawing::Unlock()
{
	if(Window)
		Window->Unlock();
}

/*
 * function    : MakeFocus
 * purpose     : Set the focus to or from the widget
 * input       : 
 *
 * bool focused, true if we set the focus
 *
 * output      : none
 * side effect : none
 */
void SQI_BDrawing::MakeFocus(bool focused = true)
{
	BView *view = GiveBView();
	if(view)
	{
		Lock();
		view->MakeFocus(focused);
		Unlock();	
	}
}

/*
 * function    : IsFocus
 * purpose     : Return true if the widget is focused
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::IsFocus(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	BView *view = GiveBView();

	if(view)
	{
		if(view->IsFocus())
			return squirrel->interpreter->True;
		else
			return squirrel->interpreter->False;	
	}
	else
		return squirrel->interpreter->False;	
}

/*
 * function    : IsDrawing
 * purpose     : Test if an object is a widget
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BDrawing *, the bwidget or NULL else
 * side effect : none
 */
SQI_BDrawing *IsDrawing(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->OfClass()==CLASS_BDRAWING)
    	return dynamic_cast<SQI_BDrawing *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

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
bool SQI_BDrawing::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	if(View)
		return View->Hook(event,action,extra);
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
bool SQI_BDrawing::UnHook(SQI_Keyword *event)
{
	if(View)
		return View->UnHook(event);
	else
		return false;
}

/*
 * function    : SetView
 * purpose     : Set the view of the BDrawing
 * input       : 
 *
 * SQI_Drawing *v, the view
 *
 * output      : none
 * side effect : none
 */
void SQI_BDrawing::SetView(SQI_Drawing *v)
{
	View = v;
	View->Interpreter = Window->Squirrel->interpreter; 
	View->Squirrel = Window->Squirrel;	
}

//other methods

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
SQI_Object *SQI_BDrawing::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BDrawing::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
		MemberHook = (*Members)[*member];
			
		if(MemberHook)
		{	
			SQI_Object *ret;
			Lock();
			ret = (this->*MemberHook)(squirrel,inputs);
			Unlock();
			return ret;	
		}	
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
 * function    : Config
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, method name
 * deque<SQI_Object *> *inputs, arguments
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BDrawing::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();			
		
				SQI_Object *(SQI_BDrawing::*SetHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
				SetHook = (*Settings)[*key];
			
				if(SetHook)
				{	
					SQI_Object *ret;
					Lock();
					ret = (this->*SetHook)(squirrel,inputs);
					Unlock();
					return ret;	
				}	
				else
				{
					Settings->erase(*key);
					throw(new SQI_Exception(SQI_EX_INVALIDE,"config call","unknow config"));
				}
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DRAWITEM~config","need 2 arguments at least"));	
	}
	
	return NULL;
}

/*
 * function    : MMoveBy
 * purpose     : Move the drawing by a certain distance
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::MoveBy(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==2)
	{
		SQI_Number *w = IsNumber((*inputs)[0]);
		SQI_Number *h = IsNumber((*inputs)[1]);
		
		if(w && h)
		{
			float fw,fh;
			
			fw = Number2Float(w);
			fh = Number2Float(h);
			
			View->MoveBy(fw,fh);	
			
			return NULL;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWRECT~move.by","inputs must be number"));		
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DRAWRECT~move.by","need two inputs"));
}

/*
 * function    : MoveTo
 * purpose     : Move the drawing to a given position
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::MoveTo(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_List *lst = IsList((*inputs)[0]);
		if(lst)
		{
			View->MoveTo(List2Point(lst));
			return NULL;	
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~move.to","input must be a list or 2 elements"));	
	}
	else
	if(inputs->Length()==2)
	{
		SQI_Number *xx = IsNumber((*inputs)[0]);
		SQI_Number *yy = IsNumber((*inputs)[1]);
		
		if(xx && yy)
		{			
			View->MoveTo(Number2Float(xx),Number2Float(yy));	
			return NULL;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~move.to","inputs must be 2 numbers"));		
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DRAWITEM~move.to","need one inputs at least"));
}

/*
 * function    : MoveToC
 * purpose     : Move the drawing to a given position (centered on it)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::MoveToC(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_List *lst = IsList((*inputs)[0]);
		if(lst)
		{
			float fh,fw;
			BPoint p = List2Point(lst);
			
			fh = View->Bounds().Height();
			fw = View->Bounds().Width();	
			
			p.y -= fh/2;
			p.x -= fw/2;	
			
			View->MoveTo(p);
			return NULL;	
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~move.to.c","input must be a list or 2 elements"));	
	}
	else
	if(inputs->Length()==2)
	{
		SQI_Number *xx = IsNumber((*inputs)[0]);
		SQI_Number *yy = IsNumber((*inputs)[1]);
		
		if(xx && yy)
		{		
			float fh,fw;
			
			fh = View->Bounds().Height();
			fw = View->Bounds().Width();		
				
				
			View->MoveTo(Number2Float(xx) - fw/2,Number2Float(yy) - fh/2);	
			return NULL;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~move.to.c","inputs must be 2 numbers"));		
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DRAWITEM~move.to.c","need one inputs at least"));
}

/*
 * function    : Fill
 * purpose     : Fill the drawing
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::Fill(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_List *lst = IsList((*inputs)[0]);
		if(lst)
		{
			rgb_color color;
			
			if(GetColorFromList(lst,&color))
			{
				Lock();
				View->Fill(color,color,B_SOLID_HIGH);
				Unlock();
				return NULL;	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~fill","input must be a color"));		
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~fill","input must be a color"));	
	}
	else
	if(inputs->Length()==2)
	{
		SQI_List *hc = IsList((*inputs)[0]);
		SQI_List *lc = IsList((*inputs)[1]);	
		
		if(hc && lc)
		{
			rgb_color lcolor,hcolor;
			
			if(GetColorFromList(lc,&lcolor) && GetColorFromList(hc,&hcolor))
			{
				Lock();
				View->Fill(lcolor,hcolor,B_MIXED_COLORS);
				Unlock();
				return NULL;	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~fill","inputs must be colors"));		
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~fill","inputs must be colors"));	
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DRAWITEM~fill","need one inputs at least"));
}

// config

/*
 * function    : Movable
 * purpose     : Set or get if the item can be moved
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::Movable(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{			
		if(View->movable)
			return squirrel->interpreter->True;
		else
			return squirrel->interpreter->False;
	}
	else
	{
		SQI_Number *val = IsBool((*inputs)[1]);
		if(val)
		{
			View->movable = val->IsTrue();
			return NULL;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~config","second input must be a boolean"));
	}
}

/*
 * function    : Layer
 * purpose     : Set or get if the item layer
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BDrawing::Layer(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
		return squirrel->LocalHeap->Number(View->layer); 
	else
	{
		SQI_Number *val = IsInteger((*inputs)[1]);
		if(val)
		{
			View->layer = Number2Int(val);
			return NULL;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DRAWITEM~config","second input must an integer"));
	}
}

void InitBDrawing()
{
	DWMembers[string("config")] 	= &SQI_BDrawing::Config;
	DWMembers[string("move.by")] 	= &SQI_BDrawing::MoveBy;
	DWMembers[string("move.to")] 	= &SQI_BDrawing::MoveTo;
	DWMembers[string("move.to.c")] 	= &SQI_BDrawing::MoveToC;
	DWMembers[string("fill")] 		= &SQI_BDrawing::Fill;
	
	DWConfigs[string("movable")]	= &SQI_BDrawing::Movable;
	DWConfigs[string("layer")]	= &SQI_BDrawing::Layer;		
}
