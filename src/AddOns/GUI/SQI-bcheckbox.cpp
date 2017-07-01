/*
 * Squirrel project
 *
 * file ?	SQI-bcheckbox.cpp
 * what	?  	bcheckbox object
 * who	?	jlv
 * when	?	12/02/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bcheckbox.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-checkbox.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *CheckBoxMethods = NULL;	// methods hash table
MethodTable *CheckBoxSettings = NULL;	// settings hash table
  
  
/*
 * function    : SQI_BCheckBox
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
SQI_BCheckBox::SQI_BCheckBox(SQI_Squirrel *squirrel,const char *label,SQI_Keyword *var,int width,int height,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BCHECKBOX)
{	
	if(width>0 && height>0)
		checkbox = new SQI_CheckBox(squirrel->interpreter,this,label,new BMessage(WAS_INVOKED),BRect(0,0,width,height),resizingMode,flags | B_FRAME_EVENTS);	
	else
	if(width>0)
		checkbox = new SQI_CheckBox(squirrel->interpreter,this,label,new BMessage(WAS_INVOKED),BRect(0,0,width,0),resizingMode,flags | B_FRAME_EVENTS);		
	else
	if(height>0)
		checkbox = new SQI_CheckBox(squirrel->interpreter,this,label,new BMessage(WAS_INVOKED),BRect(0,0,0,height),resizingMode,flags | B_FRAME_EVENTS);	
	else
		checkbox = new SQI_CheckBox(squirrel->interpreter,this,label,new BMessage(WAS_INVOKED),BRect(0,0,0,0),resizingMode,flags | B_FRAME_EVENTS);		
	
	View   = dynamic_cast<BView *>(checkbox);
	Widget = dynamic_cast<SQI_Widget *>(View);	
		
	Variable = var;
	Variable->AddRef();
	Variable->Contained();
	
	if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
	{
		SQI_Object *value = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
		SQI_Number *val = IsNumber(value);
		if(val)
			checkbox->SetValue(Number2Bool(val));	
	}
	else
	{
		squirrel->interpreter->GlobalVar->Set(squirrel,Variable->Data(),squirrel->interpreter->False,NO_SYSTEM_HOOK);
		checkbox->SetValue(false);
	}	
	
	// we need to set up the callback from when the variable is modified
	
	squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,CheckBoxVarHook);	
	
	if(!CheckBoxMethods)
		InitBCheckBox();	
		
	Methods		= CheckBoxMethods;
	Settings 	= CheckBoxSettings;
}

/*
 * function    : SQI_BCheckBox
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
SQI_BCheckBox::SQI_BCheckBox(SQI_Heap *target,const char *label,SQI_Keyword *var,BRect frame,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(target,SQI_BCHECKBOX)
{	
	checkbox = new SQI_CheckBox(NULL,this,label,new BMessage(WAS_INVOKED),frame,resizingMode,flags | B_FRAME_EVENTS);		
	
	View   = dynamic_cast<BView *>(checkbox);
	Widget = dynamic_cast<SQI_Widget *>(View);	

	if(!CheckBoxMethods)
		InitBCheckBox();	
		
	Methods		= CheckBoxMethods;
	Settings 	= CheckBoxSettings;	

	Variable = var;
	Variable->AddRef();
	Variable->Contained();
}

/*
 * function    : SQI_BCheckBox
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BCheckBox::~SQI_BCheckBox()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BCheckBox::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View)
 			{
 				if(Widget->Interpreter->GlobalVar)
 					Widget->Interpreter->GlobalVar->RemoveSystemFunc(Variable->Data(),BIND_SET);
 				if(Window)
 				{	
 					Lock();	
					View->RemoveSelf();
					Unlock();
 				}	
				delete View;
 			}
 			Variable->Alone();
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
void SQI_BCheckBox::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
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
SQI_Object *SQI_BCheckBox::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BCheckBox *clone = new SQI_BCheckBox(hp,checkbox->Label(),VariableClone(Variable),View->Frame(),View->ResizingMode(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		checkbox->Clone(clone->checkbox);
		
		clone->checkbox->Interpreter->GlobalVar->SetSystemFunc(clone->Variable->Data(),clone,BIND_SET,CheckBoxVarHook);

					
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
string *SQI_BCheckBox::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << checkbox->Label() << '\0';
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
string *SQI_BCheckBox::Show(int prec = 3)
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
string *SQI_BCheckBox::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BCHECKBOX[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

// members function


/*
 * function    : SetEnabled
 * purpose     : Set if the widget is enable or disable
 * input       : 
 *
 * bool state, enable or disable
 *
 * output      : none
 * side effect : none
 */
void SQI_BCheckBox::SetEnabled(bool state)
{
	if(View)	
	{
		enabled = state;
		checkbox->SetEnabled(state);	
	}
}

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
SQI_Object *SQI_BCheckBox::Invoke(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		Lock();
		if(checkbox->Value()==B_CONTROL_ON)
			checkbox->SetValue(B_CONTROL_OFF);
		else
			checkbox->SetValue(B_CONTROL_ON);			
		checkbox->Invoked(checkbox,NULL);
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
SQI_Object *SQI_BCheckBox::Label(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
			return new SQI_String(squirrel->LocalHeap,checkbox->Label());	
		else
		if(*str == "set")
		{
			SQI_String *label = IsString((*inputs)[2]);
			if(!label)
				label = IsKeyword((*inputs)[2]);
			
			if(label)
			{
				string *str = label->Data();
				
				checkbox->SetLabel(str->c_str());
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","third input must be a string/word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BCheckBox::Value(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			if(checkbox->Value()==B_CONTROL_ON)
				return new SQI_Number(squirrel->LocalHeap,true);
			else
				return new SQI_Number(squirrel->LocalHeap,false);
		}	
		else
		if(*str == "set")
		{
			SQI_Number *value = IsBool((*inputs)[2]);
		
			if(value)
			{
				if(value->IsTrue())	
				{				
					checkbox->SetValue(B_CONTROL_ON);
					Widget->Interpreter->GlobalVar->Set(squirrel,Variable->Data(),Widget->Interpreter->True,NO_SYSTEM_HOOK);
				}	
				else
				{
					checkbox->SetValue(B_CONTROL_OFF);
					Widget->Interpreter->GlobalVar->Set(squirrel,Variable->Data(),Widget->Interpreter->False,NO_SYSTEM_HOOK);
				}	
		
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BCheckBox::Var(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			return Variable;
		}	
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
				Variable->AddRef();
				Variable->Contained();
				
				if(squirrel->interpreter->GlobalVar->Exist(Variable->Data()))
				{
					SQI_Object *value = squirrel->interpreter->GlobalVar->Get(squirrel,Variable->Data());
					SQI_Number *val = IsNumber(value);
					if(val)
						checkbox->SetValue(Number2Bool(val));	
				}
				else
				{
					SQI_Number *v;
					
					if(checkbox->Value()==B_CONTROL_ON)
						v = squirrel->interpreter->True;
					else
						v = squirrel->interpreter->False;
						 	
					squirrel->interpreter->GlobalVar->Set(squirrel,Variable->Data(),v,NO_SYSTEM_HOOK);
				}	
				
				squirrel->interpreter->GlobalVar->SetSystemFunc(Variable->Data(),this,BIND_SET,CheckBoxVarHook);	
			
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CHECKBOX~config","second input must be the word \"get or \"set"));		
}

// some used function

// Fill up the member map
void InitBCheckBox()
{	
	CheckBoxMethods 	= new MethodTable();
	CheckBoxSettings 	= new MethodTable();

	// specific
	CheckBoxMethods->Add("invoke",(OMeth)(&SQI_BCheckBox::Invoke));
	
	CheckBoxSettings->Add("label",(OMeth)(&SQI_BCheckBox::Label));
	CheckBoxSettings->Add("value",(OMeth)(&SQI_BCheckBox::Value));	
	CheckBoxSettings->Add("variable",(OMeth)(&SQI_BCheckBox::Var));			
}

void UnInitBCheckBox()
{
	if(CheckBoxMethods)
	{
		delete CheckBoxMethods;
		delete CheckBoxSettings;	
	}	
}

void CheckBoxVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel)
{
	SQI_BCheckBox *cb = static_cast<SQI_BCheckBox *>(data);
	SQI_Number *v = IsNumber(value);
	if(v)
	{
		cb->Lock();
		if(v->IsTrue())
		{
			if(!cb->checkbox->Value())
				cb->checkbox->SetValue(true);	
		}
		else
		{
			if(cb->checkbox->Value())
				cb->checkbox->SetValue(false);
		}
		cb->Unlock();	
	}	
}

/*
 * function    : IsCheckBox
 * purpose     : Test if an object is a bcheckbox
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BCheckBox *, the bcheckbox or NULL else
 * side effect : none
 */
SQI_BCheckBox *IsCheckBox(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BCHECKBOX)
    	return dynamic_cast<SQI_BCheckBox *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

