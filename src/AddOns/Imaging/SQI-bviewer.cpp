/*
 * Squirrel project
 *
 * file ?	SQI-bview.cpp
 * what	?  	bview object
 * who	?	jlv
 * when	?	02/06/01
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bviewer.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-viewer.h"
#include "SQI-utils.h"
#include "SQI-font.h"
#include "SQI-scrollview.h"

MethodTable *ViewerMethods = NULL;		// methods hash table
MethodTable *ViewerSettings= NULL;	// settings hash table

  
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
SQI_BViewer::SQI_BViewer(SQI_Squirrel *squirrel,int width,int height,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BVIEWER)
{	
	viewer = new SQI_Viewer(squirrel->interpreter,this,BRect(0,0,width-1,height-1),B_FOLLOW_NONE,flags | B_FRAME_EVENTS);	

	Contain = new ScrollView(this,BRect(0,0,width-1,height-1),viewer,false);

	Image 	= NULL;	
	scroll 	= false;
	
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(Contain);
	Widget = dynamic_cast<SQI_Widget *>(viewer);	
	
	if(!ViewerMethods)
		InitBViewer();		
		
	Methods		= ViewerMethods;
	Settings 	= ViewerSettings;	
}

/*
 * function    : SQI_BView
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * SQI_Image *image, Image to display
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags of the view
 *
 * output      : none
 * side effect : none
 */
SQI_BViewer::SQI_BViewer(SQI_Squirrel *squirrel,SQI_Image *image,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BVIEWER)
{	
	
	viewer = new SQI_Viewer(squirrel->interpreter,this,image->Bounds(),B_FOLLOW_NONE,flags | B_FRAME_EVENTS);	
	viewer->bitmap = image->GetBitmap();
	viewer->pos.x = 0;
	viewer->pos.y = 0;

	Contain = new ScrollView(this,image->Bounds(),viewer,false);

	Image 	= image;	
	scroll 	= false;
	
	Image->AddRef();
	Image->Contained();
	Image->Lock();
	Image->Export(this->heap);	
	
	SetClass(CLASS_BWIDGET);
	
	View   = dynamic_cast<BView *>(Contain);
	Widget = dynamic_cast<SQI_Widget *>(viewer);	
	
	if(!ViewerMethods)
		InitBViewer();		
		
	Methods		= ViewerMethods;
	Settings 	= ViewerSettings;	
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
SQI_BViewer::SQI_BViewer(SQI_Heap *target,BRect frame,uint32 resizingMode,uint32 flags)
	:SQI_BWidget(target,SQI_BVIEWER)
{			
	viewer = new SQI_Viewer(NULL,this,frame,B_FOLLOW_NONE,flags); // | B_FRAME_EVENTS);
	
	Contain = new ScrollView(this,frame,viewer,false);	
		
	SetClass(CLASS_BWIDGET);
	
	Image = NULL;
	scroll 	= false;

	View   = dynamic_cast<BView *>(Contain);
	Widget = dynamic_cast<SQI_Widget *>(viewer);	
	
	if(!ViewerMethods)
		InitBViewer();	
		
	Methods		= ViewerMethods;
	Settings 	= ViewerSettings;
}

/*
 * function    : SQI_BView
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BViewer::~SQI_BViewer()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BViewer::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{ 			
 			if(Image)
 			{
 				Image->Unlock();
 				Image->Alone();
 				Image->RemRef();	
 			}
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
void SQI_BViewer::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);
		if(Image)
			nheap->Import(Image);	
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
SQI_Object *SQI_BViewer::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		if(View->Window())
			View->Window()->Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
			
		SQI_BViewer *clone = new SQI_BViewer(hp,View->Frame(),View->ResizingMode(),View->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		viewer->Clone(clone->viewer);
					
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
string *SQI_BViewer::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "View" << '\0';
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
string *SQI_BViewer::Show(int prec = 3)
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
string *SQI_BViewer::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BVIEWER[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
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
void SQI_BViewer::MakeFocus(bool focused = true)
{
	if(viewer)
	{
		Lock();
		viewer->MakeFocus(focused);
		Unlock();	
	}
}

/*
 * function    : GiveBViewer
 * purpose     : Return the BViewer
 * input       : none
 * output      : BViewer *
 * side effect : none
 */
BView *SQI_BViewer::GiveBView()
{
	return static_cast<BView *>(Contain);	
}

/*
 * function    : TrueBounds
 * purpose     : Return the Bounds of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BViewer::TrueBounds()
{
	BRect b = Contain->Bounds();
	b.right += 2.0 * Contain->padx;
	b.bottom += 2.0 * Contain->pady;	
	return b;
}

/*
 * function    : TrueFrame
 * purpose     : Return the Frame of the view with pading
 * input       : none
 * output      : BRect
 * side effect : none
 */
BRect SQI_BViewer::TrueFrame()
{
	BRect b = Contain->Frame();
	b.InsetBy(-Contain->padx,-Contain->pady);		
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
void SQI_BViewer::MoveTo(float x,float y)
{	
	//cout << "move to " << x+Contain->padx << " " << y+Contain->pady << "\n";
	//Contain->Frame().PrintToStream();
	//cout << Contain->Frame().left << " " << Contain->Frame().top << "\n";
	
	Contain->MoveTo(x+Contain->padx,y+Contain->pady);
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
void SQI_BViewer::MoveBy(float w,float h)
{
	Contain->MoveBy(w,h);
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
void SQI_BViewer::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
				
	Contain->MoveBy(-left,-top);
	
	// now we resize-it
	
	Contain->ResizeBy(right+left,top+bottom);
	
	// we need also to expand the viewer
	
	
}

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it prefered size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BViewer::ResizeToPreferred()
{		
	if(viewer->style == ADAPT)
		Contain->ResizeTo(viewer->original_width,viewer->original_height);
	else
		Contain->ResizeToPreferred();	
}


// members function

bool SQI_BViewer::DisplayWithStyle(string *style)
{
	if(viewer)
		if(viewer->bitmap)
		{
			if(*style == "adapt")
			{
				Lock();
				
				// cancel scroll if needed
				if(viewer->style == SCROLL)
					DoScroll(false);	
				
				viewer->style = ADAPT;
				viewer->original_width = viewer->bitmap->Bounds().IntegerWidth();
				viewer->original_height = viewer->bitmap->Bounds().IntegerHeight();
				viewer->pos.x = 0;
				viewer->pos.y = 0;
				Contain->o_w = viewer->original_width;
				Contain->o_h = viewer->original_height;
				Window->ReGlue(NULL,NULL);
				viewer->Invalidate();
				Unlock();
			}
			else
			if(*style == "center")
			{
				Lock();	
			
				// cancel scroll if needed
				if(viewer->style == SCROLL)
					DoScroll(false);	
				
				viewer->style = CENTER;
				viewer->ComputePos();
				viewer->Invalidate();
				Unlock();							
			}
			else
			if(*style == "scale")
			{
				Lock();
				
				// cancel scroll if needed
				if(viewer->style == SCROLL)
					DoScroll(false);				
				
				viewer->style = SCALE;
				viewer->Invalidate();
				Unlock();	
			}
			else
			if(*style == "pro.scale")
			{
				Lock();
				
				// cancel scroll if needed
				if(viewer->style == SCROLL)
					DoScroll(false);				
				
				viewer->style = PSCALE;
				viewer->Invalidate();
				Unlock();	
			}
			else			
			if(*style == "scroll")
			{
				Lock();
				if(viewer->bitmap->Bounds().Width() > viewer->Bounds().Width() ||
				   viewer->bitmap->Bounds().Height() > viewer->Bounds().Height())
				{  
					viewer->pos.Set(0,0); // set drawing point to the top/left corner of the view
				
					DoScroll(true);	// create scroll bar if needed and display them or update them if needed	  
				}  
				else
				{
					viewer->style = CENTER;
					viewer->ComputePos();
					viewer->Invalidate();					
				}  
				Unlock();	
			}
			else
				return false;
		}
	
	return true;	
}

SQI_Object *SQI_BViewer::Display(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=1)
		{
			SQI_Image *img = IsImage((*inputs)[0]);
			if(img)
			{
				img->Lock();
				img->AddRef();
				img->Contained();
				img->Export(this->heap);
				
				if(Image)
				{
					Image->Unlock();
					Image->Alone();
					Image->RemRef();	
				}	
				
				Image = img;
				viewer->bitmap = img->GetBitmap();
				
				if(inputs->Length()==2)
				{
					SQI_String *style = IsKeyword((*inputs)[1]);
					if(!style)
						style = IsString((*inputs)[1]);		
					
					if(style)
					{
						string *str = style->Data();
						
						if(!DisplayWithStyle(str))
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VIEWER~display","second input must be the word : \"adapt or \"center"));	
					}
					else	
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VIEWER~display","second input must be a word"));
				}
				else
				{
					Lock();	
					viewer->style = CENTER;
					viewer->ComputePos();
					viewer->Invalidate();
					Unlock();
				}	
				
				return NULL;	
			}
			else
			{
				SQI_String *style = IsKeyword((*inputs)[0]);
				if(!style)
					style = IsString((*inputs)[1]);	
										
				if(style)
				{
					string *str = style->Data();
					if(!DisplayWithStyle(str))
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VIEWER~display","input must be the word : \"adapt or \"center"));	
					
					return NULL;	
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"VIEWER~display","input must be an image or a word"));
			}	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VIEWER~display","need an input at least"));					
	}	
}

SQI_Object *SQI_BViewer::ResizeTo(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()==1)
		{
			SQI_List *lst = IsList((*inputs)[0]);
			if(lst)
			{
				float w,h;
				List2Pad(lst,&w,&h);
				
				Lock();
				Contain->o_w = w;
				Contain->o_h = h;
				Window->ReGlue(NULL,NULL);
				viewer->Invalidate();
				Unlock();	
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VIEWER~resize.to","input must be a list"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"VIEWER~resize.to","need an input"));					
	}	
}

// settings function

// other methods

void SQI_BViewer::DoScroll(bool s)
{		
	if(s)
	{
		viewer->style = SCROLL;
		// resize the viewer to the need size to fit the image
		viewer->pos.x = 0;
		viewer->pos.y = 0;
		viewer->ResizeTo(viewer->bitmap->Bounds().Width(),viewer->bitmap->Bounds().Height());			
	}
	
	// call the scroll method of the ScrollView widget
	Contain->Scroll(s);	
}

// some used function

// Fill up the member map
void InitBViewer()
{
	ViewerMethods 	= new MethodTable;	
	
	ViewerMethods->Add("display",(OMeth)(&SQI_BViewer::Display));
	ViewerMethods->Add("resize.to",(OMeth)(&SQI_BViewer::ResizeTo));
}

void UnInitBViewer()
{
	if(ViewerMethods)
		delete ViewerMethods;
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
SQI_BViewer *IsViewer(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BVIEWER)
    	return dynamic_cast<SQI_BViewer *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
