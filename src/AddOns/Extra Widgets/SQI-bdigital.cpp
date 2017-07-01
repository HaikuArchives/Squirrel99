/*
 * Squirrel project
 *
 * file ?	SQI-bdigital.cpp
 * what	?  	bdigital object
 * who	?	jlv
 * when	?	02/24/01
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <File.h>
#include <Resources.h> 
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bdigital.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-digital.h"
#include "SQI-utils.h"
#include "SQI-font.h"

MethodTable *DigitalMethods = NULL;		// methods hash table
MethodTable *DigitalSettings= NULL;	// settings hash table

#include <Bitmap.h>

BBitmap *BitmapDigits[13] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL	
};

  
/*
 * function    : SQI_BView
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
SQI_BDigital::SQI_BDigital(SQI_Squirrel *squirrel,int digit,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BDIGITAL)
{	
	digital = new SQI_Digital(squirrel->interpreter,this,digit,flags);	
	
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(digital);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!DigitalMethods)
		InitBDigital();		
		
	Methods		= DigitalMethods;
	Settings 	= DigitalSettings;	
}


/*
 * function    : SQI_BView
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
SQI_BDigital::SQI_BDigital(SQI_Heap *target,int digit,uint32 flags)
	:SQI_BWidget(target,SQI_BDIGITAL)
{			
	digital = new SQI_Digital(NULL,this,digit,flags);	
	
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(digital);
	Widget = dynamic_cast<SQI_Widget *>(View);	
	
	if(!DigitalMethods)
		InitBDigital();		
		
	Methods		= DigitalMethods;
	Settings 	= DigitalSettings;
}

/*
 * function    : SQI_BView
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BDigital::~SQI_BDigital()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BDigital::Suicidal(bool force=false)
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
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_BDigital::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BDigital *clone = new SQI_BDigital(hp,digital->Digit,View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		digital->Clone(clone->digital);
					
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
string *SQI_BDigital::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Digital" << '\0';
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
string *SQI_BDigital::Show(int prec = 3)
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
string *SQI_BDigital::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BDIGITAL[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
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
void SQI_BDigital::MakeFocus(bool focused = true)
{
	if(digital)
	{
		Lock();
		digital->MakeFocus(focused);
		Unlock();	
	}
}

/*
 * function    : GiveBDigital
 * purpose     : Return the BDigital
 * input       : none
 * output      : BDigital *
 * side effect : none
 */
BView *SQI_BDigital::GiveBView()
{
	return static_cast<BView *>(digital);	
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
void SQI_BDigital::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
			
	digital->MoveBy(-left,-top);
	
	// no resize	
}


SQI_Object *SQI_BDigital::Display(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_Number *num = IsNumber((*inputs)[0]);	
			
			if(num)
			{	
				Lock();
				digital->Display(num);
				Unlock();
				return NULL;
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DIGITAL~display","input must be a number"));		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DIGITAL~display","need an input at least"));					
	}	
}

SQI_Object *SQI_BDigital::Precision(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_Number *num = IsNumber((*inputs)[0]);	
			
			if(num)
			{	
				Lock();
				digital->Precision = Number2Int(num);
				Unlock();
				return NULL;
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DIGITAL~precision","input must be a number"));		
		}
		else
			return new SQI_Number(squirrel->LocalHeap,digital->Precision);					
	}	
}

// settings function

// some used function

bool GetDigits()
{
	BFile file("/boot/home/config/add-ons/Squirrel/Widgets.so",B_READ_ONLY);
	BResources res;
	
	if(file.InitCheck()==B_NO_ERROR)
	{
		if(res.SetTo(&file,false) == B_NO_ERROR)
		{
			char *data;
			size_t size;
			BMessage *msg;	
				
			// get  all the icons
			for(int32 i=0;i<13;i++)
			{	
				if((data = (char *)res.FindResource('BBMP',i,&size))!=NULL)
				{
					msg = new BMessage();
					
					if(msg->Unflatten(data)==B_OK)	
						BitmapDigits[12-i] = new BBitmap(msg);	
							
					delete msg;	
					delete data;	
				}
			}
				
			return true;
		}
		else
			return false;
	}
	else
		return false;		
}

void DelDigits()
{
	for(int i=0;i<13;i++)
		if(BitmapDigits[i])
			delete BitmapDigits[i];	
}

// Fill up the member map
void InitBDigital()
{
	DigitalMethods	= new MethodTable;	
	
	DigitalMethods->Add("display",(OMeth)(&SQI_BDigital::Display));
	DigitalMethods->Add("precision",(OMeth)(&SQI_BDigital::Precision));
	
	GetDigits();
}

void UnInitBDigital()
{
	if(DigitalMethods)
		delete DigitalMethods;

	DelDigits();	
}

/*
 * function    : IsView
 * purpose     : Test if an object is a bview
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BView *, the bview or NULL else
 * side effect : none
 */
SQI_BDigital *IsDigital(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BDIGITAL)
    	return dynamic_cast<SQI_BDigital *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
