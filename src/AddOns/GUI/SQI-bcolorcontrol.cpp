/*
 * Squirrel project
 *
 * file ?	SQI-bcolorcontrol.cpp
 * what	?  	bcolorcontrol object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bcolorcontrol.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-colorcontrol.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *CControlMethods = NULL;	// methods hash table
MethodTable *CControlSettings= NULL;// settings hash table
 
/*
 * function    : SQI_BColorControl
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
SQI_BColorControl::SQI_BColorControl(SQI_Squirrel *squirrel,color_control_layout matrix,float cellSide,SQI_Keyword *var)
	:SQI_BWidget(squirrel,SQI_BCOLORCONTROL)
{	
	control = new SQI_ColorControl(squirrel->interpreter,this,matrix,cellSide,new BMessage(WAS_INVOKED),BPoint(0,0));		
	
	View   = dynamic_cast<BView *>(control);
	Widget = dynamic_cast<SQI_Widget *>(View);	

	Variable = var;
	Variable->AddRef();
	Variable->Contained();
	
	if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
	{
		SQI_Object *value = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
		SQI_List *color = IsList(value);
		if(color)
		{
			rgb_color c;
		
			GetColorFromList(color,&c);	
			
			control->SetValue(c);	
		}	
	}
	else
	{
		rgb_color c;
		c.red = 0;
		c.green = 0;
		c.blue = 0;
		c.alpha = 255;
		
		SQI_List *color = new SQI_List(squirrel->LocalHeap);	
		
		GetListFromColor(squirrel,color,&c);
		
		squirrel->interpreter->GlobalVar->Set(squirrel,Variable->Data(),color);	
	}	
	
	// we need to set up the callback from when the variable is modified
	
	squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,ColorControlVarHook);
	
	if(!CControlMethods)
		InitBColorControl();	
		
	Methods		= CControlMethods;
	Settings 	= CControlSettings;		
}

/*
 * function    : SQI_BColorControl
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
SQI_BColorControl::SQI_BColorControl(SQI_Heap *target,color_control_layout matrix,float cellSide,SQI_Keyword *var,BPoint lefttop)
	:SQI_BWidget(target,SQI_BCOLORCONTROL)
{	
	control = new SQI_ColorControl(NULL,this,matrix,cellSide,new BMessage(WAS_INVOKED),lefttop);		

	Variable = var;
	Variable->AddRef();
	Variable->Contained();

	View   = dynamic_cast<BView *>(control);
	Widget = dynamic_cast<SQI_Widget *>(View);	

	if(!CControlMethods)
		InitBColorControl();	
		
	Methods		= CControlMethods;
	Settings 	= CControlSettings;	
}

/*
 * function    : SQI_BColorControl
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BColorControl::~SQI_BColorControl()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BColorControl::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View)
 			{
 				if(control->Interpreter->GlobalVar)
 					control->Interpreter->GlobalVar->RemoveSystemFunc(Variable->Data(),BIND_SET);
 				if(Window)
 				{	
 					Lock();	
					View->RemoveSelf();
					Unlock();
 				}	
				delete View;
 			}
 			
 			Variable->RemRef();		
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
void SQI_BColorControl::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
		nheap->Import(this);
		
	Variable->Export(nheap,force);	
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
SQI_Object *SQI_BColorControl::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BColorControl *clone = new SQI_BColorControl(hp,control->Layout(),control->CellSize(),VariableClone(Variable),View->Frame().LeftTop());
		
		// now we need to "clone" the view itself (set the same way)
		
		control->Clone(clone->control);
					
		// and it's over
		
		Unlock();		
			
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
string *SQI_BColorControl::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "" << '\0';
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
string *SQI_BColorControl::Show(int prec = 3)
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
string *SQI_BColorControl::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BCOLORCONTROL[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// members function

/*
 * function    : Invoke
 * purpose     : Invoke the button click hook
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_BColorControl::Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		Lock();
		control->Invoked(View);
		Unlock();	
		return NULL;				
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
SQI_Object *SQI_BColorControl::Layout(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_Number *layout;
			
			switch(control->Layout())
			{
				case B_CELLS_4x64:
				{
						layout = new SQI_Number(squirrel->LocalHeap,4);
						break;
				}		
				case B_CELLS_8x32:
				{
						layout = new SQI_Number(squirrel->LocalHeap,8);
						break;
				}			
				case B_CELLS_16x16:
				{
						layout = new SQI_Number(squirrel->LocalHeap,16);
						break;
				}
				case B_CELLS_32x8:
				{
						layout = new SQI_Number(squirrel->LocalHeap,32);
						break;
				}
				case B_CELLS_64x4:
				{
						layout = new SQI_Number(squirrel->LocalHeap,64);
						break;
				}
			}	
			
			return layout;
		}	
		else
		if(*str == "set")
		{
			SQI_Number *layout = IsInteger((*inputs)[2]);			
			if(layout)
			{
				int l = Number2Int(layout);
				color_control_layout matrix;	
				
				switch(l)
				{
					case 4:
					{
						matrix = B_CELLS_4x64;
						break;	
					}		
					case 8:
					{
						matrix = B_CELLS_8x32;
						break;	
					}			
					case 16:
					{
						matrix = B_CELLS_16x16;
						break;	
					}
					case 32:
					{
						matrix = B_CELLS_32x8;
						break;	
					}
					case 64:
					{
						matrix = B_CELLS_64x4;
						break;	
					}
					default:
						throw(new SQI_Exception(SQI_EX_INVALIDE,"COLORCONTROL~config","the layout must be 4 8 16 32 or 64"));		
				}	
				
				control->SetLayout(matrix);
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","third input must be an integer"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Value
 * purpose     : Set/Get the value of the checkbox
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BColorControl::Value(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			rgb_color c = control->ValueAsColor();
			SQI_List *color = new SQI_List(squirrel->LocalHeap);
			
			GetListFromColor(squirrel,color,&c);
			
			return color;
		}	
		else
		if(*str == "set")
		{
			SQI_List *color = IsList((*inputs)[2]);
		
			if(color)
			{
				rgb_color c;
				
				GetColorFromList(color,&c);	
							
				control->SetValue(c);	
							
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Variable
 * purpose     : Set/Get the variable to use in the checkbox
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BColorControl::Var(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return Variable;
		else
		if(*str == "set")
		{
			SQI_Keyword *var = IsKeyword((*inputs)[2]);
		
			if(var)
			{
				var->Contained();
				var->AddRef();
			
				squirrel->interpreter->GlobalVar->RemoveSystemFunc(Variable->Data(),BIND_SET);
				Variable->Alone();
				Variable->RemRef();
				
				Variable = var;
				
				if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
				{
					SQI_Object *value = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
					SQI_List *color = IsList(value);
					if(color)
					{
						rgb_color c;
					
						GetColorFromList(color,&c);	
						control->SetValue(c);
					}	
				}
				else
				{
					SQI_List *color = new SQI_List(squirrel->LocalHeap);
				
					rgb_color c = control->ValueAsColor();
					
					GetListFromColor(squirrel,color,&c);	
					 	
					squirrel->interpreter->GlobalVar->Set(squirrel,Variable->Data(),color);
				}	
				
				squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,ColorControlVarHook);	
			
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : CellSide
 * purpose     : Set/Get the side size of a cell
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BColorControl::CellSize(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			return new SQI_Number(squirrel->LocalHeap,control->CellSize());
		}	
		else
		if(*str == "set")
		{
			SQI_Number *size = IsInteger((*inputs)[2]);
			
			if(size)
			{
				control->SetCellSize(Number2Float(size));	
			
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","third input must be an integer"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"COLORCONTROL~config","second input must be the word \"get or \"set"));		

}

// some used function

// Fill up the member map
void InitBColorControl()
{	
	CControlMethods 	= new MethodTable();
	CControlSettings 	= new MethodTable();

	CControlMethods->Add("invoke",(OMeth)(&SQI_BColorControl::Invoke));

	CControlSettings->Add("cellsize",(OMeth)(&SQI_BColorControl::CellSize));
	CControlSettings->Add("value",(OMeth)(&SQI_BColorControl::Value));	
	CControlSettings->Add("variable",(OMeth)(&SQI_BColorControl::Var));			
}

void UnInitBColorControl()
{
	if(CControlMethods)
	{
		delete CControlMethods;
		delete CControlSettings;	
	}	
}

void ColorControlVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel)
{
	SQI_BColorControl *cb = static_cast<SQI_BColorControl *>(data);
	SQI_List *color = IsList(value);
	if(color)
	{
		rgb_color c;
		
		GetColorFromList(color,&c);
		
		cb->control->SetValue(c);	
	}
}

/*
 * function    : IsColorControl
 * purpose     : Test if an object is a bcolorcontrol
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BColorControl *, the bcolorcontrol or NULL else
 * side effect : none
 */
SQI_BColorControl *IsColorControl(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BCOLORCONTROL)
    	return dynamic_cast<SQI_BColorControl *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

