/*
 * Squirrel project
 *
 * file ?	SQI-bmemo.cpp
 * what	?  	bmemo object
 * who	?	jlv
 * when	?	02/20/00
 * last	?	09/17/00
 *
 *
 * (c) electron Technology 1999
 */
   
#include "ScrollView.h"

#include "File.h"   
   
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-container.h"
#include "SQI-bmemo.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h"
#include "SQI-memo.h"
#include "SQI-utils.h"
#include "SQI-font.h"

BMemoMembers MemoSettingMap;
BMemoMembers MemoMemberMap;
  
/*
 * function    : SQI_BMemo
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
SQI_BMemo::SQI_BMemo(SQI_Squirrel *squirrel,char ver,char hor,int width,int height,uint32 flags)
	:SQI_BWidget(squirrel,SQI_BMEMO)
{	
	View = new Container(this,BRect(0,0,0,0),B_FOLLOW_NONE,B_WILL_DRAW | B_NAVIGABLE);

	Text = new SQI_Memo(squirrel->interpreter,this,width,height,BRect(0,0,0,0),flags);	

	positionv = ver;
	positionh = hor;
	
	if(positionv != SCROLL_NONE)
	{
		ScrollV = new BScrollBar(BRect(0,0,B_V_SCROLL_BAR_WIDTH,100),NULL,Text,0,0,B_VERTICAL);
		ScrollV->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(ScrollV);
	}
	else
		ScrollV = NULL;
		
	if(positionh != SCROLL_NONE)
	{
		ScrollH = new BScrollBar(BRect(0,0,B_V_SCROLL_BAR_WIDTH,B_H_SCROLL_BAR_HEIGHT),NULL,Text,0,0,B_HORIZONTAL);
		ScrollH->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(ScrollH);
	}
	else
		ScrollH = NULL;			
		
	View->AddChild(Text);	
			
	Window = NULL;
	Members = &MemoMemberMap;
	Settings = &MemoSettingMap;	
	Contents = NULL;
}

/*
 * function    : SQI_BMemo
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
SQI_BMemo::SQI_BMemo(SQI_Heap *target,char ver,char hor,int width,int height,BRect frame,uint32 flags)
	:SQI_BWidget(target,SQI_BMEMO)
{	
	View = new Container(this,frame,B_FOLLOW_NONE,B_WILL_DRAW);	
	
	Text = new SQI_Memo(NULL,this,width,height,frame,flags);
	
	positionv = ver;
	positionh = hor;
	
	if(positionv != SCROLL_NONE)
	{
		ScrollV = new BScrollBar(BRect(0,0,B_V_SCROLL_BAR_WIDTH,100),NULL,Text,0,0,B_VERTICAL);
		ScrollV->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(ScrollV);
	}
	else
		ScrollV = NULL;
		
	if(positionh != SCROLL_NONE)
	{
		ScrollH = new BScrollBar(BRect(0,0,B_V_SCROLL_BAR_WIDTH,B_H_SCROLL_BAR_HEIGHT),NULL,Text,0,0,B_HORIZONTAL);
		ScrollH->SetResizingMode(B_FOLLOW_NONE);
		View->AddChild(ScrollH);
	}
	else
		ScrollH = NULL;	
		
	View->AddChild(Text);
				
	Window = NULL;
	Members = &MemoMemberMap;
	Settings = &MemoSettingMap;	
	Contents = NULL;
}

/*
 * function    : SQI_BMemo
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BMemo::~SQI_BMemo()
{			
}

/*
 * function    : Suicidal
 * purpose     : The object is no longer used , so we delete him
 * input       : none
 * output      : none
 * side effect : delete all the object depending on it
 */
bool SQI_BMemo::Suicidal(bool force=false)
{	
	if(status || force)
 		if(!REF)
 		{
 			if(View)
 			{
 				if(Window)
 				{
 					Lock();
 					Text->RemoveSelf();
					View->RemoveSelf();
					if(ScrollH) ScrollH->RemoveSelf();
					if(ScrollV) ScrollV->RemoveSelf();
					Unlock();
 				}
				if(ScrollH) delete ScrollH;
				if(ScrollV) delete ScrollV;
				delete View;
				delete Text;
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
void SQI_BMemo::Export(SQI_Heap *nheap,char force = SQI_ETERNAL)
{
	if(status || force)
	{
		nheap->Import(this);	
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
SQI_Object *SQI_BMemo::Clone(SQI_Heap *target = NULL)
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
SQI_Object *SQI_BMemo::DeepClone(SQI_Heap *target = NULL)
{
	if(View)
	{
		Lock();	
		
		SQI_Heap *hp = heap;
		if(!target)	
			hp = target;
						
		SQI_BMemo *clone = new SQI_BMemo(hp,positionv,positionh,0,0,View->Frame(),Text->Flags());
		
		// now we need to "clone" the view itself (set the same way)
		
		Text->Clone(clone->Text);
							
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
SQI_Object *SQI_BMemo::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_BMemo::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
string *SQI_BMemo::Print(int prec = 3)
{	
	ostrstream out;
  	string *str;
  		
  	if(View)	
		out << "Memo" << '\0';
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
string *SQI_BMemo::Show(int prec = 3)
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
string *SQI_BMemo::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "BMEMO[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
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
bool SQI_BMemo::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	if(View)
		return Text->Hook(event,action,extra);
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
bool SQI_BMemo::UnHook(SQI_Keyword *event)
{
	if(View)
		return Text->UnHook(event);
	else
		return false;
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
void SQI_BMemo::MakeFocus(bool focused = true)
{
	if(Text)
	{
		Lock();
		Text->MakeFocus(focused);
		Unlock();	
	}
}

/*
 * function    : GiveBMemo
 * purpose     : Return the BMemo
 * input       : none
 * output      : BMemo *
 * side effect : none
 */
BView *SQI_BMemo::GiveBView()
{
	return static_cast<BView *>(View);	
}

/*
 * function    : GiveWidget
 * purpose     : Return the SQI_Widget
 * input       : none
 * output      : BMemo *
 * side effect : none
 */
SQI_Widget *SQI_BMemo::GiveWidget()
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
BRect SQI_BMemo::TrueBounds()
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
BRect SQI_BMemo::TrueFrame()
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
void SQI_BMemo::MoveTo(float x,float y)
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
void SQI_BMemo::MoveBy(float w,float h)
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
double SQI_BMemo::Area()
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
void SQI_BMemo::Expand(float left,float top,float right,float bottom)
{
	// first we move the view to the new position
			
	View->MoveBy(-left,-top);
	
	// now we resize-it
	
	View->ResizeBy(right+left,top+bottom);
		
	// we need also to expand the text and the scrollbars
	
	Text->ResizeBy(right+left,top+bottom);
	BRect trec = Text->Bounds();
	//Text->SetTextRect(trec);
	Text->MoveTo(0,0);
	
	if(ScrollV && ScrollH)
	{
		switch(positionv)
		{
			case SCROLL_RIGHT:
			{
				ScrollV->MoveTo(Text->Frame().right+1,Text->Frame().top);
				break;	
			}	
			case SCROLL_LEFT:
			{
				Text->MoveTo(ScrollV->Frame().right+1,ScrollV->Frame().top);	
				break;	
			}
		}
		
		ScrollV->ResizeTo(B_V_SCROLL_BAR_WIDTH,Text->Frame().Height());
		
		switch(positionh)
		{
			case SCROLL_TOP:
			{
				ScrollH->MoveTo(Text->Frame().left,0);
				Text->MoveBy(0,B_H_SCROLL_BAR_HEIGHT+1);
				ScrollV->MoveBy(0,B_H_SCROLL_BAR_HEIGHT+1);
				break;	
			}	
			case SCROLL_BOTTOM:
			{
				ScrollH->MoveTo(Text->Frame().left,Text->Frame().bottom + 1);	
				break;	
			}
		}
		
		ScrollH->ResizeTo(Text->Frame().Width(),B_H_SCROLL_BAR_HEIGHT);		
	}
	else
	if(ScrollV)
	{
		switch(positionv)
		{
			case SCROLL_RIGHT:
			{
				ScrollV->MoveTo(Text->Frame().right+1,Text->Frame().top);
				break;	
			}	
			case SCROLL_LEFT:
			{
				Text->MoveTo(ScrollV->Frame().right+1,ScrollV->Frame().top);	
				break;	
			}
		}
		
		ScrollV->ResizeTo(B_V_SCROLL_BAR_WIDTH,Text->Frame().Height());				
	}
	else
	if(ScrollH)
	{
		switch(positionh)
		{
			case SCROLL_TOP:
			{
				ScrollH->MoveTo(Text->Frame().left,0);
				Text->MoveBy(0,B_H_SCROLL_BAR_HEIGHT+1);
				break;	
			}	
			case SCROLL_BOTTOM:
			{
				ScrollH->MoveTo(Text->Frame().left,Text->Frame().bottom + 1);	
				break;	
			}
		}
		
		ScrollH->ResizeTo(Text->Frame().Width(),B_H_SCROLL_BAR_HEIGHT);		
	}	
	
	
	/*

	if(ScrollV && !ScrollH)
	{
		ScrollV->ResizeTo(B_V_SCROLL_BAR_WIDTH,View->Bounds().bottom);
		Text->ResizeTo(View->Bounds().Width()-B_V_SCROLL_BAR_WIDTH,View->Bounds().Height());
		switch(positionv)
		{
			case SCROLL_RIGHT:
			{
				ScrollV->MoveTo(Text->Bounds().right+1,0);
				break;	
			}	
			case SCROLL_LEFT:
			{
				Text->MoveTo(ScrollV->Bounds().right+1,0);
				break;	
			}
		}
	}
	if(ScrollV && ScrollH)
	{
		ScrollV->ResizeTo(B_V_SCROLL_BAR_WIDTH,View->Bounds().bottom);
		ScrollV->ResizeTo(View->Bounds().right,B_H_SCROLL_BAR_HEIGHT);
		Text->ResizeTo(View->Bounds().Width()-B_V_SCROLL_BAR_WIDTH,View->Bounds().Height()-B_H_SCROLL_BAR_HEIGHT);		
	}
	else
	if(ScrollH && !ScrollV)
	{
		
	}
	else
		Text->ResizeTo(View->Bounds().Width(),View->Bounds().Height());
		
	*/			
}

/*
 * function    : ResizeToPreferred
 * purpose     : Resize the view to it prefered size
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_BMemo::ResizeToPreferred()
{
	Text->ResizeToPreferred();
	// we need to set the scroll to the correct size now
	BRect f = Text->Frame();
		
	if(ScrollV)
	{
		switch(positionv)
		{
			case SCROLL_RIGHT:
			{	
				ScrollV->MoveTo(f.right+1,f.top);
				break;
			}
			case SCROLL_LEFT:
			{
				Text->MoveTo(ScrollV->Frame().right+1,ScrollV->Frame().top);
				break;	
			}
		}	
	
		ScrollV->ResizeTo(B_V_SCROLL_BAR_WIDTH,f.Height());
	}	
	
	if(ScrollH)
	{
		switch(positionh)
		{
			case SCROLL_TOP:
			{	
				ScrollH->MoveTo(Text->Frame().left,0);
				Text->MoveTo(Text->Frame().left,B_H_SCROLL_BAR_HEIGHT+1);
				break;
			}
			case SCROLL_BOTTOM:
			{
				ScrollH->MoveTo(Text->Frame().left,Text->Frame().bottom+1);
				break;	
			}
		}	
	
		ScrollH->ResizeTo(f.Width(),B_H_SCROLL_BAR_HEIGHT);		
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
void SQI_BMemo::SetEnabled(bool state)
{	
	if(View)
	{
		Text->MakeEditable(state);	
	}	
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
SQI_Object *SQI_BMemo::Config(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()>=2)
		{
			SQI_Keyword *property = IsKeyword((*inputs)[0]);
			
			if(property)
			{
				string *key = property->Data();
					
				SQI_Object *(SQI_BMemo::*Setting)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Memo~config","first input must be a word"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Memo~config","need 2 arguments at least"));	
	}
	
	return NULL;	
}

/*
 * function    : Text
 * purpose     : Set/Get the text displayed in the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::TheText(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_String *s;
			
			if(inputs->Length()==2)
			{	
				SQI_List *lst = IsList((*inputs)[1]);
				if(lst)
				{
					SQI_Number *from = IsInteger((*lst)[0]);	
					SQI_Number *to = IsInteger((*lst)[1]);
					if(from && to)
					{
						int32 f,t;
						char *buffer;
						
						f = min(Number2Long(from),Number2Long(to));
						t = max(Number2Long(from),Number2Long(to));
						
						buffer = (char *)malloc(sizeof(char)*(t-f+2));	
							
						Text->GetText(f,t-f+1,buffer);	
						
						s = squirrel->LocalHeap->String(buffer);
											
						free(buffer);	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","when specified the offset must be a list of 2 numbers"));		
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","when specified the offset must be a list or 2 numbers"));
			}
			else
			if(inputs->Length()==3)
			{
				SQI_Number *from = IsInteger((*inputs)[1]);	
				SQI_Number *to = IsInteger((*inputs)[2]);
				if(from && to)
				{
					int32 f,t;
					char *buffer;
					
					f = min(Number2Long(from),Number2Long(to));
					t = max(Number2Long(from),Number2Long(to));
					
					buffer = (char *)malloc(sizeof(char)*(t-f+2));	
					
					Text->GetText(f,t-f+1,buffer);	
					
					s = squirrel->LocalHeap->String(buffer);		
				
					free(buffer);		
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","when specified the offset must be a list or 2 numbers"));					
			}
			else
				s = squirrel->LocalHeap->String(Text->Text());
			
			return s;
		}	
		else
		if(*str == "set")
		{
			SQI_String *str = IsString((*inputs)[1]);
			
			if(str)
			{
				//squirrel->interpreter->GlobalVar->Set(squirrel,Variable->Data(),str,NO_SYSTEM_HOOK);
				Text->SetText(str->Data()->c_str());
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","third input must be a string"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~text","second input must be the word \"get or \"set"));		
}



/*
 * function    : Allow
 * purpose     : Allow a set of char
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Allow(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *word;
	SQI_String *str;
	const char *ch;	
	int j,k;
	
	for(int i=0;i<inputs->Length();i++)
	{
		word = IsKeyword((*inputs)[i]);
		if(word)
		{
			ch = word->Data()->c_str();
			for(j=0;j<strlen(ch);j++)
				Text->AllowChar(ch[j]); 	
		}
		else
		{
			str = IsString((*inputs)[i]);
			if(str)
			{
				ch = str->Data()->c_str();
				for(j=0;j<strlen(ch);j++)
					Text->AllowChar(ch[j]);						
			}	
		}		
	}	
	
	return NULL;
}

/*
 * function    : Disallow
 * purpose     : Disallow a set of char
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Disallow(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *word;
	SQI_String *str;
	const char *ch;	
	int j;
	
	for(int i=0;i<inputs->Length();i++)
	{
		word = IsKeyword((*inputs)[i]);
		if(word)
		{
			ch = word->Data()->c_str();
			for(j=0;j<strlen(ch);j++)
				Text->DisallowChar(ch[j]); 	
		}
		else
		{
			str = IsString((*inputs)[i]);
			if(str)
			{
				ch = str->Data()->c_str();
				for(j=0;j<strlen(ch);j++)
					Text->DisallowChar(ch[j]);						
			}	
		}		
	}	
	
	return NULL;
}

/*
 * function    : AllowAll
 * purpose     : Allow all the chars
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::AllowAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	uint32 c;	
	
	if(View)
	{
		for(c=0;c<256;c++)
			Text->AllowChar(c);
	}	
	
	return NULL;		
}

/*
 * function    : DisallowAll
 * purpose     : Disallow all char
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::DisallowAll(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	uint32 c;	
	
	if(View)
	{
		for(c=0;c<256;c++)
			Text->DisallowChar(c);
		
		Text->AllowChar(B_ENTER);
		Text->AllowChar(B_SPACE);	
	}	
	
	return NULL;
}

/*
 * function    : Selection
 * purpose     : Set/Get the text selected
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Selection(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			SQI_List *lst = squirrel->LocalHeap->List();
			
			int32 f,t;
			
			Text->GetSelection(&f,&t);
			
			lst->Add2End(squirrel->LocalHeap->Number(f));
			lst->Add2End(squirrel->LocalHeap->Number(t-1));	
			
			return lst;
		}	
		else
		if(*str == "set")
		{
			SQI_List *lst = IsList((*inputs)[1]);
			
			if(lst)
			{
				if(lst->Length()==2)
				{
					SQI_Number *from = IsInteger((*lst)[0]);	
					SQI_Number *to = IsInteger((*lst)[1]);
					
					if(from && to)
					{
						Text->Select(Number2Long(from),Number2Long(to));
						return NULL;	
					}
					else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","offsets must be integer"));						
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","the list must have two inputs"));			
			}
			else
			{
				if(inputs->Length()==3)
				{
					SQI_Number *from = IsInteger((*inputs)[1]);	
					SQI_Number *to = IsInteger((*inputs)[2]);
					
					if(from && to)
					{
						Text->Select(Number2Long(from),Number2Long(to));
						return NULL;	
					}
					else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","offsets must be integer"));										
				}
				else
				{
					SQI_Keyword *word = IsKeyword((*inputs)[1]);
					if(word)
					{
						string *str = word->Data();
						if(*str == "all")
							Text->SelectAll();
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","word as second input must be the word \"all"));												
					
						return NULL;	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","second input must be a list or two numbers or a word"));				
				}	
			}	
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","first input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~selection","first input must be the word \"get or \"set"));		
}

/*
 * function    : Line
 * purpose     : Set/Get the line position
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Line(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[0]);
	
	if(View)
	{	
		if(cmd)
		{
			string *str = cmd->Data();
			if(*str == "get")
			{
				if(inputs->Length()==2)
				{
					SQI_Keyword *wd = IsKeyword((*inputs)[1]);
					if(wd)
					{
						str = wd->Data();
						
						if(*str == "last")
							return squirrel->LocalHeap->Number(Text->CountLines()-1);
						else
							return squirrel->LocalHeap->Number(0);	
					}
					else	
						throw(new SQI_Exception(SQI_EX_INVALIDE,"MEMO~line","second input must be the word \"last"));		
				}
				else
				return squirrel->LocalHeap->Number(Text->CurrentLine());	
			}	
			else
			if(*str == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_Number *l = IsInteger((*inputs)[1]);
		
					if(l)
					{
						uint32 ln = Number2Long(l);
						
						Text->GoToLine(ln);	
						
						return NULL;	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~line","second input must be an integer"));	
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MEMO~line","need a second input"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~line","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~line","first input must be the word \"get or \"set"));	
	}	
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
SQI_Object *SQI_BMemo::Enable(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Memo~enable","input must be a boolean"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Memo~enable","need one input"));
	
	return NULL;	
}

/*
 * function    : Insert
 * purpose     : Insert at the current position
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Insert(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	string *s;
	string str;
	
	for(int i=0;i<inputs->Length();i++)
	{
		s = (*inputs)[i]->Print(squirrel->precision);	
		str += *s;
		delete s;
	}	
	
	// first we move to the end of the text
	
	Lock();

	Text->Insert(str.c_str());	
	
	Unlock();
	return NULL;
}

/*
 * function    : Delete
 * purpose     : Delete come characters
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Delete(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(View)
	{
		if(inputs->Length()==2)
		{
			SQI_Number *f,*t;
			
			f = IsInteger((*inputs)[0]);
			t = IsInteger((*inputs)[1]);
			
			if(f && t)
			{
				int32 from,to;
				
				from = Number2Long(f);
				to = Number2Long(t);
				
				Lock();
				Text->Delete(from,to);
				Unlock();	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~delete","inputs must be integers"));			
		}
		if(inputs->Length()==1)
		{
			SQI_List *lst = IsList((*inputs)[0]);
			if(lst)
			{
				SQI_Number *f,*t;
				list<SQI_Object *> *l;
				list<SQI_Object *>::const_iterator i;
				
				if(lst->Length()!=2)
					throw(new SQI_Exception(SQI_EX_INVALIDE,"MEMO~delete","the lust must have two elements"));		
				
				l = lst->Data();
				
				i = l->begin();
				
				f = IsInteger(*i);
				i++;
				t = IsInteger(*i);
				
				if(f && t)
				{
					int32 from,to;
					
					from = Number2Long(f);
					to = Number2Long(t);
					
					Lock();
					Text->Delete(from,to);
					Unlock();	
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~delete","inputs must be integers"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MEMO~delete","input must be a list"));				
		}	
		else // the current selection
			Text->Delete();		
	}
		
	return NULL;	
}

/*
 * function    : NewLine
 * purpose     : Add a new line to the memo
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::NewLine(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	string *s;
	string str;
	
	for(int i=0;i<inputs->Length();i++)
	{
		s = (*inputs)[i]->Print(squirrel->precision);	
		str += *s;
		delete s;
	}	
	
	// first we move to the end of the text
	
	Lock();

	int32 count = Text->CountLines();
		
	if(count)
	{	
		Text->GoToLine(count-1);	
	}

	Text->Insert(str.c_str());	
	
	Unlock();
	return NULL;
}

/*
 * function    : Load
 * purpose     : Load a file in the widget
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Load(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		if(IsString((*inputs)[0]))
		{
			BFile *file;
			off_t size;
			SQI_String *path = IsString((*inputs)[0]);
			
			file = new BFile(path->Data()->c_str(),B_READ_ONLY);
			
			if(file->InitCheck()==B_NO_ERROR)
			{	
				file->GetSize(&size);	
			
				Lock();
				Text->SetText(file,0,size);
				Unlock();
				delete file;
				return squirrel->interpreter->True;
			}
			else
				return squirrel->interpreter->False;	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~load","input must be a string"));				
	}
	else	
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MEMO~load","need an input"));
}

/*
 * function    : Save
 * purpose     : Save the text in a file
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Save(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		if(IsString((*inputs)[0]))
		{
			BFile *file;
			off_t size;
			SQI_String *path = IsString((*inputs)[0]);
			
			file = new BFile(path->Data()->c_str(),B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
			
			if(file->InitCheck()==B_NO_ERROR)
			{	
				const char *text;
				
				Lock();
				text = Text->Text();							
				file->Write(text,strlen(text));
				Unlock();	
		
				delete file;
			
				return squirrel->interpreter->True;
			}
			else
				return squirrel->interpreter->False;	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~save","input must be a string"));				
	}
	else	
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"MEMO~save","need an input"));	
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
SQI_Object *SQI_BMemo::AlignVertical(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be \"top \"center \"bottom"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
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
SQI_Object *SQI_BMemo::AlignHorizontal(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be \"left \"center \"right"));
					
				return NULL;			
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::PadX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::PadY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::Pad(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third and fourth inputs must be a number"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BMemo::Align(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be \"left \"center \"right"));
				
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
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","fourth input must be \"top \"center \"bottom"));
							
					return NULL;			
				}
				else	
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third and fourth inputs must be a word"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","need 4 inputs"));		
	
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
SQI_Object *SQI_BMemo::ExpandX(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::ExpandY(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a number"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::Expand(SQI_Squirrel *squirrel,SQI_Args *inputs)
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
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third and fourth inputs must be a boolean"));
			}		
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","need 2 inputs at least"));		
	
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
SQI_Object *SQI_BMemo::BgColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = Text->ViewColor();
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
				
				Text->SetViewColor(color);	
				
				Text->bgcolor_set = true;
				
				Text->Invalidate();
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::HighColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = Text->HighColor();
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
				
				Text->SetHighColor(color);	
				
				Text->hcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::LowColor(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			rgb_color color = Text->LowColor();
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
				
				Text->SetLowColor(color);	
				
				Text->lcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Color
 * purpose     : Set/Get the text color
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Color(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{		
			BFont font;
			uint32 flag = B_FONT_ALL;
			rgb_color color;
			
			Text->GetFontAndColor(&font,&flag,&color);
			
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
				
				Text->SetFontAndColor(NULL,B_FONT_ALL,&color);	
				
				Text->lcolor_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a list"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

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
SQI_Object *SQI_BMemo::Font(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{	
			BFont font;
			uint32 flag = B_FONT_ALL;
			
			Text->GetFontAndColor(&font,&flag,NULL);	
			
			return new SQI_Font(squirrel,&font);	
		}	
		else
		if(*str == "set")
		{
			SQI_Font *font = IsFont((*inputs)[2]);
			
			if(font)
			{
				Text->SetFontAndColor(font,B_FONT_ALL,NULL);
				
				Text->font_set = true;
				
				return NULL;					
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a font object"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		

}

/*
 * function    : Warp
 * purpose     : Set/Get the warping of the line
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Wrap(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			if(Text->DoesWordWrap())
				return squirrel->interpreter->True;
			else
				return squirrel->interpreter->False;	
		}	
		else
		if(*str == "set")
		{
			SQI_Number *w = IsBool((*inputs)[2]);
			
			if(w)
			{
				Text->SetWordWrap(Number2Bool(w));
				
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Indent
 * purpose     : Set/Get the indent of the line
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Indent(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			if(Text->DoesAutoindent())
				return squirrel->interpreter->True;
			else
				return squirrel->interpreter->False;	
		}	
		else
		if(*str == "set")
		{
			SQI_Number *w = IsBool((*inputs)[2]);
			
			if(w)
			{
				Text->SetAutoindent(Number2Bool(w));
				
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a boolean"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));		
}

/*
 * function    : Alignment
 * purpose     : Set/Get the alignment of the content
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : SQI_Object *
 * side effect : none
 */
SQI_Object *SQI_BMemo::Alignment(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	SQI_Keyword *cmd = IsKeyword((*inputs)[1]);
	
	if(cmd)
	{
		string *str = cmd->Data();
		if(*str == "get")
		{
			alignment a;
			
			a = Text->Alignment();
			
			if(a == B_ALIGN_LEFT)
				return squirrel->LocalHeap->Keyword("left");
			else
			if(a == B_ALIGN_CENTER)
				return squirrel->LocalHeap->Keyword("center");
			else
			if(a == B_ALIGN_RIGHT)
				return squirrel->LocalHeap->Keyword("right");							
		}	
		else
		if(*str == "set")
		{
			SQI_Keyword *w = IsKeyword((*inputs)[2]);
			
			if(w)
			{
				string *str = w->Data();
				
				if(*str == "left")
					Text->SetAlignment(B_ALIGN_LEFT);
				else	
				if(*str == "right")
					Text->SetAlignment(B_ALIGN_RIGHT);
				else			
				if(*str == "center")
					Text->SetAlignment(B_ALIGN_CENTER);
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"MEMO~config","third input must be the word : \"left \"right or \"center"));			
				
				return NULL;				
			}
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","third input must be a word"));
		}		
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MEMO~config","second input must be the word \"get or \"set"));
}

// some used function

// Fill up the member map
void InitBMemo()
{	
	MemoMemberMap[string("config")] = &SQI_BMemo::Config;
	MemoMemberMap[string("enable")] = &SQI_BMemo::Enable;
	MemoMemberMap[string("is.enable")] = &SQI_BMemo::IsEnable;
	MemoMemberMap[string("is.focus")] = &SQI_BMemo::IsFocus;
	MemoMemberMap[string("text")] = &SQI_BMemo::TheText;
	MemoMemberMap[string("allow")] = &SQI_BMemo::Allow;
	MemoMemberMap[string("disallow")] = &SQI_BMemo::Disallow;
	MemoMemberMap[string("allow.all")] = &SQI_BMemo::AllowAll;
	MemoMemberMap[string("disallow.all")] = &SQI_BMemo::DisallowAll;
	MemoMemberMap[string("selection")] = &SQI_BMemo::Selection;
	MemoMemberMap[string("insert")] = &SQI_BMemo::Insert;
	MemoMemberMap[string("delete")] = &SQI_BMemo::Delete;
	MemoMemberMap[string("load")] = &SQI_BMemo::Load;
	MemoMemberMap[string("save")] = &SQI_BMemo::Save;
	MemoMemberMap[string("line")] = &SQI_BMemo::Line;		
	
	MemoSettingMap[string("align")] = &SQI_BMemo::Align;
	MemoSettingMap[string("align.v")] = &SQI_BMemo::AlignVertical;
	MemoSettingMap[string("align.h")] = &SQI_BMemo::AlignHorizontal;
	MemoSettingMap[string("bgcolor")] = &SQI_BMemo::BgColor;
	MemoSettingMap[string("expand")] = &SQI_BMemo::Expand;
	MemoSettingMap[string("expand.x")] = &SQI_BMemo::ExpandX;
	MemoSettingMap[string("expand.y")] = &SQI_BMemo::ExpandY;
	MemoSettingMap[string("font")] = &SQI_BMemo::Font;
	MemoSettingMap[string("low.color")] = &SQI_BMemo::LowColor;
	MemoSettingMap[string("high.color")] = &SQI_BMemo::HighColor;
	MemoSettingMap[string("pad")] = &SQI_BMemo::Pad;
	MemoSettingMap[string("pad.x")] = &SQI_BMemo::PadX;
	MemoSettingMap[string("pad.y")] = &SQI_BMemo::PadY;	
	MemoSettingMap[string("wrap")] = &SQI_BMemo::Wrap;
	MemoSettingMap[string("indent")] = &SQI_BMemo::Indent;
	MemoSettingMap[string("color")] = &SQI_BMemo::Color;
	MemoSettingMap[string("alignment")] = &SQI_BMemo::Alignment;			
}

/*
void MemoVarHook(SQI_Object *value,void *data,SQI_Squirrel *squirrel)
{
	SQI_BMemo *cb = static_cast<SQI_BMemo *>(data);
	SQI_String *str = IsString(value);
	if(str)
	{
		cb->Lock();
		cb->Text->SetText(str->Data()->c_str());
		cb->Unlock();	
	}		
}
*/

/*
 * function    : IsMemo
 * purpose     : Test if an object is a bsimplelist
 * input       : none
 *
 * SQI_Object *object, object to test
 *
 * output      : SQI_BMemo *, the bsimplelist or NULL else
 * side effect : none
 */
SQI_BMemo *IsMemo(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_BMEMO)
    	return dynamic_cast<SQI_BMemo *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

