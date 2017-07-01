/*
 * Squirrel project
 *
 * file ?	SQI-bbox.cpp
 * what	?  	bbox object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbox.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-box.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *BoxMethods  = NULL;	// methods hash table
MethodTable *BoxSettings = NULL;	// settings hash table
  
/*
 * function    : SQI_BBox
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
SQI_BBox::SQI_BBox(SQI_Squirrel *squirrel,const char *label,int width,int height,uint32 resizingMode,uint32 flags,border_style border)
	:SQI_BWidget(squirrel,SQI_BBOX)
{	
	if(width>0 && height>0)
		box = new SQI_Box(squirrel->interpreter,this,label,BRect(0,0,width,height),resizingMode,flags | B_FRAME_EVENTS,border);	
	else
	if(width>0)
		box = new SQI_Box(squirrel->interpreter,this,label,BRect(0,0,width,0),resizingMode,flags | B_FRAME_EVENTS,border);		
	else
	if(height>0)
		box = new SQI_Box(squirrel->interpreter,this,label,BRect(0,0,0,height),resizingMode,flags | B_FRAME_EVENTS,border);	
	else
		box = new SQI_Box(squirrel->interpreter,this,label,BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS,border);		
	
	Label = NULL;
	
	View   = dynamic_cast<BView *>(box);
	Widget = dynamic_cast<SQI_Widget *>(View);					

	
	SetClass(CLASS_CONTAINER);
	
	if(!BoxMethods)
		InitBBox();	
				
	Methods		= BoxMethods;
	Settings 	= BoxSettings;
}

/*
 * function    : SQI_BBox
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to use
 * char *label, label to use
 * BRect frame, fram of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BBox::SQI_BBox(SQI_Heap *target,const char *label,BRect frame,uint32 resizingMode,uint32 flags,border_style border)
	:SQI_BWidget(target,SQI_BBOX)
{	
	box = new SQI_Box(NULL,this,label,frame,resizingMode,flags | B_FRAME_EVENTS,border);		
	
	Label = NULL;
	
	View   = dynamic_cast<BView *>(box);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	SetClass(CLASS_CONTAINER);
	
	if(!BoxMethods)
		InitBBox();	
				
	Methods		= BoxMethods;
	Settings 	= BoxSettings;	
}

/*
 * function    : SQI_BBox
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
SQI_BBox::SQI_BBox(SQI_Squirrel *squirrel,SQI_BWidget *label,int width,int height,uint32 resizingMode,uint32 flags,border_style border)
	:SQI_BWidget(squirrel,SQI_BBOX)
{	
	BView *lv = label->GiveBView();

	if(width>0 && height>0)
		box = new SQI_Box(squirrel->interpreter,this,lv,BRect(0,0,width,height),resizingMode,flags | B_FRAME_EVENTS,border);	
	else
	if(width>0)
		box = new SQI_Box(squirrel->interpreter,this,lv,BRect(0,0,width,0),resizingMode,flags | B_FRAME_EVENTS,border);		
	else
	if(height>0)
		box = new SQI_Box(squirrel->interpreter,this,lv,BRect(0,0,0,height),resizingMode,flags | B_FRAME_EVENTS,border);	
	else
		box = new SQI_Box(squirrel->interpreter,this,lv,BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS,border);		
	
	View   = dynamic_cast<BView *>(box);
	Widget = dynamic_cast<SQI_Widget *>(View);
	
	SetClass(CLASS_CONTAINER);
	
	Label = label;
	Label->Contained();
	Label->AddRef();
	
	if(!BoxMethods)
		InitBBox();	
				
	Methods		= BoxMethods;
	Settings 	= BoxSettings;	
}

/*
 * function    : SQI_BBox
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to use
 * char *label, label to use
 * BRect frame, fram of the view
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BBox::SQI_BBox(SQI_Heap *target,SQI_BWidget *label,BRect frame,uint32 resizingMode,uint32 flags,border_style border)
	:SQI_BWidget(target,SQI_BBOX)
{	
	BView *lv = label->GiveBView();
	
	box = new SQI_Box(NULL,this,lv,frame,resizingMode,flags | B_FRAME_EVENTS,border);		
	
	View   = dynamic_cast<BView *>(box);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	SetClass(CLASS_CONTAINER);
	
	Label = label;
	Label->Contained();
	Label->AddRef();
	
	if(!BoxMethods)
		InitBBox();	
				
	Methods		= BoxMethods;
	Settings 	= BoxSettings;	
}


/*
 * function    : SQI_BBox
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BBox::~SQI_BBox()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BBox::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			BView *v = NULL;
 			if(View)
 			{
 				if(Label)
 				{
 					box->SetLabel((BView *)NULL);
 					Label->Alone();
 					Label->RemRef();
 				}	
 				if(Window)
 				{
 					Lock();
					View->RemoveSelf();
					Unlock();
 				}	
				v = View;
 			} 		
 			delete this;
 			if(v)
 				delete v;
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
void SQI_BBox::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
		// now we need to export all the widget 
		
		if(Label)
			Label->Export(nheap,force);
		
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
SQI_Object *SQI_BBox::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();
			
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
		
		SQI_BBox *clone ;	
		
		if(Label)
		{
			SQI_BWidget *label = dynamic_cast<SQI_BWidget *>(Label->DeepClone(hp));
			clone = new SQI_BBox(hp,label,View->Frame(),View->ResizingMode(),View->Flags(),box->Border());	
		}
		else		
			clone = new SQI_BBox(hp,box->Label(),View->Frame(),View->ResizingMode(),View->Flags(),box->Border());
		
		// now we need to "clone" the view itself (set the same way)
		
		box->Clone(clone->box);
		
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
string *SQI_BBox::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Box" << '\0';
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
string *SQI_BBox::Show(int prec = 3)
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
string *SQI_BBox::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BBOX[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// legacy from SQI_Widget

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
void SQI_BBox::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
		
	if(left || top || right || bottom)
	{			
		SQI_BWidget::Expand(left,top,right,bottom);		
		box->Adapt();	
		box->ExpandChilds();
	}
}

// members function

/*
 * function    : Style
 * purpose     : Set/get the style of the bbox
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BBox::Style(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				{		
					switch(box->Border())
					{
						case B_PLAIN_BORDER:
							return new SQI_Keyword(squirrel->LocalHeap,"plain");
						case B_FANCY_BORDER:
							return new SQI_Keyword(squirrel->LocalHeap,"fancy");
						case B_NO_BORDER:
							return new SQI_Keyword(squirrel->LocalHeap,"none");	
					}
				}	
				else
				if(*str == "set")
				{
					SQI_Keyword *relief = IsKeyword((*inputs)[1]);
					
					if(relief)
					{
						str = relief->Data();
						
						if(*str == "plain")
							box->SetBorder(B_PLAIN_BORDER);
						else
						if(*str == "fancy")
							box->SetBorder(B_FANCY_BORDER);
						else
						if(*str == "none")
							box->SetBorder(B_NO_BORDER);
						else									
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~style","second input must be a valid word"));	
							
						if(Window)
						{
							Lock();
							View->Invalidate();
							Unlock();
						}
											
						return NULL;			
					}
					else	
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~style","second input must be a word"));
				}		
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~style","first input must be the word \"get or \"set"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~style","first input must be the word \"get or \"set"));		

		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~style","need at least one input"));				
	}
	
	return NULL;	
}

/*
 * function    : ReGlue
 * purpose     : ReGlue the view (dynamicaly)
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BBox::ReGlue(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(Window)
		{
			Lock();
			box->ReGluing();
			Unlock();
		}	
	}	
	return NULL;	
}

/*
 * function    : Widgets
 * purpose     : Output a list of all the widgets glued on the view
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BBox::Widgets(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		SQI_List *lst = new SQI_List(squirrel->LocalHeap);
		Lock();
		
		if(Contents)
		{
				list<TGlue *>::const_iterator i;
				list<SQI_BWidget *>::const_iterator j;			
				for(i=Contents->begin();i!=Contents->end();i++)						
					for(j=(*i)->elements->begin();j!=(*i)->elements->end();j++)
						lst->Add2End(*j);
		}	
		
		Unlock();
		return lst;	
	}
	
	return NULL;	
}



// settings function

/*
 * function    : Label
 * purpose     : Set/Get the label
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BBox::LLabel(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			if(Label)
				return Label;
			else
				return new SQI_String(squirrel->LocalHeap,box->Label());
		}	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				box->SetLabel(label->Data()->c_str());
				
				return NULL;					
			}
			if(IsWidget((*inputs)[2]))
			{
				SQI_BWidget *widget = IsWidget((*inputs)[2]);
				box->SetLabel(widget->GiveBView());
				widget->Contained();
				widget->AddRef();
				Label->Alone();
				Label->RemRef();
				Label = widget;
				return NULL;	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Border
 * purpose     : Set/Get the size of the border
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BBox::Border(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			return new SQI_Number(squirrel->LocalHeap,box->border);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *size = IsNumber((*inputs)[2]);
			
			if(size)
			{
				View->SetPenSize(Number2Float(size));
				box->border = View->PenSize() + 1;
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BOX~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBBox()
{	
	BoxMethods 	= new MethodTable();
	BoxSettings = new MethodTable();

	// specific
	BoxMethods->Add("style",(OMeth)(&SQI_BBox::Style));	
	BoxMethods->Add("reglue",(OMeth)(&SQI_BBox::ReGlue));
	BoxMethods->Add("widgets",(OMeth)(&SQI_BBox::Widgets));
	
	BoxSettings->Add("border",(OMeth)(&SQI_BBox::Border));
	BoxSettings->Add("label",(OMeth)(&SQI_BBox::LLabel));		
}

void UnInitBBox()
{
	if(BoxMethods)
	{
		delete BoxMethods;
		delete BoxSettings;	
	}	
}

/*
 * function    : IsBox
 * purpose     : Test if an object is a bbox
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BBox *, the bbox or NULL else
 * side effect : none
 */
SQI_BBox *IsBox(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BBOX)
    	return dynamic_cast<SQI_BBox *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

