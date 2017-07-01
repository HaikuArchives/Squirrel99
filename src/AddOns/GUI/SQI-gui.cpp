/*
 * Squirrel project
 *
 * file ?	SQI-gui.cpp
 * what	?	gui objects & primitives
 * who	?	jlv
 * when	?	11/21/99
 * last	?	01/15/01
 *
 *
 * (c) Kirilla 1999-2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "Family.h"
#include "SQI-font.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"

// widgets include go there
#include "SQI-bbutton.h"
#include "SQI-bframe.h"
#include "SQI-bbox.h"
#include "SQI-bcheckbox.h"
#include "SQI-bbanner.h"
#include "SQI-bcolorcontrol.h"
#include "SQI-bdroplist.h"
#include "SQI-bentry.h"
#include "SQI-bmenu.h"
#include "SQI-bmenubar.h"
#include "SQI-container.h"
#include "SQI-bmemo.h"
#include "SQI-bradiobutton.h"
#include "SQI-bsimplelist.h"
#include "SQI-bstatusbar.h"
#include "SQI-btext.h"
#include "SQI-bbarberpole.h"

#include "SQI-glue.h"
#include "SQI-bwindow.h"

#include "AnimateCursor.h"
#include "Spinwheel.h"
#include "Watch.h"
#include "Cursor.h"

// now some infos on the package

char *GUI_Pkg_name = "GUI";
char *GUI_Pkg_purpose = "GUI widgets";
char *GUI_Pkg_author = "[e-]";
char *GUI_Pkg_version = "0.74";
char *GUI_Pkg_date = "03/23/01";

// Busy animated cursor
AnimateCursor *Busy = NULL;
bool AppBusy = false;

/*
 * function    : SQI_GUI_Init
 * purpose     : Builtin predicate "Font.init"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_FontInit(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	InitFontFamilies(squirrel);
  	return NULL;
}

/*
 * function    : SQI_GUI_Families
 * purpose     : Builtin predicate "font.families"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_FontFamilies(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	SQI_List *lst = new SQI_List(squirrel->LocalHeap);
	
	GetFontFamilies(lst);
	
	return lst;
}

/*
 * function    : SQI_GUI_Styles
 * purpose     : Builtin predicate "font.styles"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_FontStyles(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		SQI_String *font = IsString((*args)[0]);
		if(!font)
			font = IsKeyword((*args)[0]);			
		if(font)
			return GetFontStyles(font->Data());	
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Font.styles","need one string)"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Font.styles","need one argument"));
}

/*
 * function    : SQI_GUI_Exist
 * purpose     : Builtin predicate "font.exist"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_FontExist(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		SQI_String *font = IsString((*args)[0]);	
		if(!font)
			font = IsKeyword((*args)[0]);	
		
		
		if(font)
		{
			if (ExistFont(font->Data()))
				return interp->True;
			else
				return interp->False;	
		}	
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Font.exists","need one string)"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Font.exists","need one argument"));
}

/*
 * function    : SQI_GUI_Font
 * purpose     : Builtin predicate "Font"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Font(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==0)
		return new SQI_Font(squirrel,be_plain_font);
	else
	{	
		SQI_String *font = IsString((*args)[0]);	
		if(!font)
			font = IsKeyword((*args)[0]);	
			
		if(font)
		{
			string *name = font->Data();
			
			if(*name == "plain")
				return new SQI_Font(squirrel,be_plain_font);	
			else
			if(*name == "bold")
				return new SQI_Font(squirrel,be_bold_font);
			else
			if(*name == "fixed")
				return new SQI_Font(squirrel,be_fixed_font);	
			else
			if (ExistFont(name))
			{
				if(args->Length()==2)
				{
					SQI_String *style = IsString((*args)[1]);	
					if(!style)
						style = IsKeyword((*args)[1]);
						
					if(style)
					{
						string *st = style->Data();
						return new SQI_Font(squirrel,name->c_str(),st->c_str());
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Font","second input must be a string/word)"));							
				}
				else	
					return new SQI_Font(squirrel,name->c_str(),NULL);
			}
			else
				return new SQI_Font(squirrel,be_plain_font);	
		}	
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Font","first input must be a string/word)"));
	}
}

/*
 * function    : SQI_GUI_IsFont
 * purpose     : Builtin predicate "is.font"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsFont(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsFont((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.font","need one input"));
}

// Gui

/*
 * function    : SQI_GUI_Hook
 * purpose     : Builtin predicate "Hook"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Hook(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<3)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Hook","need at least 3 inputs"));	
		
	SQI_ThePointer *obj = IsThePointer((*args)[0]);
	SQI_Keyword *event = IsKeyword((*args)[1]);
	SQI_Object *action = (*args)[2];

	if(obj && event && action)
	{
		if((action->Is()==SQI_KEYWORD) || (action->Is()==SQI_NODE))
		{
			SQI_BWindow *win;
			if(win = IsWindow(obj))
			{
				list<SQI_Object *> *extra = NULL;
				
				// if we have more args , it's extra inputs for the hook
				
				if(args->Length()>3)
				{
					SQI_Object *obj;
					extra = new list<SQI_Object *>;
					for(int i=3;i<args->Length();i++)
					{
						obj = (*args)[i];
						obj->AddRef();
						extra->push_back(obj);	
					}	
				}
					
				if(!win->Hook(event,action,extra))
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Hook","unknow hook"));
			}
			else
			if(IsWidget(obj))
			{
				list<SQI_Object *> *extra = NULL;
				SQI_BWidget *w = dynamic_cast<SQI_BWidget *>(obj);
				
				// if we have more args , it's extra inputs for the hook
				
				if(args->Length()>3)
				{
					SQI_Object *obj;
					extra = new list<SQI_Object *>;
					for(int i=3;i<args->Length();i++)
					{
						obj = (*args)[i];
						obj->AddRef();
						extra->push_back(obj);	
					}	
				}
					
				if(!w->Hook(event,action,extra))
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Hook","unknow hook"));					
			}
			else
			/*
			if(IsDrawing(obj))
			{
				list<SQI_Object *> *extra = NULL;
				SQI_BDrawing *w = dynamic_cast<SQI_BDrawing *>(obj);
				
				// if we have more args , it's extra inputs for the hook
				
				if(args->Length()>3)
				{
					SQI_Object *obj;
					extra = new list<SQI_Object *>;
					for(int i=3;i<args->Length();i++)
					{
						obj = (*args)[i];
						obj->AddRef();
						extra->push_back(obj);	
					}	
				}
					
				if(!w->Hook(event,action,extra))
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Hook","unknow hook"));						
			}	
			else
			*/
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Hook","not a window/widget object"));
			
			return NULL;	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Hook","third argument must be a keyword or a block"));	
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Hook","not a window/widget object"));	
} 

/*
 * function    : SQI_GUI_UnHook
 * purpose     : Builtin predicate "Unhook"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_UnHook(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<2)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Unhook","need at least 2 inputs"));	
		
	SQI_ThePointer *obj = IsThePointer((*args)[0]);
	SQI_Keyword *event = IsKeyword((*args)[1]);

	if(obj && event)
	{
	
		SQI_BWindow *win;
		if(win = IsWindow(obj))
		{				
			if(!win->UnHook(event))
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Unhook","unknow hook"));
		}
		else
		if(IsWidget(obj))
		{
			
			SQI_BWidget *w = dynamic_cast<SQI_BWidget *>(obj);
					
			if(!w->UnHook(event))
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Unhook","unknow hook"));					
		}
		else
		/*
		if(IsDrawing(obj))
		{
			SQI_BDrawing *w = dynamic_cast<SQI_BDrawing *>(obj);
					
			if(!w->UnHook(event))
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Unhook","unknow hook"));				
		}	
		else
		*/
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Unhook","not a window/widget object"));
		
		return NULL;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Unhook","not a window/widget object"));		
} 

/*
 * function    : SQI_GUI_Window
 * purpose     : Builtin predicate "Window"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Window(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=3)
	{
		SQI_Keyword *type = IsKeyword((*args)[0]);
		SQI_String *title = IsString((*args)[1]);
		if(!title)
			title = IsKeyword((*args)[1]);
		SQI_List *position = IsList((*args)[2]);
		
		if(title && position)
		{
			// we get the look and feel
			
			window_look	look;
			window_feel	feel = B_NORMAL_WINDOW_FEEL;
			uint32 flags = 0;
		
			if(type)
			{	
				string *str = type->Data();
		
				if(*str == "titled")
					look = B_TITLED_WINDOW_LOOK;
				else
				if(*str == "modal")
				{
					look = B_MODAL_WINDOW_LOOK;
					feel = B_MODAL_APP_WINDOW_FEEL;	
				}	
				else				
				if(*str == "bordered")
					look = B_BORDERED_WINDOW_LOOK;
				else
				if(*str == "document")
					look = B_DOCUMENT_WINDOW_LOOK;
				else
				if(*str == "floating")
					look = B_FLOATING_WINDOW_LOOK;
				else
				if(*str == "no.bordered")
					look = B_NO_BORDER_WINDOW_LOOK;
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","window look must be a valide one"));
			}
			else
			{
				SQI_List *lst = IsList((*args)[0]);
				if(lst)
				{
					if(lst->Length()!=2)
						throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","look/feel list must contain 2 arguments"));					
					else
					{
						SQI_Keyword *l = IsKeyword((*lst)[0]);
						SQI_Keyword *f = IsKeyword((*lst)[1]);
						
						if(l && f)
						{
							string *str = l->Data();
							
							if(*str == "titled")
								look = B_TITLED_WINDOW_LOOK;
							else
							if(*str == "modal")
								look = B_MODAL_WINDOW_LOOK;
							else				
							if(*str == "bordered")
								look = B_BORDERED_WINDOW_LOOK;
							else
							if(*str == "document")
								look = B_DOCUMENT_WINDOW_LOOK;
							else
							if(*str == "floating")
								look = B_FLOATING_WINDOW_LOOK;
							else
							if(*str == "no.bordered")
								look = B_NO_BORDER_WINDOW_LOOK;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","window look must be a valide one"));

							str = f->Data();
							
							if(*str == "normal")
								feel = B_NORMAL_WINDOW_FEEL;
							else
							if(*str == "modal.subset")
								feel = B_MODAL_SUBSET_WINDOW_FEEL;
							else
							if(*str == "modal.app")
								feel = B_MODAL_APP_WINDOW_FEEL;
							else
							if(*str == "modal.all")
								feel = B_MODAL_ALL_WINDOW_FEEL;
							else
							if(*str == "floating.subset")
								feel = B_FLOATING_SUBSET_WINDOW_FEEL;
							else
							if(*str == "floating.app")
								feel = B_FLOATING_APP_WINDOW_FEEL;
							else
							if(*str == "floating.all")
								feel = B_FLOATING_ALL_WINDOW_FEEL;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","window feel must be a valide one"));								
						}
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","look/feel list must contain 2 words"));												
					}					
				}
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Window","window look and feel must be a word or a list"));					
			}

			// now the position we define an arbitrary size
			
			BRect frame = Position2Frame(position);
			
			if(args->Length()>3)
			{
				SQI_Keyword *word;
				string *flag;
				for(int i=3;i<args->Length();i++)
				{
					if(word = IsKeyword((*args)[i]))
					{
						flag = word->Data();
						if(*flag == "not.movable")
							flags = flags | B_NOT_MOVABLE;
						else
						if(*flag == "not.resizable")
							flags = flags | B_NOT_RESIZABLE;
						else
						if(*flag == "not.h.resizable")
							flags = flags | B_NOT_H_RESIZABLE;
						else
						if(*flag == "not.v.resizable")
							flags = flags | B_NOT_V_RESIZABLE;
						else
						if(*flag == "not.closable")
							flags = flags | B_NOT_CLOSABLE;
						else
						if(*flag == "not.zoomable")
							flags = flags | B_NOT_ZOOMABLE;
						else
						if(*flag == "not.minimizable")
							flags = flags | B_NOT_MINIMIZABLE;
						else
						if(*flag == "accept.first.click")
							flags = flags | B_WILL_ACCEPT_FIRST_CLICK;
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Window","behavior must be valid word"));							
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Window","behaviors must be word"));	
				}	
			}
			
			return new SQI_BWindow(squirrel,frame,title->Data()->c_str(),look,feel,flags);
		}	
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Window","need a word, string and list"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Window","need at least 3 arguments"));
}

/*
 * function    : SQI_GUI_IsWindow
 * purpose     : Builtin predicate "is.window"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsWindow(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsWindow((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.window","need one input"));
}

/*
 * function    : SQI_GUI_Glue
 * purpose     : Builtin predicate "Glue"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Glue(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=4)
	{
		SQI_BWindow *win;
		SQI_BWidget *view;
		if(win = IsWindow((*args)[0]))
		{
			list<SQI_BWidget *> *childs = NULL;
			
			float padx = 0;
			float pady = 0;	
			char a = ALIGN_TOP;
		
			SQI_Keyword *align = IsKeyword((*args)[1]);	
			SQI_List *pad = IsList((*args)[2]);	
			
			if(align && pad)
			{
				string *al = align->Data();
				
				if(*al == "top")
					a = ALIGN_TOP;
				else					
				if(*al == "bottom")
					a = ALIGN_BOTTOM;
				else		
				if(*al == "left")
					a = ALIGN_LEFT;
				else
				if(*al == "right")
					a = ALIGN_RIGHT;			
								
				List2Pad(pad,&padx,&pady);
				
				// now, all the other inputs are to be put in a list	
				
				childs = new list<SQI_BWidget *>;	
				SQI_BWidget *widget;
				SQI_Widget *w;
				BView *v;
					
				// we going to make the list by ordering the widget a bit first
				// left , center , right
		
				for(int i=3;i<args->Length();i++)
				{
					if(widget = IsWidget((*args)[i]))
					{
						w = widget->GiveWidget();
						v = widget->GiveBView();
									
						if(v->Parent() || v->Window())
							continue;	
						
						// if a pad has been given, we set it here
						// else we don't do that										
						if(padx > 0 && pady > 0)
						{
							w->padx = padx;
							w->pady = pady;
						}	
						
						if(a==ALIGN_RIGHT)
							childs->push_back(widget);
						else
							childs->push_front(widget);	
					}
					else
					{
						delete childs;
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Glue","the object to glue must be a widget"));	
					}								
				}	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Glue","second input must be a word and third input a list"));
											
			win->Glue(a,childs);
		}
		else
		if(view = IsContainer((*args)[0]))
		{
			list<SQI_BWidget *> *childs = NULL;
			float padx = 0;
			float pady = 0;	
			char a = ALIGN_TOP;
				
			SQI_Keyword *align = IsKeyword((*args)[1]);	
			SQI_List *pad = IsList((*args)[2]);	
			
			if(align && pad)
			{
				string *al = align->Data();
				
				if(*al == "top")
					a = ALIGN_TOP;
				else					
				if(*al == "bottom")
					a = ALIGN_BOTTOM;
				else		
				if(*al == "left")
					a = ALIGN_LEFT;
				else
				if(*al == "right")
					a = ALIGN_RIGHT;			
								
				List2Pad(pad,&padx,&pady);
				
				// now, all the other inputs are to be put in a list	
				
				childs = new list<SQI_BWidget *>;	
				SQI_BWidget *widget;
				SQI_Widget *w;
				BView *v;
					
				// we going to make the list by ordering the widget a bit first
				// left , center , right
		
				for(int i=3;i<args->Length();i++)
				{
					if(widget = IsWidget((*args)[i]))
					{
							w = widget->GiveWidget();
							v = widget->GiveBView();
							
							if(v->Parent() || v->Window())
								continue;
							
							// if a pad has been given, we set it here
							// else we don't do that										
							if(padx > 0 && pady > 0)
							{
								w->padx = padx;
								w->pady = pady;
							}	
							
						if(a==ALIGN_RIGHT)
							childs->push_back(widget);
						else
							childs->push_front(widget);		
					}
					else
					{
							delete childs;
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Glue","the object to glue must be a widget"));	
					}									
				}
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Glue","second input must be a word and third input a list"));
				
			view->Glue(a,childs);
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Glue","first input must be a window or a widget container"));		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Glue","need at least 4 arguments"));	
		
	return NULL;
}

/*
 * function    : SQI_GUI_UnGlue
 * purpose     : Builtin predicate "Unglue"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_UnGlue(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_BWidget *widget;
		BView *v;
		SQI_Widget *p;
		for(int i=0;i<args->Length();i++)
		{
			if(widget = IsWidget((*args)[i]))
			{
				v = widget->GiveBView();		
				
				if(v->Parent())
				{	
					p = dynamic_cast<SQI_Widget *>(widget->GiveBView()->Parent());	
					SQI_BWidget *w = p->BWidget;
					w->UnGlue(widget);
					w->ReGlue(NULL,NULL);
				}
				else
				if(v->Window())
				{
					SQI_BWindow *window = widget->Window;
					window->UnGlue(widget);
					window->ReGlue(NULL,NULL);
				}
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Unglue","the object to unglue must be a widget"));									
		}
		
		return NULL;			
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Unglue","need at least one argument"));	
}

/*
 * function    : SQI_GUI_Focus
 * purpose     : Builtin predicate "Focus"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Focus(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{	
		SQI_BWidget *widget;
		if(widget = IsWidget((*args)[0]))
		{
			if(args->Length()==2)
			{
				SQI_Keyword *focus = IsKeyword((*args)[1]);
				if(focus)
				{
					string *str = focus->Data();
					
					if(*str == "on")
						widget->MakeFocus(true);
					else
					if(*str == "off")
						widget->MakeFocus(false);						
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Focus","second input must be the word : \"on or \"off"));			
			}
			else	
				widget->MakeFocus(true);		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Focus","need a widget as first input"));	
		
		return NULL;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Focus","need one input at least"));
}	

/*
 * function    : SQI_GUI_IsWidget
 * purpose     : Builtin predicate "is.widget"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsWidget(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsWidget((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.widget","need one input"));
}

/*
 * function    : SQI_GUI_IsContainer
 * purpose     : Builtin predicate "is.container"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsContainer(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsContainer((*args)[0]) || IsWindow((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.container","need one input"));
}

// Widgets !!

/*
 * function    : SQI_GUI_Button
 * purpose     : Builtin predicate "Button"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Button(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
			
		if(!label)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Button","first input must be a string/word"));
			
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW;		
			
		if(args->Length()>1)
		{	
			SQI_List *size = IsList((*args)[1]);	
			
			if(size)
			{			
				if(args->Length()>=3)
				{
					for(int i = 2;i<args->Length();i++)
					{	
						SQI_Keyword *f = IsKeyword((*args)[i]);
						if(f)
						{
							string *s = f->Data();
							
							if(*s == "navigable")
								flags = flags | B_NAVIGABLE;
							else	
							if(*s == "navigable.jump")
								flags = flags | B_NAVIGABLE_JUMP;
							else
							if(*s == "pulsed")
								flags = flags | B_PULSE_NEEDED;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Button","word is not a valide flag"));	
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Button","flag must be word"));
					}
				}
				
				// time to get the size from the list
				
				List2Size(size,&width,&height);
			}
		}	
			
		return new SQI_BButton(squirrel,label->Data()->c_str(),width,height,mode,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Button","need at least one input"));	
}

/*
 * function    : SQI_GUI_IsButton
 * purpose     : Builtin predicate "is.button"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsButton(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsButton((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.button","need one input"));
}

/*
 * function    : SQI_GUI_Frame
 * purpose     : Builtin predicate "Frame"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Frame(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_List *size = IsList((*args)[0]);
		if(size)
		{
			char r = RELIEF_FLATENED;
			int width = -1;
			int height = -1;
			uint32 mode = B_FOLLOW_NONE;
			uint32 flags = 0;	
			
			if(args->Length()>=2)
			{				
				for(int i = 1;i<args->Length();i++)
				{	
					SQI_Keyword *f = IsKeyword((*args)[i]);
					if(f)
					{
						string *s = f->Data();
						
						if(*s == "navigable")
							flags = flags | B_NAVIGABLE;
						else	
						if(*s == "navigable.jump")
							flags = flags | B_NAVIGABLE_JUMP;
						else
						if(*s == "pulsed")
							flags = flags | B_PULSE_NEEDED;
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Frame","word is not a valide flag"));	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Frame","flag must be word"));
				}
			}
			
			// time to get the size from the list
			
			List2Size(size,&width,&height);	
			
			return new SQI_BFrame(squirrel,r,width,height,mode,flags);	
		}
		else
		{
			SQI_Keyword *relief = IsKeyword	((*args)[0]);
			if(relief)
			{
				char r = RELIEF_FLATENED;
				int width = -1;
				int height = -1;
				uint32 mode = B_FOLLOW_NONE;
				uint32 flags = 0;	
			
				string *str = relief->Data();
				
				if(*str == "raised")
					r = RELIEF_RAISED;
				else
				if(*str == "lowered")
					r = RELIEF_LOWERED;
				else
				if(*str == "flattened")
					r = RELIEF_FLATENED;
				else
				if(*str == "bordered")
					r = RELIEF_BORDERED;
				else							
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Frame","relief must be a valid word"));		
			
				if(args->Length()>1)
				{	
					size = IsList((*args)[1]);	
					
					if(args->Length()>2)
					{				
						for(int i = 2;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
								
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"Frame","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Frame","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);	
				}
				
				return new SQI_BFrame(squirrel,r,width,height,mode,flags);	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Frame","first input must be a list or a word"));	
		}
			
	}
	else
		return new SQI_BFrame(squirrel,RELIEF_FLATENED,0,0,B_FOLLOW_NONE,0);
}

/*
 * function    : SQI_GUI_IsFrame
 * purpose     : Builtin predicate "is.frame"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsFrame(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsFrame((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.frame","need one input"));
}

/*
 * function    : SQI_GUI_Box
 * purpose     : Builtin predicate "Box"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Box(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
	
		if(label)
		{		
			border_style b = B_PLAIN_BORDER;
			int width = -1;
			int height = -1;
			uint32 mode = B_FOLLOW_NONE;
			uint32 flags = 0;	
			
			if(args->Length()>1)
			{		
				SQI_List *size = IsList((*args)[1]);
				
				if(size)
				{		
					if(args->Length()>=3)
					{
						for(int i = 2;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
								
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Box","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);	
				}
				else
				{
					SQI_Keyword *style = IsKeyword((*args)[1]);
					if(style)
					{
						string *y = style->Data();
						
						if(*y == "plain")
							b = B_PLAIN_BORDER;
						else
						if(*y == "fancy")
							b = B_FANCY_BORDER;
						else
						if(*y == "none")
							b = B_NO_BORDER;
						else							
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide style"));	
	
						if(args->Length()>2)
						{
							size = IsList((*args)[2]);
							
							if(size)
							{
								if(args->Length()>3)
								{
									for(int i = 3;i<args->Length();i++)
									{	
										SQI_Keyword *f = IsKeyword((*args)[i]);
										if(f)
										{
											string *s = f->Data();
											
											if(*s == "navigable")
												flags = flags | B_NAVIGABLE;
											else	
											if(*s == "navigable.jump")
												flags = flags | B_NAVIGABLE_JUMP;
											else
											if(*s == "pulsed")
												flags = flags | B_PULSE_NEEDED;
											else
												throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide flag"));	
										}
										else
											throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Box","flag must be word"));
									}
								}
								
								// time to get the size from the list
								
								List2Size(size,&width,&height);
							}
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","third argument must be a list"));
						}					
							
					}
				}
			}
			
			return new SQI_BBox(squirrel,label->Data()->c_str(),width,height,mode,flags,b);	
		}
		else
		{
			SQI_BWidget *widget;
			
			if(widget = IsWidget((*args)[0]))
			{	
				border_style b = B_PLAIN_BORDER;
				int width = -1;
				int height = -1;
				uint32 mode = B_FOLLOW_NONE;
				uint32 flags = 0;	
				
				if(args->Length()>1)
				{		
					SQI_List *size = IsList((*args)[1]);
					
					if(size)
					{		
						if(args->Length()>=3)
						{
							for(int i = 2;i<args->Length();i++)
							{	
								SQI_Keyword *f = IsKeyword((*args)[i]);
								if(f)
								{
									string *s = f->Data();
									
									if(*s == "navigable")
										flags = flags | B_NAVIGABLE;
									else	
									if(*s == "navigable.jump")
										flags = flags | B_NAVIGABLE_JUMP;
									else
									if(*s == "pulsed")
										flags = flags | B_PULSE_NEEDED;
									else
										throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide flag"));	
								}
								else
									throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Box","flag must be word"));
							}
						}
						
						// time to get the size from the list
						
						List2Size(size,&width,&height);	
					}
					else
					{
						SQI_Keyword *style = IsKeyword((*args)[1]);
						if(style)
						{
							string *y = style->Data();
							
							if(*y == "plain")
								b = B_PLAIN_BORDER;
							else
							if(*y == "fancy")
								b = B_FANCY_BORDER;
							else
							if(*y == "none")
								b = B_NO_BORDER;
							else							
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide style"));	
		
							if(args->Length()>2)
							{
								size = IsList((*args)[2]);
								
								if(size)
								{
									if(args->Length()>3)
									{
										for(int i = 3;i<args->Length();i++)
										{	
											SQI_Keyword *f = IsKeyword((*args)[i]);
											if(f)
											{
												string *s = f->Data();
												
												if(*s == "navigable")
													flags = flags | B_NAVIGABLE;
												else	
												if(*s == "navigable.jump")
													flags = flags | B_NAVIGABLE_JUMP;
												else
												if(*s == "pulsed")
													flags = flags | B_PULSE_NEEDED;
												else
													throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","word is not a valide flag"));	
											}
											else
												throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Box","flag must be word"));
										}
									}
									
									// time to get the size from the list
									
									List2Size(size,&width,&height);
								}
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"Box","third argument must be a list"));
							}					
								
						}
					}
				}
					
				return new SQI_BBox(squirrel,widget,width,height,mode,flags,b);	
			}	
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Box","first input must be a string/word"));
		}	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Box","need at least one input"));	
}

/*
 * function    : SQI_GUI_IsBox
 * purpose     : Builtin predicate "is.box"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsBox(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsBox((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.box","need one input"));
}

/*
 * function    : SQI_GUI_CheckBox
 * purpose     : Builtin predicate "CheckBox"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_CheckBox(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=2)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
			
		if(!label)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CheckBox","first input must be a string/word"));
			
		SQI_Keyword *var = IsKeyword((*args)[1]);
		if(!var)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CheckBox","second input must be a word"));	
			
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW | B_NAVIGABLE;		
			
		if(args->Length()>2)
		{			
			SQI_List *size = IsList((*args)[2]);
				
			if(size)
			{			
				if(args->Length()>=4)
				{
					for(int i = 3;i<args->Length();i++)
					{	
						SQI_Keyword *f = IsKeyword((*args)[i]);
						if(f)
						{
							string *s = f->Data();
							
							if(*s == "navigable")
								flags = flags | B_NAVIGABLE;
							else	
							if(*s == "navigable.jump")
								flags = flags | B_NAVIGABLE_JUMP;
							else
							if(*s == "pulsed")
								flags = flags | B_PULSE_NEEDED;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"CheckBox","word is not a valide flag"));	
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CheckBox","flag must be word"));
					}
				}
				
				// time to get the size from the list
				
				List2Size(size,&width,&height);
			}
		}	
			
		return new SQI_BCheckBox(squirrel,label->Data()->c_str(),var,width,height,mode,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"CheckBox","need at least two inputs"));	
}

/*
 * function    : SQI_GUI_IsCheckBox
 * purpose     : Builtin predicate "is.checkbox"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsCheckBox(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsCheckBox((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.checkbox","need one input"));
}


/*
 * function    : SQI_GUI_Banner
 * purpose     : Builtin predicate "Banner"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Banner(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_Keyword *var = IsKeyword((*args)[0]);			
		if(!var)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Banner","first input must be a word"));
									
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW;
		alignment align = B_ALIGN_LEFT;		
			
		if(args->Length()>1)
		{			
		
			SQI_Keyword *a = IsKeyword((*args)[1]);
			if(!a)
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Banner","second input must be a word"));	
				
			string *str = a->Data();
			
			if(*str == "center")
				align = B_ALIGN_CENTER;
			else
			if(*str == "left")
				align = B_ALIGN_LEFT;
			else				
			if(*str == "right")
				align = B_ALIGN_RIGHT;
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Banner","alignment must be a valid word : \"center , \"left or \"right"));	
				
			if(args->Length()>2)
			{
				SQI_List *size = IsList((*args)[2]);
				
				if(size)
				{			
					if(args->Length()>3)
					{
						for(int i = 3;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
								
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"Banner","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Banner","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);
				}
			}
		}	
			
		return new SQI_BBanner(squirrel,var,width,height,align,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Banner","need at least one input"));	
}

/*
 * function    : SQI_GUI_IsBanner
 * purpose     : Builtin predicate "is.banner"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsBanner(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsBanner((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.banner","need one input"));
}

/*
 * function    : SQI_GUI_ColorControl
 * purpose     : Builtin predicate "ColorControl"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_ColorControl(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==3)
	{
		SQI_Number *layout = IsInteger((*args)[0]);	
		SQI_Number *side = IsInteger((*args)[1]);
		SQI_Keyword *var = IsKeyword((*args)[2]);		
		
		if(layout && side && var)
		{
			float cell = Number2Float(side);
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
					throw(new SQI_Exception(SQI_EX_INVALIDE,"ColorControl","the layout must be 4 8 16 32 or 64"));		
			}	
			
			return new SQI_BColorControl(squirrel,matrix,cell,var);	
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"ColorControl","need 2 integers and a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"ColorControl","need 3 inputs"));	
}

/*
 * function    : SQI_GUI_IsColorControl
 * purpose     : Builtin predicate "is.colorcontrol"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsColorControl(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsColorControl((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.control","need one input"));
}

/*
 * function    : SQI_GUI_DropList
 * purpose     : Builtin predicate "DropList"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_DropList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=3)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
		SQI_Keyword *var = IsKeyword((*args)[1]);
		SQI_List *items = IsList((*args)[2]);
		
		if(label && var && items)
		{				
			int width = -1;
			int height = -1;
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE;
								
			if(args->Length()>3)
			{			
				SQI_List *size = IsList((*args)[3]);
					
				if(size)
				{			
					if(args->Length()>4)
					{
						for(int i = 5;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
								
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"DropList","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DropList","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);
				}
			}	
				
			return new SQI_BDropList(squirrel,label->Data()->c_str(),var,items,width,height,flags);
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"DropList","need : word/string word list"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"DropList","need at least three inputs"));	
}

/*
 * function    : SQI_GUI_IsDropList
 * purpose     : Builtin predicate "is.droplist"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsDropList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsDropList((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.droplist","need one input"));
}

/*
 * function    : SQI_GUI_Entry
 * purpose     : Builtin predicate "Entry"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Entry(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=2)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
			
		if(!label)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Entry","first input must be a string/word"));
			
		SQI_Keyword *var = IsKeyword((*args)[1]);
		if(!var)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Entry","second input must be a word"));	
			
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW | B_NAVIGABLE;		
			
		if(args->Length()>2)
		{			
			SQI_List *size = IsList((*args)[2]);
				
			if(size)
			{			
				if(args->Length()>=4)
				{
					for(int i = 3;i<args->Length();i++)
					{	
						SQI_Keyword *f = IsKeyword((*args)[i]);
						if(f)
						{
							string *s = f->Data();
							
							if(*s == "navigable")
								flags = flags | B_NAVIGABLE;
							else	
							if(*s == "navigable.jump")
								flags = flags | B_NAVIGABLE_JUMP;
							else
							if(*s == "pulsed")
								flags = flags | B_PULSE_NEEDED;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"Entry","word is not a valide flag"));	
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Entry","flag must be word"));
					}
				}
				
				// time to get the size from the list
				
				List2Size(size,&width,&height);
			}
		}	
			
		return new SQI_BEntry(squirrel,label->Data()->c_str(),var,width,height,mode,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Entry","need at least two inputs"));	
}

/*
 * function    : SQI_GUI_IsEntry
 * purpose     : Builtin predicate "is.entry"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsEntry(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsEntry((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.entry","need one input"));
}

/*
 * function    : SQI_GUI_MenuBar
 * purpose     : Builtin predicate "MenuBar"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_MenuBar(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	menu_layout layout = B_ITEMS_IN_ROW;
	
	if(args->Length()==1)
	{
		SQI_Keyword *l = IsKeyword((*args)[0]);
		if(l)
		{
			string *str = l->Data();
			
			if(*str == "column")
				layout = B_ITEMS_IN_COLUMN;
			else
			if(*str == "row")
				layout = B_ITEMS_IN_ROW;
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MenuBar","layout must be : \"row , \"column"));																	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"MenuBar","when specified the layout must be a word"));				
	}
		
	return new SQI_BMenuBar(squirrel,layout);
}

/*
 * function    : SQI_GUI_IsMenuBar
 * purpose     : Builtin predicate "is.menubar"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsMenuBar(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsMenuBar((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.menubar","need one input"));
}

/*
 * function    : SQI_GUI_Menu
 * purpose     : Builtin predicate "Menu"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Menu(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]); 	
			
		if(label)	
			return new SQI_BMenu(squirrel,label->Data()->c_str());
		else
		{
			SQI_Image *img = IsImage((*args)[0]);
			if(img)
				return new SQI_BMenu(squirrel,img);
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Menu","input must be a word, a string or an image"));		
		}
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Menu","need an input"));
}

/*
 * function    : SQI_GUI_IsMenu
 * purpose     : Builtin predicate "is.menu"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsMenu(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsMenu((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.menu","need one input"));
}

/*
 * function    : SQI_GUI_Memo
 * purpose     : Builtin predicate "Memo"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Memo(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=0)
	{
		SQI_List *size = IsList((*args)[0]);
		
		if(size)
		{				
			int width = -1;
			int height = -1;
			uint32 flags = 0;
			BFont *font = const_cast<BFont *>(be_plain_font);
													
			List2Size(size,&width,&height);		
					
			if(args->Length()>=2)
			{
				SQI_Font *ft = IsFont((*args)[1]);
				if(ft)
					font = ft;	
			}		
					
			if(args->Length()>=3)
			{
				for(int i = 2;i<args->Length();i++)
				{	
					SQI_Keyword *f = IsKeyword((*args)[i]);
					if(f)
					{
						string *s = f->Data();
						
						if(*s == "navigable")
							flags = flags | B_NAVIGABLE;
						else	
						if(*s == "navigable.jump")
							flags = flags | B_NAVIGABLE_JUMP;
						else
						if(*s == "pulsed")
							flags = flags | B_PULSE_NEEDED;
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Memo","word is not a valide flag"));	
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Memo","flag must be word"));
				}		
			}	
				
			return new SQI_BMemo(squirrel,width,height,font,flags);
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Memo","need : list"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Memo","need one inputs"));	
}

/*
 * function    : SQI_GUI_IsMemo
 * purpose     : Builtin predicate "is.memo"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsMemo(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsMemo((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.memo","need one input"));
}

/*
 * function    : SQI_GUI_RadioButton
 * purpose     : Builtin predicate "RadioButton"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_RadioButton(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=3)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
			
		if(!label)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"RadioButton","first input must be a string/word"));
			
		SQI_Keyword *var = IsKeyword((*args)[1]);
		if(!var)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"RadioButton","second input must be a word"));	
			
		SQI_Object *value = (*args)[2];	
			
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW | B_NAVIGABLE;		
			
		if(args->Length()>3)
		{			
			SQI_List *size = IsList((*args)[3]);
				
			if(size)
			{			
				if(args->Length()>5)
				{
					for(int i = 4;i<args->Length();i++)
					{	
						SQI_Keyword *f = IsKeyword((*args)[i]);
						if(f)
						{
							string *s = f->Data();
							
							if(*s == "navigable")
								flags = flags | B_NAVIGABLE;
							else	
							if(*s == "navigable.jump")
								flags = flags | B_NAVIGABLE_JUMP;
							else
							if(*s == "pulsed")
								flags = flags | B_PULSE_NEEDED;
							else
								throw(new SQI_Exception(SQI_EX_INVALIDE,"RadioButton","word is not a valide flag"));	
						}
						else
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"RadioButton","flag must be word"));
					}
				}
				
				// time to get the size from the list
				
				List2Size(size,&width,&height);
			}
		}	
			
		return new SQI_BRadioButton(squirrel,label->Data()->c_str(),var,value,width,height,mode,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"RadioButton","need at least two inputs"));	
}

/*
 * function    : SQI_GUI_IsRadioButton
 * purpose     : Builtin predicate "is.radiobutton"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsRadioButton(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsRadioButton((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.radiobutton","need one input"));
}

/*
 * function    : SQI_GUI_SimpleList
 * purpose     : Builtin predicate "SimpleList"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_SimpleList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=3)
	{
		SQI_Keyword *mode = IsKeyword((*args)[0]);
		SQI_Keyword *scroll = IsKeyword((*args)[1]);
		SQI_Keyword *var = IsKeyword((*args)[2]);
		SQI_List *items = IsList((*args)[3]);
		
		if(mode && scroll && var && items)
		{				
			int width = -1;
			int height = -1;
			uint32 flags = 0;
			list_view_type type;
			char scr = SCROLL_NONE;	
			
			string *str = mode->Data();
			
			if(*str == "single")
				type = B_SINGLE_SELECTION_LIST;
			else
			if(*str == "multiple")
				type = B_MULTIPLE_SELECTION_LIST;
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"SimpleList","selection mode must be : \"single or \"multiple"));
		
			str = scroll->Data();
			
			if(*str == "left")
				scr = SCROLL_LEFT;
			else
			if(*str == "right")
				scr = SCROLL_RIGHT;
			else
			if(*str == "none")
				scr = SCROLL_NONE;
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"SimpleList","Scrollbar must be : \"left, \"right or \"none"));					
					
			if(args->Length()>4)
			{			
				SQI_List *size = IsList((*args)[4]);
					
				if(size)
				{			
					if(args->Length()>5)
					{
						for(int i = 5;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
								
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"CheckBox","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"CheckBox","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);
				}
			}	
				
			return new SQI_BSimpleList(squirrel,items,var,scr,width,height,type,flags);
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"SimpleList","need : word word list"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"SimpleList","need at least three inputs"));	
}

/*
 * function    : SQI_GUI_IsSimpleList
 * purpose     : Builtin predicate "is.simplelist"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsSimpleList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsSimpleList((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.simplelist","need one input"));
}

/*
 * function    : SQI_GUI_StatusBar
 * purpose     : Builtin predicate "StatusBar"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_GUI_StatusBar(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<2)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"StatusBar","need two inputs at least"));
	else
	{
		SQI_List *size = IsList((*args)[0]);
		if(size)
		{
			SQI_String *str;
			const char *label = NULL,*trailing = NULL;
			int wt = -1;
			int wtr = -1;
			float max;
			SQI_Number *themax;
				
			List2Size(size,&wt,&wtr);
			
			themax = IsNumber((*args)[1]);
			if(themax)
				max = Number2Float(themax);
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"StatusBar","second input must be a number"));	
					
			
			if(args->Length()>=3)
			{
				str = IsString((*args)[2]);
				if(!str)
					str = IsKeyword((*args)[2]);
					
				if(str)
				{
					label = str->Data()->c_str();
					if(args->Length()==4)
					{
						str = IsString((*args)[3]);
						if(!str)
							str = IsKeyword((*args)[3]);
							
						if(str)
							trailing = str->Data()->c_str();	
						else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"StatusBar","second input must be a string or a word"));			
					}
				}
				else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"StatusBar","second input must be a string or a word"));			
			}
			
			
			return new SQI_BStatusBar(squirrel,label,trailing,wt,wtr,max);
		}
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"StatusBar","first input must be a list"));	
	}	
}
/*
 * function    : SQI_GUI_IsStatusBar
 * purpose     : Builtin predicate "is.statusbar"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsStatusBar(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsStatusBar((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.statusbar","need one input"));
}

/*
 * function    : SQI_GUI_Text
 * purpose     : Builtin predicate "Text"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Text(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>=1)
	{
		SQI_String *label = IsString((*args)[0]);
		if(!label)
			label = IsKeyword((*args)[0]);
			
		if(!label)	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Text","first input must be a string/word"));
									
		int width = -1;
		int height = -1;
		uint32 mode = B_FOLLOW_NONE;
		uint32 flags = B_WILL_DRAW;
		alignment align = B_ALIGN_LEFT;		
			
		if(args->Length()>1)
		{			
		
			SQI_Keyword *a = IsKeyword((*args)[1]);
			if(!a)
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Text","second input must be a word"));	
				
			string *str = a->Data();
			
			if(*str == "center")
				align = B_ALIGN_CENTER;
			else
			if(*str == "left")
				align = B_ALIGN_LEFT;
			else				
			if(*str == "right")
				align = B_ALIGN_RIGHT;
			else	
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Text","alignment must be a valid word : \"center , \"left or \"right"));	
				
			if(args->Length()>2)
			{
				SQI_List *size = IsList((*args)[2]);
				
				if(size)
				{			
					if(args->Length()>3)
					{
						for(int i = 3;i<args->Length();i++)
						{	
							SQI_Keyword *f = IsKeyword((*args)[i]);
							if(f)
							{
								string *s = f->Data();
						
								if(*s == "navigable")
									flags = flags | B_NAVIGABLE;
								else	
								if(*s == "navigable.jump")
									flags = flags | B_NAVIGABLE_JUMP;
								else
								if(*s == "pulsed")
									flags = flags | B_PULSE_NEEDED;
								else
									throw(new SQI_Exception(SQI_EX_INVALIDE,"Text","word is not a valide flag"));	
							}
							else
								throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Text","flag must be word"));
						}
					}
					
					// time to get the size from the list
					
					List2Size(size,&width,&height);
				}
			}
		}	
			
		return new SQI_BText(squirrel,label->Data()->c_str(),width,height,align,flags);
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Text","need at least one input"));	
}

/*
 * function    : SQI_GUI_IsText
 * purpose     : Builtin predicate "is.text"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsText(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsText((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.text","need one input"));
}

/*
 * function    : SQI_GUI_BarberPole
 * purpose     : Builtin predicate "BarberPole"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_GUI_BarberPole(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()>1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"BarberPole","need one input at least"));
	else
	{
		SQI_List *size = IsList((*args)[0]);
		if(size)
		{
			int direction = FROM_LEFT_TO_RIGHT;
			int width = -1;
			int height = -1;
			List2Size(size,&width,&height);
			
			if(args->Length()==2)
			{
				SQI_Keyword *dir = IsKeyword((*args)[1]);
				if(dir)
				{
					string *str = dir->Data();
					
					if(*str == "left")
						direction = FROM_RIGHT_TO_LEFT;
					else
					if(*str == "right")
						direction = FROM_LEFT_TO_RIGHT;
					else	
						throw(new SQI_Exception(SQI_EX_INVALIDE,"BarberPole","second input must be the word : \"left or \"right"));							
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"BarberPole","second input must be a word"));	
			}
			
			return new SQI_BBarberPole(squirrel,width,height,direction,B_FRAME_EVENTS);	
		}
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"BarberPole","input must be a list"));	
	}	
}

/*
 * function    : SQI_GUI_IsBarberPole
 * purpose     : Builtin predicate "is.barberPole"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_IsBarberPole(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()==1)
	{	
		if(IsBarberPole((*args)[0]))
			return interp->True;
		else
			return interp->False;	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.barberpole","need one input"));
}


/*
 * function    : SQI_GUI_Busy
 * purpose     : Builtin predicate "Busy"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_GUI_Busy(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	static bool spinning = true;
			
	if(args->Length()>=1)
	{	
		SQI_Number *flag = IsBool((*args)[0]);
		if(flag)
		{		
			AppBusy = flag->IsTrue();
				
			if(flag->IsTrue())
			{	// start busy	
				if(!Busy)
				{	
					if(args->Length()==2)
					{
						string *str;
						SQI_Keyword *w = IsKeyword((*args)[1]);
						if(!w)
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Busy","second input must be a word"));	
					
						str = w->Data();
						
						if(*str == "spinwheel")
							spinning = true;
						else
						if(*str == "watch")
							spinning = false;
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Busy","second input must be valid word"));							
					}	
					
					if(spinning)
					{
						Busy = new AnimateCursor("busy",4,0.2);
						Busy->SetCursor(0,Spinwheel0);
						Busy->SetCursor(1,Spinwheel45);
						Busy->SetCursor(2,Spinwheel90);
						Busy->SetCursor(3,Spinwheel135);
					}
					else
					{
						Busy = new AnimateCursor("busy",8,0.2);
						Busy->SetCursor(0,Watch0);	
						Busy->SetCursor(1,Watch1);
						Busy->SetCursor(2,Watch2);
						Busy->SetCursor(3,Watch3);
						Busy->SetCursor(4,Watch4);
						Busy->SetCursor(5,Watch5);
						Busy->SetCursor(6,Watch6);
						Busy->SetCursor(7,Watch7);	
					}	
				}
				else
				{
					// do we have to change the busy cursor ??
					if(args->Length()==2)
					{
						string *str;
						SQI_Keyword *w = IsKeyword((*args)[1]);
						if(!w)
							throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Busy","second input must be a word"));	
					
						str = w->Data();
								
						if(*str == "spinwheel")
						{		
							if(!spinning)
							{
								spinning = true;
								delete Busy;
								Busy = new AnimateCursor("busy",4,0.2);
								Busy->SetCursor(0,Spinwheel0);
								Busy->SetCursor(1,Spinwheel45);
								Busy->SetCursor(2,Spinwheel90);
								Busy->SetCursor(3,Spinwheel135);									
							}	
						}	
						else
						if(*str == "watch")
						{
							if(spinning)
							{
								spinning = false;
								delete Busy;
								Busy = new AnimateCursor("busy",8,0.2);
								Busy->SetCursor(0,Watch0);	
								Busy->SetCursor(1,Watch1);
								Busy->SetCursor(2,Watch2);
								Busy->SetCursor(3,Watch3);
								Busy->SetCursor(4,Watch4);
								Busy->SetCursor(5,Watch5);
								Busy->SetCursor(6,Watch6);
								Busy->SetCursor(7,Watch7);	
							}
						}	
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"Busy","second input must be valid word"));							
					}						
				}
			
				if(!Busy->IsRunning())
					Busy->Run();		
			}
			else
			{	// stop busy
				if(Busy)
				{
					if(Busy->IsRunning())
						Busy->Quit();	
					//delete Busy;
					//Busy = NULL;	
				}	
			}	
			
			return NULL;
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Busy","input must be boolean"));			
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Busy","need one input"));
}




/*
 * function    : SQI_GUI_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_GUI_Remove(SQI_BuiltInMap *Map)
{
	// remove FuncTable of each widget (if used)
	UnInitBWidget();
	UnInitBButton();
	UnInitBFrame();
	UnInitBBox(); 
	UnInitBCheckBox();
	UnInitBColorControl();
	UnInitBDropList();
	UnInitBEntry();	
	UnInitBMenu();
	UnInitBMenuBar();
	UnInitBMemo();
	UnInitBRadioButton();
	UnInitBSimpleList();
	UnInitBStatusBar();
	UnInitBText();
	UnInitBBarberPole();
	
	if(Busy)
		delete Busy;
	
	FreeCursors();
	
	Map->RemoveFunc("Font.init");	
	Map->RemoveFunc("Font.families");
	Map->RemoveFunc("Font.styles");
	Map->RemoveFunc("Font.exist");
	Map->RemoveFunc("Font");
	Map->RemoveFunc("is.font");
	Map->RemoveFunc("is.picture");

	Map->RemoveFunc("Hook");
	Map->RemoveFunc("Unhook");
	
	Map->RemoveFunc("Window");
	Map->RemoveFunc("is.window");
	Map->RemoveFunc("Glue");
	Map->RemoveFunc("Unglue");
	Map->RemoveFunc("Focus");
	Map->RemoveFunc("is.widget");
	Map->RemoveFunc("is.container");	
	
	Map->RemoveFunc("Button");
	Map->RemoveFunc("is.button");	
	
	Map->RemoveFunc("Frame");
	Map->RemoveFunc("is.frame");

	Map->RemoveFunc("Box");
	Map->RemoveFunc("is.box");

	Map->RemoveFunc("CheckBox");
	Map->RemoveFunc("is.checkbox");
	
	Map->RemoveFunc("Banner");
	Map->RemoveFunc("is.banner");
	
	Map->RemoveFunc("ColorControl");
	Map->RemoveFunc("is.colorcontrol");
	
	Map->RemoveFunc("DropList");
	Map->RemoveFunc("is.droplist");
	
	Map->RemoveFunc("Entry");
	Map->RemoveFunc("is.entry");
	
	Map->RemoveFunc("Menu");
	Map->RemoveFunc("is.menu");	
	
	Map->RemoveFunc("MenuBar");
	Map->RemoveFunc("is.menubar");
	
	Map->RemoveFunc("Memo");
	Map->RemoveFunc("is.memo");

	Map->RemoveFunc("RadioButton");
	Map->RemoveFunc("is.radiobutton");

	Map->RemoveFunc("SimpleList");
	Map->RemoveFunc("is.simplelist");
	
	Map->RemoveFunc("StatusBar");
	Map->RemoveFunc("is.statusbar");	

	Map->RemoveFunc("Text");
	Map->RemoveFunc("is.text");

	Map->RemoveFunc("BarberPole");
	Map->RemoveFunc("is.BarberPole");
	
	Map->RemoveFunc("Busy");

/*
	
	Map->RemoveFunc("Canvas");
	Map->RemoveFunc("is.canvas");			
	*/		
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
	InitSQIFont();	
	InitBWindow();
		
  	Map->AddAddOn(addon_id,GUI_Pkg_name,GUI_Pkg_purpose,GUI_Pkg_author,GUI_Pkg_version,GUI_Pkg_date,SQI_GUI_Remove);
  
  	Map->AddFunc("Font.init",
	       GUI_Pkg_name,
	       "Get from the system all the families font installed",
	       "Font.init",
	       SQI_GUI_FontInit
	       ); 
  
  	Map->AddFunc("Font.families",
	       GUI_Pkg_name,
	       "Output a list of all the families font installed",
	       "Font.families",
	       SQI_GUI_FontFamilies
	       );
	       
  	Map->AddFunc("Font.styles",
	       GUI_Pkg_name,
	       "Output a list of all the styles of a family",
	       "Font.styles string",
	       SQI_GUI_FontStyles
	       );
	       
	Map->AddFunc("Font.exists",
	       GUI_Pkg_name,
	       "Output true if the font specified is installed",
	       "Font.exists string",
	       SQI_GUI_FontExist
	       );
	       
	Map->AddFunc("Font",
	       GUI_Pkg_name,
	       "Output a new font object",
	       "Font string/word (style)",
	       SQI_GUI_Font
	       );
	       
	Map->AddFunc("is.font",
	       GUI_Pkg_name,
	       "Output true if the input is a font",
	       "is.font thing",
	       SQI_GUI_IsFont
	       );      
	       	       
	Map->AddFunc("Hook",
	       GUI_Pkg_name,
	       "Hook an event to a action for a window or widget",
	       "Hook object event action input1 input2 ....",
	       SQI_GUI_Hook
	       );
	       
	Map->AddFunc("Unhook",
	       GUI_Pkg_name,
	       "Unhook an event for a window or widget",
	       "Unhook object event",
	       SQI_GUI_UnHook
	       );
	       
	Map->AddFunc("Window",
	       GUI_Pkg_name,
	       "Create a new window object",
	       "Window type title position",
	       SQI_GUI_Window
	       );
	       
	Map->AddFunc("is.window",
	       GUI_Pkg_name,
	       "Output true if the input is a window object",
	       "is.window thing",
	       SQI_GUI_IsWindow
	       );
	       
	Map->AddFunc("Glue",
	       GUI_Pkg_name,
	       "Add to a container (Window or view) view objects",
	       "Glue container vertical_alignment [pading] thing1 thing2 ...",
	       SQI_GUI_Glue
	       );
	       
	Map->AddFunc("Unglue",
	       GUI_Pkg_name,
	       "Remove from a container (Window or view) view objects",
	       "Unglue thing1 thing2 ...",
	       SQI_GUI_UnGlue
	       );
	       
	Map->AddFunc("Focus",
	       GUI_Pkg_name,
	       "Give the focus to the widget given as input",
	       "Focus widget",
	       SQI_GUI_Focus
	       );	       
	       
	Map->AddFunc("is.widget",
	       GUI_Pkg_name,
	       "Output true if the input is a widget object",
	       "is.widget thing",
	       SQI_GUI_IsWidget
	       );
	       	       
	Map->AddFunc("is.container",
	       GUI_Pkg_name,
	       "Output true if the input is a window or a widget constainer object",
	       "is.container thing",
	       SQI_GUI_IsContainer
	       );	       
	             
	Map->AddFunc("Box",
	       GUI_Pkg_name,
	       "Create a new box object",
	       "Box label [width height] flags ...",
	       SQI_GUI_Box
	       ); 
	       
	Map->AddFunc("is.box",
	       GUI_Pkg_name,
	       "Output true if the input is a box object",
	       "is.box thing",
	       SQI_GUI_IsBox
	       );      
	       	       
	Map->AddFunc("Button",
	       GUI_Pkg_name,
	       "Create a new button object",
	       "Button label [width height] flags ...",
	       SQI_GUI_Button
	       );
	       
	Map->AddFunc("is.button",
	       GUI_Pkg_name,
	       "Output true if the input is a button object",
	       "is.button thing",
	       SQI_GUI_IsButton
	       );
	       	  	       
	Map->AddFunc("CheckBox",
	       GUI_Pkg_name,
	       "Create a new checkbox object",
	       "CheckBox label variable [width height] flags ...",
	       SQI_GUI_CheckBox
	       );
	       
	Map->AddFunc("is.checkbox",
	       GUI_Pkg_name,
	       "Output true if the input is a checkbox object",
	       "is.checkbox thing",
	       SQI_GUI_IsCheckBox
	       );
	       		       	  	       
	Map->AddFunc("ColorControl",
	       GUI_Pkg_name,
	       "Create a new colorcontrol object",
	       "ColorControl layout cellside variable",
	       SQI_GUI_ColorControl
	       ); 
	       
	Map->AddFunc("is.colorcontrol",
	       GUI_Pkg_name,
	       "Output true if the input is a colorcontrol object",
	       "is.colorcontrol thing",
	       SQI_GUI_IsColorControl
	       );
	      	       	  	       
	Map->AddFunc("SimpleList",
	       GUI_Pkg_name,
	       "Create a new simplelist object",
	       "SimpleList mode variable content [width height] flags ...",
	       SQI_GUI_SimpleList
	       );
	       
	Map->AddFunc("is.simplelist",
	       GUI_Pkg_name,
	       "Output true if the input is a simplelist object",
	       "is.simplelist thing",
	       SQI_GUI_IsSimpleList
	       );
	     	       	  	       
	Map->AddFunc("Frame",
	       GUI_Pkg_name,
	       "Create a new frame object",
	       "Frame style [width height] flags ...",
	       SQI_GUI_Frame
	       );       
	       
	Map->AddFunc("is.frame",
	       GUI_Pkg_name,
	       "Output true if the input is a frame object",
	       "is.frame thing",
	       SQI_GUI_IsFrame
	       );

	Map->AddFunc("DropList",
	       GUI_Pkg_name,
	       "Create a new droplist object",
	       "DropList label variable content [width height] flags ...",
	       SQI_GUI_DropList
	       );
	       
	Map->AddFunc("is.droplist",
	       GUI_Pkg_name,
	       "Output true if the input is a droplist object",
	       "is.droplist thing",
	       SQI_GUI_IsDropList
	       );
	       
	Map->AddFunc("MenuBar",
	       GUI_Pkg_name,
	       "Create a new menubar",
	       "MenuBar",
	       SQI_GUI_MenuBar
	       );
	       
	Map->AddFunc("is.menubar",
	       GUI_Pkg_name,
	       "Output true if the input is a menubar object",
	       "is.menubar thing",
	       SQI_GUI_IsMenuBar
	       );
	       
	Map->AddFunc("Menu",
	       GUI_Pkg_name,
	       "Create a new menu",
	       "Menu label",
	       SQI_GUI_Menu
	       );
	       
	Map->AddFunc("is.menu",
	       GUI_Pkg_name,
	       "Output true if the input is a menu object",
	       "is.menu thing",
	       SQI_GUI_IsMenu
	       );
	       	       
	Map->AddFunc("RadioButton",
	       GUI_Pkg_name,
	       "Create a new radiobutton object",
	       "RadioButton label variable [width height] flags ...",
	       SQI_GUI_RadioButton
	       );
	       
	Map->AddFunc("is.radiobutton",
	       GUI_Pkg_name,
	       "Output true if the input is a radiobutton object",
	       "is.radiobutton thing",
	       SQI_GUI_IsRadioButton
	       );
	             
	Map->AddFunc("Text",
	       GUI_Pkg_name,
	       "Create a new text widget",
	       "Text string [width height] flags ...",
	       SQI_GUI_Text
	       );
	       
	Map->AddFunc("is.text",
	       GUI_Pkg_name,
	       "Output true if the input is a text object",
	       "is.text thing",
	       SQI_GUI_IsText
	       );
	        
	Map->AddFunc("Banner",
	       GUI_Pkg_name,
	       "Create a new banner widget",
	       "Banner variable [width height] flags ...",
	       SQI_GUI_Banner
	       );
	       
	Map->AddFunc("is.banner",
	       GUI_Pkg_name,
	       "Output true if the input is a banner object",
	       "is.banner thing",
	       SQI_GUI_IsBanner
	       );
	        
	Map->AddFunc("Entry",
	       GUI_Pkg_name,
	       "Create a new entry object",
	       "Entry label variable [width_label width_entry] flags ...",
	       SQI_GUI_Entry
	       );
	       
	Map->AddFunc("is.entry",
	       GUI_Pkg_name,
	       "Output true if the input is an entry object",
	       "is.entry thing",
	       SQI_GUI_IsEntry
	       );
	       	       
	Map->AddFunc("Memo",
	       GUI_Pkg_name,
	       "Create a new memo widget",
	       "Memo vertical horizontal variable [width height] flags ...",
	       SQI_GUI_Memo
	       );
	       
	Map->AddFunc("is.memo",
	       GUI_Pkg_name,
	       "Output true if the input is an memo widget",
	       "is.memo thing",
	       SQI_GUI_IsMemo
	       );
	      
	Map->AddFunc("StatusBar",
	       GUI_Pkg_name,
	       "Create a new StatusBar widget",
	       "StatusBar [width height] (label (trailing))",
	       SQI_GUI_StatusBar
	       );
	       
	Map->AddFunc("is.statusbar",
	       GUI_Pkg_name,
	       "Output true if the input is a statusbar widget",
	       "is.statusbar thing",
	       SQI_GUI_IsStatusBar
	       );	       	      
/*	      
	       
	Map->AddFunc("Canvas",
	       GUI_Pkg_name,
	       "Create a new Canvas widget",
	       "Canvas size",
	       SQI_GUI_Canvas
	       );
	       	          
	Map->AddFunc("is.canvas",
	       GUI_Pkg_name,
	       "Output true if the input is a Canvas widget",
	       "is.canvas thing",
	       SQI_GUI_IsCanvas
	       );
	       
	       */
	       
	Map->AddFunc("BarberPole",
	       GUI_Pkg_name,
	       "Create a new BarberPole widget",
	       "StatusBar [width height] (direction)",
	       SQI_GUI_BarberPole
	       );
	          
	Map->AddFunc("is.barberpole",
	       GUI_Pkg_name,
	       "Output true if the input is a BarberPole widget",
	       "is.barberpole thing",
	       SQI_GUI_IsBarberPole
	       );
	       
	Map->AddFunc("Busy",
	       GUI_Pkg_name,
	       "Set or unset the busy cursor over the application",
	       "Busy boolean",
	       SQI_GUI_Busy
	       );	                      	       	  	       	       	 	       	  	       	              
}
