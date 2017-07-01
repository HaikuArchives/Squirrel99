/*
 * Squirrel project
 *
 * file ?	SQI-simplelist.cpp
 * what	?   simplelist object
 * who	?	jlv
 * when	?	12/04/99
 * last	?	02/15/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bsimplelist.h"
#include "SQI-glue.h"
#include "SQI-simplelist.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_SimpleList
 * purpose     : Constructor
 * input       : 
 *
 * BRect frame, view frame
 * uint32 resizingMode, resizing mode
 * uint32 flags, flags
 *
 * output      : none
 * side effect : none
 */
SQI_SimpleList::SQI_SimpleList(SQI_Interp *interp,SQI_BSimpleList *ptr,int cw,int l,BRect frame,list_view_type type,uint32 flags)
	:BListView(frame,NULL,type,B_FOLLOW_NONE) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	
	Chars = cw;
	Lines = l;
		
	SetInvocationMessage(new BMessage(WAS_INVOKED));
	SetSelectionMessage(new BMessage(WAS_SELECTED)); 
}


/*
 * function    : ~SQI_SimpleList
 * purpose     : SimpleList destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_SimpleList::~SQI_SimpleList()
{
	MagicPtr->List = NULL;
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::AttachedToWindow(void)
{
	BListView::AttachedToWindow();	
	
	// the view is attached to a Window, we set some variable	
		
	BWindow *win = Window();	
	SQI_Window *w = static_cast<SQI_Window *>(win);	
	SQI_BWindow *bw = w->MagicPtr;
		
	MagicPtr->Window = bw;
	Squirrel = bw->Squirrel; 	
			
	if(Parent())
	{
		if(!font_set)
		{	
			BFont font;
			Parent()->GetFont(&font);
			SetFont(&font);	
		}	
	}
	else
	{
		if(!font_set)
			SetFont(&(bw->Font));				
	} 
	
	
	// did the variable have a value ?
	
	SQI_Object *value = NULL;
	
	if(Interpreter->GlobalVar->Exist(MagicPtr->Variable->Data()))
		value = Interpreter->GlobalVar->Get(Squirrel,MagicPtr->Variable->Data());
	
	AddItems(MagicPtr->Items,value);
			
	// we set the target to the Window	
		
	SetTarget(this,NULL);
					
	if(Hooks[HOOK_WIDGET_ATTACHEDTOWINDOW].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();	
		
		BWindow *parent = Window();
		SQI_Window *win = dynamic_cast<SQI_Window *>(parent); 
		args->push_back(win->MagicPtr);
		win->MagicPtr->AddRef();
			
		AddExtra(args,Hooks[HOOK_WIDGET_ATTACHEDTOWINDOW].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIDGET_ATTACHEDTOWINDOW].action,args);
				
		delete args;
	}			
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::AllAttached(void)
{		
	// time to compute the size of the view
	
	ResizeToPreferred();
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();

	BRect f = Frame();
		
	if(MagicPtr->Scroll)
	{
		switch(MagicPtr->scroll_type)
		{
			case SCROLL_RIGHT:
			{
				MagicPtr->Scroll->MoveTo(f.right+1,f.top);
				break;	
			}	
			case SCROLL_LEFT:
			{
				MagicPtr->List->MoveTo(MagicPtr->Scroll->Frame().right+1,MagicPtr->Scroll->Frame().top);	
				break;	
			}
		}
		
		MagicPtr->Scroll->ResizeTo(14,f.Height());
	}	
	
	BListView::AllAttached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);			
	BListView::DetachedFromWindow();	
}

/*
 * function    : Draw
 * purpose     : Called when the view need to be redraw
 * input       :
 *
 * BRect updateRect, update rect
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);		
	BListView::Draw(updateRect);			
}

/*
 * function    : FrameMoved
 * purpose     : Called when the view position have changed
 * input       :
 *
 * BPoint parentPoint
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::FrameMoved(BPoint parentPoint)
{
	SQI_Widget::FrameMoved(this,parentPoint);		
	BListView::FrameMoved(parentPoint);	
}

/*
 * function    : FrameResized
 * purpose     : Called when the view size has changed
 * input       :
 *
 * float width, new width
 * float height, new height
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::FrameResized(float width,float height)
{
	SQI_Widget::FrameResized(this,width,height);			
	BListView::FrameResized(width,height);	
}

/*
 * function    : KeyDown
 * purpose     : Called when the user press a key in the view
 * input       : 
 *
 * const char *bytes, character code
 * int32 numBytes, ??
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))					
		BListView::KeyDown(bytes,numBytes);
}

/*
 * function    : KeyUp
 * purpose     : Called when the user release a key in the view
 * input       : 
 *
 * const char *bytes, character code
 * int32 numBytes, ??
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))		
		BListView::KeyUp(bytes,numBytes);
}

/*
 * function    : MakeFocus
 * purpose     : The focus is set to the view , or removed from the view
 * input       : 
 *
 * bool focused, true if we have the focus, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BListView::MakeFocus(focused);		
}

/*
 * function    : Widget Selected
 * purpose     : we execute the hook (if any)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::Selected(BView *view,BMessage *message)
{
	if(Hooks[HOOK_WIDGET_SELECTED].action)
	{				
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Object *index;
		SQI_Object *value;
		
		GetSelection(message,&index,&value);
		
		if(value)
		{
			Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),value,NO_SYSTEM_HOOK);	
			
			index->AddRef();
			value->AddRef();	
			
			args->push_back(index);
			args->push_back(value);
		
			AddExtra(args,Hooks[HOOK_WIDGET_SELECTED].extra);	
			
			UseHook(Hooks[HOOK_WIDGET_SELECTED].action,args,true);
		}
	}
	else
	{
		SQI_Object *index;
		SQI_Object *value;
						
		GetSelection(message,&index,&value);
		
		if(value)
		{					
			Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),value,NO_SYSTEM_HOOK);		
			index->Suicidal();
		}					
	}	
}

/*
 * function    : Widget Invoked
 * purpose     : we execute the hook (if any)
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::Invoked(BView *view,BMessage *message = NULL)
{
	if(Hooks[HOOK_WIDGET_INVOKED].action)
	{				
		// we need to create a list of value for the Hook which contain the input of the function	
	
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
		MagicPtr->AddRef();
		
		SQI_Object *index;
		SQI_Object *value;
		
		GetSelection(message,&index,&value);
		
		if(value)
		{	
			Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),value,NO_SYSTEM_HOOK);	
			
			index->AddRef();
			value->AddRef();	
			
			args->push_back(index);
			args->push_back(value);
		
			AddExtra(args,Hooks[HOOK_WIDGET_INVOKED].extra);	
			
			UseHook(Hooks[HOOK_WIDGET_INVOKED].action,args,true);
		}
	}		
	else
	{
		SQI_Object *index;
		SQI_Object *value;
					
		GetSelection(message,&index,&value);
			
		if(value)
		{
			Interpreter->GlobalVar->Set(Squirrel,MagicPtr->Variable->Data(),value,NO_SYSTEM_HOOK);
			index->Suicidal();
		}					
	}		
}


/*
 * function    : MessageReceived
 * purpose     : We receive a message
 * input       : 
 *
 * BMessage *message, message received
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::MessageReceived(BMessage *message)
{	
	if(MagicPtr->enabled)
		if(!SQI_Widget::MessageReceived(this,message))
			BListView::MessageReceived(message);
}


/*
 * function    : MouseDown
 * purpose     : A button of the mouse has been clicked
 * input       : 
 *
 * BPoint point, position of the mouse
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
			BListView::MouseDown(point);			
}

/*
 * function    : MouseMoved
 * purpose     : Enter or exit the view
 * input       :
 *
 * BPoint point, where we have entered or exited
 * uint32 transit, what's happening
 * const BMessage *message, message
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);								
	BListView::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::Pulse(void)
{
	SQI_Widget::Pulse(this);	
	BListView::Pulse();
} 

/*
 * function    : WindowActivated
 * purpose     : Executed when the window is activated or deactivated
 * input       : 
 *
 * bool active, trie if active, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);		
	BListView::WindowActivated(active);
} 

/*
 * function    : GetPreferredSize
 * purpose     : Get the Preferred size of the view
 * input       : 
 *
 * float *width, preferred width
 * float *height, preferred height
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::GetPreferredSize(float *width,float *height)
{
	if(Chars>0)
	{
		string s(Chars,'X');		
		
		*width = StringWidth(s.c_str());
	}
	else
	{
		// the preferred width of the widget is the width of the
		// longer items
		
		const BListItem **ptr = Items();
		const BStringItem *item;
		float max = 0;
		float w;	
		for(long i=0;i<CountItems();i++)
		{
			item = dynamic_cast<const BStringItem *>(ptr[i]);
			w = StringWidth(item->Text());
			if(w>max)
				max = w;
		}
				
		*width = max + 3;
	}
	
	if(Lines>0)
	{	
		font_height h;
		GetFontHeight(&h);
		
		*height = Lines * (ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1) - 1;			
	}	
	else
	{
		font_height h;
		GetFontHeight(&h);
		
		*height = CountItems() * (ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1) - 1;
	}			
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned view to be the same
 * input       : 
 *
 * SQI_SimpleList *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::Clone(SQI_SimpleList *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a SimpleList
	
	clone->RealBounds = RealBounds;
	clone->original_width = original_width;
	clone->original_height = original_height;

	// the font used
	
	BFont font;
	GetFont(&font);					
	clone->SetFont(&font);
	
	// Drawing mode		
	
	clone->SetDrawingMode(DrawingMode());
	
	// color
	
	clone->SetHighColor(HighColor());
	clone->SetLowColor(LowColor());
	clone->SetViewColor(ViewColor());
	
	// size
	
	clone->Chars = Chars;
	clone->Lines = Lines;
	
	// that's all	
}

// some usefull methods	

/*
 * function    : AddItems
 * purpose     : Add all the element of the list given in the simplelist
 * input       : 
 *
 * SQI_List *items, list to add
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::AddItems(SQI_List *items,SQI_Object *init = NULL)
{
	string *str;
	BStringItem *item;
	list<SQI_Object *>::const_iterator *j = items->Iterator();
    list<SQI_Object *>::const_iterator *e = items->End();
  
 	// if there's allready some items, we remove them all
  	
 	RemoveItems(0,CountItems()); 
 	
 	if(MagicPtr->enabled)
 	{ 
	  	if(init)
	  	{
	  		SQI_List *linit = IsList(init);
	  		if(linit)
	  		{
	  			for(int i=0;*j!=*e;(*j)++,i++)
			    {
					str = (*(*j))->Print();
					item = new BStringItem(str->c_str());
					if(linit->Find((*(*j))))
					{
			    		item->Select();
			    		AddItem(item);
			    		Select(i,true);
					}
					else
						AddItem(item);
					delete str;
			    }		
	  		}
	  		else
	  		{	
			    for(int i=0;*j!=*e;(*j)++,i++)
			    {
					str = (*(*j))->Print();
					item = new BStringItem(str->c_str());
					if(*(*(*j))==init)
					{
			    		item->Select();
			    		AddItem(item);
			    		Select(i);
					}
					else
						AddItem(item);
					delete str;
			    }
	  		}
	  	}
	  	else
	  	{
	 	    for(j;*j!=*e;(*j)++)
		    {
				str = (*(*j))->Print();
				item = new BStringItem(str->c_str());
				AddItem(item);
				delete str;
		    }
	 		
	  	}
 	}
 	else
 	{
 	  	if(init)
	  	{
	  		SQI_List *linit = IsList(init);
	  		if(linit)
	  		{
	  			for(int i=0;*j!=*e;(*j)++,i++)
			    {
					str = (*(*j))->Print();
					item = new BStringItem(str->c_str());
					item->SetEnabled(false);
					if(linit->Find((*(*j))))
					{
			    		item->Select();
			    		AddItem(item);
			    		Select(i,true);
					}
					else
						AddItem(item);
					delete str;
			    }		
	  		}
	  		else
	  		{	
			    for(int i=0;*j!=*e;(*j)++,i++)
			    {
					str = (*(*j))->Print();
					item = new BStringItem(str->c_str());
					item->SetEnabled(false);
					if(*(*(*j))==init)
					{
			    		item->Select();
			    		AddItem(item);
			    		Select(i);
					}
					else
						AddItem(item);
					delete str;
			    }
	  		}
	  	}
	  	else
	  	{
	 	    for(j;*j!=*e;(*j)++)
		    {
				str = (*(*j))->Print();
				item = new BStringItem(str->c_str());
				item->SetEnabled(false);
				AddItem(item);
				delete str;
		    }
	 		
	  	}		
 	}
  	
  	ScrollToSelection();
              
    delete j;
	delete e;	
}

/*
 * function    : GetSelection
 * purpose     : Get the current selection
 * input       : 
 *
 * BMessage *message, the message
 * SQI_Object **index, object wich will contain the index od the selection
 * SQI_Object **value, object which will contain the value selected
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::GetSelection(BMessage *message,SQI_Object **index,SQI_Object **value)
{
	SQI_Object *ind;
	SQI_Object *val;
	int32 nb;
	type_code type;
		
	message->GetInfo("index",&type,&nb);
			
	if(nb==1) 
	{
		int32 i = message->FindInt32("index");
		ind = new SQI_Number(Squirrel->LocalHeap,i+1);
		val = (*(MagicPtr->Items))[i];		
	}
	else
	{
		SQI_List *lind = new SQI_List(Squirrel->LocalHeap);
		SQI_List *lval = new SQI_List(Squirrel->LocalHeap);
		int32 num;
		for(int i=0;i<nb;i++)
		{
			num = message->FindInt32("index",i);
			lind->Add2End(new SQI_Number(Squirrel->LocalHeap,num+1));
			lval->Add2End((*(MagicPtr->Items))[num]);
		}	
		
		ind = lind;
		val = lval;		
	}		
	
	*index = ind;
	*value = val;
}

/*
 * function    : AddItem
 * purpose     : Add an object to the list
 * input       : 
 *
 * SQI_Object *item, object to add
 * int32 index, position to insert the item
 *
 * output      : none
 * side effect : none
 */
void SQI_SimpleList::AddAnItem(SQI_Object *item,int32 index = -1)
{
	string *str = item->Print();
	BStringItem *i = new BStringItem(str->c_str());
	if(!MagicPtr->enabled)
		i->SetEnabled(false);
	if(index!=-1)
		AddItem(i,index);
	else
		AddItem(i);
	delete str;		
}

