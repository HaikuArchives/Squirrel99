/*
 * Squirrel project
 *
 * file ?	sqi-window.cpp
 * what	?   BWindow object
 * who	?	jlv
 * when	?	11/22/99
 * last	?	03/12/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include <math.h> 
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-glue.h"
#include "SQI-bwindow.h" 
#include "SQI-utils.h"
#include "Hook.h"
#include "Threading.h"
#include "AnimateCursor.h"

#include <Screen.h>
#include <Application.h>
#include <MessageFilter.h>
#include <MenuBar.h>

#define MSG_WINDOW_OUT 	'MWO'
#define MENU_CALLBACK	'MenU'

extern AnimateCursor *Busy;

string Hooks_Window[HOOKS_WINDOW] = {
	string("enter"),
	string("leave"),
	string("maximize"),
	string("message"),
	string("minimize"),
	string("move"),
	string("quit"),
	string("resize"),
	string("screenchange"),
	string("workspaceactivate"),
	string("workspacechange"),
	string("zoom")
};

filter_result filter(BMessage *message,BHandler **target,BMessageFilter *messageFilter)
{
	if(message->what == B_KEY_UP || message->what == B_KEY_DOWN)
	{
		uint32 mod = message->FindInt32("modifiers");
		int32 key = message->FindInt32("key");	
		
		if(mod == 0x402 && key == 0x27)
		{
			// add a src field
			BMessage *msg = new BMessage(B_QUIT_REQUESTED);
			msg->AddPointer("src",messageFilter->Looper());
			be_app->PostMessage(msg);
			return B_SKIP_MESSAGE;
			//return B_DISPATCH_MESSAGE;					
		}
		else
			return B_DISPATCH_MESSAGE;		
	}
	else
		return B_DISPATCH_MESSAGE;	
}

/*
 * function    : SQI_Window
 * purpose     : Constructor
 * input       : 
 *
 * SQI_BWindow *ptr, ptr to the SQI_BWindow object
 * BRect frame, frame of the window
 * char *title, title of the window
 * window_type type, type of the window
 * int32 flags, flags
 *
 * output      : none
 * side effect : none
 */
SQI_Window::SQI_Window(SQI_BWindow *ptr,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags)
	:BWindow(frame,title,look,feel,flags)
{	
	// We set all the hook to NULL
	for(int i=0;i<HOOKS_WINDOW;i++)
	{
		Hooks[i].action=NULL;
		Hooks[i].extra=NULL;
	}	
	
	constraint = AUTO;
	restore = false;
	BScreen screen(NULL);
	zoom_max_w = screen.Frame().Width();
	zoom_max_h = screen.Frame().Height();	

	MagicPtr = ptr;
	
	RealBounds = empty;
	
	resized_count = 0;
	
	minw = 0;
	minh = 0;
	maxw = zoom_max_w;
	maxh = zoom_max_h;
	SetSizeLimits(0,zoom_max_w,0,zoom_max_h);
	
	RemoveShortcut('W', B_COMMAND_KEY); 
	//RemoveShortcut('Q', B_COMMAND_KEY); 
	
	// set message filter
	AddCommonFilter(new BMessageFilter(B_ANY_DELIVERY,B_LOCAL_SOURCE,filter));
}


/*
 * function    : ~SQI_BWindow
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Window::~SQI_Window()
{			
}

/*
 * function    : Quit
 * purpose     : Quit the window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Window::Quit(void)
{				
	SetPulseRate(0);

	Lock();
		
	if(MagicPtr)
	{
		MagicPtr->Terminate();	// remove all attached widget
		MagicPtr->Window = NULL;
	}
						
	// we remove the hook
		
	for(int i=0;i<HOOKS_WINDOW;i++)
	{
		if(Hooks[i].action)
		{
			Hooks[i].action->RemRef();	
			if(Hooks[i].extra)
			{
				list<SQI_Object *>::iterator j;
			
				for(j=Hooks[i].extra->begin();j!=Hooks[i].extra->end();j++)
					(*j)->RemRef(true);
				
				delete Hooks[i].extra;
			}
		}
	}
	
	Unlock();
		
	//snooze(50000);
									
	BWindow::Quit();	
}

// the hooks

/*
 * function    : QuitRequested
 * purpose     : Quit the window
 * input       : none
 * output      : bool, true if we quit , false else
 * side effect : none
 */
bool SQI_Window::QuitRequested(void)
{		
	// if we have a hook, we going to execute it
	if(Hooks[HOOK_WIN_QUITREQUESTED].action)
	{		
		// we need to create a list of value for the Hook which contain the input of the function	
	
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
				
		args->push_back(MagicPtr);
				
		MagicPtr->AddRef();	
				
		AddExtra(args,Hooks[HOOK_WIN_QUITREQUESTED].extra);
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIN_QUITREQUESTED].action,args);
						
		delete args;
				
		// in this case , we check the return value
		if(!out)
			return true;
		else
		{
			SQI_Number *num = IsNumber(out);
			if(num)
			{
				if(num->IsTrue())
				{
					if(Busy)
						if(Busy->IsRunning())
							Busy->Quit();
					return true;
				}	
				else
					return false;
			}
			else
			{
				if(Busy)
					if(Busy->IsRunning())
						Busy->Quit();
				return true;
			}	
		}	
	}	
		
	if(Busy)
		if(Busy->IsRunning())
			Busy->Quit();		
	return true;		
} 

/*
 * function    : WindowActivated
 * purpose     : Called when window became active of inactive (user mouse)
 * input       : 
 *
 * bool active, true if active, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::WindowActivated(bool active)
{	
	if(active)
	{		
		// we have a workaround the the restore bug of the BWindow
		if(restore)
		{
			restore = false;
			// if we have a hook, we going to execute it
			if(Hooks[HOOK_WIN_MAXIMIZE].action)
			{
				// we need to create a list of value for the Hook which contain the input of the function	
			
				list<SQI_Object *> *args = new 	list<SQI_Object *>;
	
				args->push_back(MagicPtr);
				MagicPtr->AddRef();
				
				AddExtra(args,Hooks[HOOK_WIN_MAXIMIZE].extra);
		
				SQI_Object *out = UseHook(Hooks[HOOK_WIN_MAXIMIZE].action,args);
				
				delete args;
			}		
		}
		else
		{			
			// if we have a hook, we going to execute it
			if(Hooks[HOOK_WIN_ENTER].action)
			{
				// we need to create a list of value for the Hook which contain the input of the function	
			
				list<SQI_Object *> *args = new 	list<SQI_Object *>;
	
				args->push_back(MagicPtr);
				MagicPtr->AddRef();
				
				AddExtra(args,Hooks[HOOK_WIN_ENTER].extra);
		
				SQI_Object *out = UseHook(Hooks[HOOK_WIN_ENTER].action,args);
				
				delete args;	
			}
		}	
	}
	else
	{
		// if we have a hook, we going to execute it
		if(Hooks[HOOK_WIN_LEAVE].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
		
			list<SQI_Object *> *args = new 	list<SQI_Object *>;

			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			AddExtra(args,Hooks[HOOK_WIN_LEAVE].extra);
	
			SQI_Object *out = UseHook(Hooks[HOOK_WIN_LEAVE].action,args);
			
			delete args;
		}			
	}
		
	BWindow::WindowActivated(active);		
}

/*
 * function    : Minimize
 * purpose     : Called when window is minimized or restored
 * input       : 
 *
 * bool active, true if minimized, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::Minimize(bool minimize)
{	
	if(minimize)
	{
		restore = true;
		// if we have a hook, we going to execute it
		if(Hooks[HOOK_WIN_MINIMIZE].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
		
			list<SQI_Object *> *args = new 	list<SQI_Object *>;

			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			AddExtra(args,Hooks[HOOK_WIN_MINIMIZE].extra);
	
			SQI_Object *out = UseHook(Hooks[HOOK_WIN_MINIMIZE].action,args);
			
			delete args;
		}	
	}
	else
	{
		// if we have a hook, we going to execute it
		if(Hooks[HOOK_WIN_MAXIMIZE].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
		
			list<SQI_Object *> *args = new 	list<SQI_Object *>;

			args->push_back(MagicPtr);
			MagicPtr->AddRef();
			
			AddExtra(args,Hooks[HOOK_WIN_MAXIMIZE].extra);
	
			SQI_Object *out = UseHook(Hooks[HOOK_WIN_MAXIMIZE].action,args);
			
			delete args;
		}			
	}
		
	BWindow::Minimize(minimize);		
}

/*
 * function    : FrameMoved
 * purpose     : Called when window is moved
 * input       : 
 *
 * BPoint screenPoint, new position
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::FrameMoved(BPoint screenPoint)
{
	if(Hooks[HOOK_WIN_MOVED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		SQI_Number *x = new SQI_Number(MagicPtr->Squirrel->LocalHeap,screenPoint.x);
		SQI_Number *y = new SQI_Number(MagicPtr->Squirrel->LocalHeap,screenPoint.y);			
		
		args->push_back(MagicPtr);
		args->push_back(x);
		args->push_back(y);		
		
		MagicPtr->AddRef();
		x->AddRef();
		y->AddRef();
		
		AddExtra(args,Hooks[HOOK_WIN_MOVED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIN_MOVED].action,args);
		
		delete args;	
	}
	
	BWindow::FrameMoved(screenPoint);		
}

/*
 * function    : FrameResized
 * purpose     : Called when window is resized
 * input       : 
 *
 * float width, new width
 * float height, new height
 *
 * output      : none
 * side effect : none
 */		
void SQI_Window::FrameResized(float width,float height)
{
	if(!resized_count)
	{	
	//if(resized_by == RESIZED_BY_USER)
	//{		
		if(Hooks[HOOK_WIN_RESIZED].action)
		{
			// we need to create a list of value for the Hook which contain the input of the function	
			
			list<SQI_Object *> *args = new 	list<SQI_Object *>;
			SQI_Number *w = new SQI_Number(MagicPtr->Squirrel->LocalHeap,width);
			SQI_Number *h = new SQI_Number(MagicPtr->Squirrel->LocalHeap,height);			
			
			args->push_back(MagicPtr);
			args->push_back(w);
			args->push_back(h);		
			
			MagicPtr->AddRef();
			w->AddRef();
			h->AddRef();
			
			AddExtra(args,Hooks[HOOK_WIN_RESIZED].extra);	
			
			SQI_Object *out = UseHook(Hooks[HOOK_WIN_RESIZED].action,args);
			
			delete args;	
		}
		
		BWindow::FrameResized(width,height);
		if(Bounds() != RealBounds)	
			StaticGluing();
	}
	else
	{
		resized_count--;
		if(resized_count<0)
			resized_count = 0;
		BWindow::FrameResized(width,height);
	}
}

/*
 * function    : WorkspaceActivated
 * purpose     : the active workspace had changed
 * input       : 
 *
 * int32 workspace, workspace
 * bool active, if true the workspace is the active, false else
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::WorkspaceActivated(int32 workspace,bool active)
{
	if(Hooks[HOOK_WIN_WORKSPACEACTIVATED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		SQI_Number *w = new SQI_Number(MagicPtr->Squirrel->LocalHeap,workspace+1);
		SQI_Number *a = new SQI_Number(MagicPtr->Squirrel->LocalHeap,active);			
		
		args->push_back(MagicPtr);
		args->push_back(w);
		args->push_back(a);		
		
		MagicPtr->AddRef();
		w->AddRef();
		a->AddRef();
		
		AddExtra(args,Hooks[HOOK_WIN_WORKSPACEACTIVATED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIN_WORKSPACEACTIVATED].action,args);
		
		delete args;
	}
	
	BWindow::WorkspaceActivated(workspace,active);	
}

/*
 * function    : WorkspaceChanged
 * purpose     : the window had been moved in a new workspace
 * input       : 
 *
 * uint32 oldWorkspaces, old
 * uint32 newWorkspaces, new
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::WorkspacesChanged(uint32 oldWorkspaces,uint32 newWorkspaces)
{
	if(Hooks[HOOK_WIN_WORKSPACECHANGED].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		SQI_Number *o = new SQI_Number(MagicPtr->Squirrel->LocalHeap,(int)(oldWorkspaces));
		SQI_Number *n = new SQI_Number(MagicPtr->Squirrel->LocalHeap,(int)(newWorkspaces));			
		
		args->push_back(MagicPtr);
		args->push_back(o);
		args->push_back(n);		
		
		MagicPtr->AddRef();
		o->AddRef();
		n->AddRef();
		
		AddExtra(args,Hooks[HOOK_WIN_WORKSPACECHANGED].extra);	
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIN_WORKSPACECHANGED].action,args);
		
		delete args;	
	}
	
	BWindow::WorkspacesChanged(oldWorkspaces,newWorkspaces);	
}

/*
 * function    : Zoom
 * purpose     : the window is zoomed or unzoomed
 * input       : 
 *
 * BPoint leftTop, new position of the window
 * float width, new width
 * float height, new height
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::Zoom(BPoint leftTop,float width,float height)
{
	if(Hooks[HOOK_WIN_ZOOM].action)
	{
		// we need to create a list of value for the Hook which contain the input of the function	
		
		list<SQI_Object *> *args = new 	list<SQI_Object *>;
		SQI_Number *x = new SQI_Number(MagicPtr->Squirrel->LocalHeap,leftTop.x);
		SQI_Number *y = new SQI_Number(MagicPtr->Squirrel->LocalHeap,leftTop.y);
		SQI_Number *w = new SQI_Number(MagicPtr->Squirrel->LocalHeap,width);
		SQI_Number *h = new SQI_Number(MagicPtr->Squirrel->LocalHeap,height);			
		
		args->push_back(MagicPtr);
		args->push_back(x);
		args->push_back(y);
		args->push_back(w);
		args->push_back(h);				
		
		MagicPtr->AddRef();
		x->AddRef();
		y->AddRef();
		w->AddRef();
		h->AddRef();	
		
		AddExtra(args,Hooks[HOOK_WIN_ZOOM].extra);		
		
		SQI_Object *out = UseHook(Hooks[HOOK_WIN_ZOOM].action,args);
		
		delete args;
	}
	
	BWindow::Zoom(leftTop,width,height);	
}

/*
 * function    : MessageReceived
 * purpose     : We received a message
 * input       : 
 *
 * BMessage *message, the message
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::MessageReceived(BMessage *message)
{		
	switch(message->what)
	{
		case MENU_CALLBACK:
		{
			if(message->HasPointer("callback"))
			{
				THook *hook;
				
				message->FindPointer("callback",(void **)(&hook));
				if(hook->extra)
				{
					// copy the args as the list will be destroyed later
					list<SQI_Object *> *args = new list<SQI_Object *>;
					list<SQI_Object *>::iterator i;
						
					for(i=hook->extra->begin();i!=hook->extra->end();i++)
					{
						(*i)->AddRef();					
						args->push_back(*i);
					}
					
					UseHook(hook->action,args,true);
					snooze(50000); // snooze to let the hook thread run first
				}
				else	
					UseHook(hook->action,NULL,true);	
			}	
			
			break;	
		}
		default:	
			BWindow::MessageReceived(message);	
	}	
}

// Some used methods

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
bool SQI_Window::Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra)
{
	bool find = false;
	int index=0;
	int code;
	string *e = event->Data();

	// first we need to find the index in the array of the hook
	
	for(index=0;index<HOOKS_WINDOW;index++)
	{
			code = e->compare(Hooks_Window[index]);
			if(!code)
			{
				find = true;
				break;	
			}
			else
				if(code<0)
					break;
	}
	
	if(find)
	{
		// if we allready have a hook , we erase it
		
		if(Hooks[index].action)
		{
			Hooks[index].action->RemRef(true);	
			if(Hooks[index].extra)
				{
					list<SQI_Object *>::iterator i;
					
					for(i=Hooks[index].extra->begin();i!=Hooks[index].extra->end();i++)
						(*i)->RemRef(true);
						
					delete Hooks[index].extra;
					Hooks[index].extra = NULL;
				}
		}
		
		// we have the hook , now we add the hook action to this hook entry
		
		SQI_nBlock *block = IsBlock(action);
		if(block)
		{
			block->Export(MagicPtr->Squirrel->LocalHeap);
			block->AddRef();
			Hooks[index].action = block;
			
		}
		else
		{	
			// we have the name of the function to call	 so we create a nUfunc
			SQI_Keyword *name = IsKeyword(action);	
		
			Hooks[index].action = new SQI_nUfunc(MagicPtr->Squirrel->LocalHeap,name->Data());
			Hooks[index].action->AddRef();
			Hooks[index].extra = extra;
			if(extra)
			{
				list<SQI_Object *>::iterator i;
						
				for(i=Hooks[index].extra->begin();i!=Hooks[index].extra->end();i++)
					(*i)->Export(MagicPtr->Squirrel->LocalHeap);
			}
		}	
		
		return true;
	}
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
bool SQI_Window::UnHook(SQI_Keyword *event)
{
	bool find = false;
	int index=0;
	int code;
	string *e = event->Data();

	// first we need to find the index in the array of the hook
	
	for(index=0;index<HOOKS_WINDOW;index++)
	{
			code = e->compare(Hooks_Window[index]);
			if(!code)
			{
				find = true;
				break;	
			}
			else
				if(code<0)
					break;
	}
	
	if(find)
	{
		// if we allready have a hook , we erase it
		
		if(Hooks[index].action)
		{
			Hooks[index].action->RemRef(true);	
			Hooks[index].action = NULL;
			if(Hooks[index].extra)
			{
				list<SQI_Object *>::iterator i;
				
				for(i=Hooks[index].extra->begin();i!=Hooks[index].extra->end();i++)
					(*i)->RemRef(true);
					
				delete Hooks[index].extra;
				Hooks[index].extra = NULL;
			}	
		}
		
		return true;
	}
	else
		return false;		
}

/*
 * function    : UseHook
 * purpose     : Use a hook (execute the hook with the squirrel)
 * input       : 
 *
 * SQI_Node *action , hook action to execute
 * list<SQI_Object *> *args, input of the hook
 *
 * output      : SQI_Object *, object returned by the action
 * side effect : none
 */
SQI_Object *SQI_Window::UseHook(SQI_Node *action,list<SQI_Object *> *args,bool spawn = false)
{
	switch(action->IsA())
	{
		case SQI_TNODE_BLOCK: 		// if we have a block
		{
			// we need to do a remref on all the element of args
			
			if(args)
			{
				list<SQI_Object *>::const_iterator i = args->begin();
				list<SQI_Object *>::const_iterator e = args->end();
	
				for(i;i!=e;i++)
					(*i)->RemRef();		
			}
			
			if(!spawn)
			{
				try
				{
					SQI_nBlock *block = dynamic_cast<SQI_nBlock *>(action);
					return MagicPtr->Squirrel->HopOnABlock(MagicPtr->Interpreter,block);
				} catch (SQI_Exception *ex) {
					string *str = ex->Print();
					*(MagicPtr->Interpreter->Stream->err) << *str << "\n" << '\0';
					MagicPtr->Interpreter->Stream->Flush();
					delete str;
					delete ex;
					return NULL;		
				}
			}
			else // spawn a thread to execute the hook
			{
				action->AddRef();
				resume_thread(SpawnThread("hook",action,MagicPtr->Squirrel,B_NORMAL_PRIORITY));	
				return NULL;
			}
			
			break;
		}
		case SQI_TNODE_USERFUN:			// if we have a function call
		{
			SQI_Object *ret = NULL;
			SQI_nUfunc *func = 	dynamic_cast<SQI_nUfunc *>(action);
			
			if(!spawn)
			{
				try
				{
					func->SetArgs(args);
					ret = MagicPtr->Squirrel->HopOnAUfunc(MagicPtr->Interpreter,func);
					func->DelArgs();
				} catch (SQI_Exception *ex) {
					func->DelArgs();
					string *str = ex->Print();
					*(MagicPtr->Interpreter->Stream->err) << *str << "\n" << '\0';
					MagicPtr->Interpreter->Stream->Flush();
					delete str;
					delete ex;
					ret = NULL;		
				}
			}
			else // spawn a thread to execute the hook
			{
				action->AddRef();
				resume_thread(SpawnThread("hook",action,MagicPtr->Squirrel,B_NORMAL_PRIORITY,args));	
				return NULL;
			}			
			
			return ret;	
		}
		default:
			return NULL;
	}	
}

// Gluing methods

/*
 * function    : DynamicGluing
 * purpose     : Glue a new set of views and resize the window according
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Window::DynamicGluing(char align,list<SQI_BWidget *> *widgets)
{	
	resized_by = RESIZED_BY_GLUING;	
				
	DisableUpdates();
	
	// first we hide all the view attached to the window	
		
	// now we seek through all the view to pack and we pack them	
			
	list<SQI_BWidget *>::const_iterator i;	
	SQI_BWidget *widget;
	BRect f;
	float horizontal,vertical,x,y;
		
	for(i=widgets->begin();i!=widgets->end();i++)
	{	
		// first thing, we check if we have already glued something	
				
		if(RealBounds == empty)
		{

			// first gluing so it's rather simple
				
			widget = *i;
																	
			// and we place the view to it position	
		
			widget->VMoveTo(0,0);
			
			// we get the frame size of the widget with padding
			
			f = widget->VTrueFrame();
																	
			RealBounds = f;	
		}
		else
		{	
			// something has been allready glued so it's more compicated
			
			switch(align)
			{
				case ALIGN_TOP:
				{
					// GLUE on top	
									
					widget = *i;
						
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
							
					// we going to increase the RealBounds of what is need
					
					if(f.Width() > RealBounds.Width())
						horizontal = f.Width() - RealBounds.Width() + 1;
					else
						horizontal = 0;
							
					if(f.Height()>0)						
						vertical = f.Height() + 1;
					else
						vertical = 0;	
												
					if(vertical>0 || horizontal>0)
					{ 																																		
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																															
						// we move all the view down
						
						MoveByChilds(MagicPtr->Contents,0,vertical);
						
						// now we add the view to the top	
						
						widget->VMoveTo(0,0);
					}
							
					break;	
				}
				case ALIGN_BOTTOM:
				{
					// glue on bottom
					
					widget = *i;
					
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
							
					// we going to increase the RealBounds of what is need
					
						if(f.Width() > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 1;
						else
							horizontal = 0;
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
						
						if(vertical>0 || horizontal>0)
						{ 			
							y = RealBounds.bottom + 1;	
													
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
								
							// we move the view to the bottom
																	
							widget->VMoveTo(0,y);
						}
				
					break;	
				}
				case ALIGN_LEFT:
				{
					// glue on left
					
					widget = *i;
					
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
							
					// we going to increase the RealBounds of what is need
						if(f.Height() > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + 1;
						else
							vertical = 0;
						if(f.Width()>0)
							horizontal = f.Width() + 1;	
						else
							horizontal = 0;
														
						if(horizontal>0 || vertical>0)
						{								
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																					
							// we move all the view right
							
							MoveByChilds(MagicPtr->Contents,f.Width()+1,0);
							
							// now we add the view to the top/left	
							
							widget->VMoveTo(0,0);
						}		
					
					break;	
				}
				case ALIGN_RIGHT:
				{
					// glue on right
					
					widget = *i;
										
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
							
					// we going to increase the RealBounds of what is need
					
					if(f.Height() > RealBounds.Height())
						vertical = f.Height() - RealBounds.Height() + 1;
					else
						vertical = 0;
					if(vertical<0)
						vertical = 0;
					if(f.Width()>0)
						horizontal = f.Width() + 1;	
					else
						horizontal = 0;
					
					if(horizontal>0 || vertical>0)
					{							
						x = RealBounds.right + 1;
										
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																
						// now we add the view to the top/left	
												
						widget->VMoveTo(x,0);
					}
											
					break;	
				}				
			}				
	
		}
	
		// we save the new bounds of the window		
	}	
		
	// we resize the window
					
	resized_count++;
	
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();
		
	// we change the min size limit to the actual size of the Window

	if(constraint==AUTO)
	{				
		//GetSizeLimits(&minw,&maxw,&minh,&maxh);	
		minw = Frame().Width();
		minh = Frame().Height();	
		
		SetSizeLimits(minw,maxw,minh,maxh);
	}	

	// Time to see what could be expanded
	
	ExpandChilds();
		
	resized_by = RESIZED_BY_USER;	
		
	EnableUpdates();
}

/*
 * function    : StaticGluing
 * purpose     : The frame of the window have been changed by the user, we re glue everything
 * input       : 
 *
 * float width, new width of the window
 * float height, new height of the window
 * 
 * output      : none
 * side effect : none
 */
void SQI_Window::StaticGluing()
{
	// if no children, don't need to do anything
	
	if(!CountChildren())
		return;	
		
	// first we hide all the view attached to the window
	
	DisableUpdates();	
					
	// we get the Bounds of the window	
	
	RealBounds = Bounds();	
		
	// we now glue everything
	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *>::const_iterator j;
	list<SQI_BWidget *> *widgets;
	SQI_BWidget *widget;
	BRect f;	
	float x,y;
	
	Childs = MagicPtr->Contents;	
	
	double area = 0;
	
	for(i=Childs->begin();i!=Childs->end();i++)
	{
		widgets = (*i)->elements;
		switch((*i)->alignment)
		{
			case ALIGN_TOP:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->VResizeToPreferred(); // remove V	
						
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	// remove V
																		
					// we move all the view aligner TOP, down
						
					MoveByChilds(MagicPtr->Contents,0.0,f.Height()+1,(char)ALIGN_BOTTOM);
										
					// now we add the view to the top	
					
					widget->VMoveTo(0,0);		
				}
				
				break;
			}	
			case ALIGN_BOTTOM:
			{	
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->VResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
											
					// we move all the view whom are aligned bottom, up
					
					MoveByChilds(MagicPtr->Contents,ALIGN_BOTTOM,0,-f.Height()-1);
										
					y = RealBounds.bottom -f.Height();
															
					widget->VMoveTo(0,y);
				}		
				
				break;		
			}	
			case ALIGN_RIGHT:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->VResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
																						
					// now we add the view to the top/left	
					
					MoveByChilds(MagicPtr->Contents,(char)ALIGN_RIGHT,-f.Width()-1,0.0);
						
					x = RealBounds.right - f.Width();
										
					widget->VMoveTo(x,0);	
				}
				
				break;	
			}	
			case ALIGN_LEFT:
			{
				for(j=widgets->begin();j!=widgets->end();j++)
				{
					widget = *j;
					
					// first we need to resize the widget to his prefered size
					
					widget->VResizeToPreferred();
					
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
																						
					// we move all the view right
					
					MoveByChilds(MagicPtr->Contents,f.Width()+1,0,ALIGN_RIGHT);
					
					// now we add the view to the top/left	
					
					widget->VMoveTo(0,0);					
				}
				
				break;				
			}	
		}
	}
	
	// Time to see what could be expanded
	
	ExpandChilds();
		
	EnableUpdates();
}

/*
 * function    : ExpandChilds
 * purpose     : Expand all the childs that could be expanded or aligned
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Window::ExpandChilds()
{	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *> *widgets;
	list<SQI_BWidget *>::const_iterator j;
	SQI_BWidget *widget;
	SQI_Widget *w;
	float left,right,top,bottom,xcenter,ycenter,width,height;
	float l,r,c,m;
	BRect f;
	bool expandx,expandy;
	
	SQI_BWidget *left_close,*right_close,*top_close,*bottom_close;
	
	Childs = MagicPtr->Contents;
	
	// we first compute the area convert by all the view in the Window

	double area = RealBounds.Width() * RealBounds.Height();
	
	//cout << "expanding childs of window " << RealBounds.Width() << " " << RealBounds.Height() << "\n";
	
	//cout << area << " " << AreaChilds(MagicPtr->Contents) << "\n";
			
	// in the first loop, we make the expand the views
		
	for(i=Childs->begin();i!=Childs->end();i++)
	{
	
		widgets = (*i)->elements;
		for(j=widgets->begin();j!=widgets->end();j++)			
		{	
			widget = *j;
														
			// we check if we have  some free space
						
			if(area > AreaChilds(MagicPtr->Contents))
			{	
				w = widget->GiveWidget();
				f = widget->VTrueFrame();
					
				left = 0;
				right = 0;
				top = 0;
				bottom = 0;
				width = 0;
				height = 0;
				expandx = false;
				expandy = false;
				
				// we need to expand in one direction first and in the other after,
				// but in which order ?
				
				switch((*i)->alignment)
				{
					case ALIGN_TOP:
					case ALIGN_BOTTOM:
					{
						// we expand y first
						if(w->expandy)
						{
							expandy = true;	
							top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
							bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);	
										
							//cout << top_close << " " << top << "\n";			
																						
							if(!top_close)
							{	
								// we need to expand to the maximum possible that mean to the
								// window size
								
								top = f.top; 
							}
							
							if(!bottom_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								bottom = RealBounds.bottom - f.bottom;
							}
														
							widget->VExpand(0,top,0,bottom);					
						}
						
						// an x after
						
						if(w->expandx)
						{
							expandx = true;
							
							left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
							right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																			
							if(!left_close)
							{	
								// we need to expand to the maximum possible that mean to the
								// window size
								
								left = f.left; 
							}
							
							if(!right_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								right = RealBounds.right - f.right;
							}
														
							widget->VExpand(left,0,right,0);	
						}
						
						break;	
					}
					default:
					{
						// we expand x first
						
						if(w->expandx)
						{
							expandx = true;
							
							left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
							right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																	
							if(!left_close)
							{	
								// we need to expand to the maximum possible that mean to the
								// window size
								
								left = f.left; 
							}
							
							if(!right_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								right = RealBounds.right - f.right;
							}
							
							widget->VExpand(left,0,right,0);	
						}
						
						// and y now
						
						if(w->expandy)
						{
							expandy = true;	
							top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
							bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);	
											
							if(!top_close)
							{	
								// we need to expand to the maximum possible that mean to the
								// window size
								
								top = f.top; 
							}
							
							if(!bottom_close)
							{
								// we need to expand to the maximum possible that mean to the
								// window size
								
								bottom = RealBounds.bottom - f.bottom;
							}
							
							widget->VExpand(0,top,0,bottom);					
						}	
						
						break;
					}	
				}		
			}	
			else
				return;
		}	
	}

	//cout << area << " " << AreaChilds(MagicPtr->Contents) << "\n";

	if(area<=AreaChilds(MagicPtr->Contents))
	{
		//cout << "la\n";
		for(i=Childs->begin();i!=Childs->end();i++)
		{
		
			widgets = (*i)->elements;
			for(j=widgets->begin();j!=widgets->end();j++)			
			{	
				widget = *j;
				widget->Virtual2Real();		
			}
		}	
		//cout << "hmm\n";
		
		return;
	}	
		
	// now we going to process the alignement
	
	for(i=Childs->begin();i!=Childs->end();i++)
	{
	
		widgets = (*i)->elements;
		for(j=widgets->begin();j!=widgets->end();j++)			
		{	
			widget = *j;
													
			w = widget->GiveWidget();
			f = widget->VTrueFrame();
			
			width = 0;
			height = 0;
		
			//if(!expandx)
			//{					
				switch(w->halign)
				{
					case ALIGN_CENTER:
					{
						left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
						right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
																
						if(!left_close)
							left = f.left; 
						
						if(!right_close)
							right = RealBounds.right - f.right;	
									
						if(left == right)
							width = 0;	
						else
						{	
							m = (f.left + f.right) / 2.0;
							l = f.left - left;
							r = f.right + right;	
							c = (l+r) / 2.0;
							
							width = c - m;	
						}	
							
						break;
					}
					case ALIGN_RIGHT:
					{
						left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
						right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
				
						if(!left_close)
							left = f.left; 
						
						if(!right_close)
							right = RealBounds.right - f.right;	
																		
						if(left == right)
							width = 0;	
						else
							width = right;	
						
						break;	
					}	
				}
				
				if(width>0)
					widget->VMoveBy(width,0);						
			//}
			
			//if(!expandy)
			//{
				switch(w->valign)
				{
					case ALIGN_CENTER:
					{
						top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
						bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
								
						if(!top_close)
							top = f.top; 
						
						if(!bottom_close)
							bottom = RealBounds.bottom - f.bottom;
														
						if(top == bottom)
							height = 0;
						else		
						{							
							m = (f.top + f.bottom) / 2.0;
							l = f.top - top;
							r = f.bottom + bottom;	
							c = (l+r) / 2.0;
							
							height = c - m;	
						}	
							
						break;	
					}	
					case ALIGN_BOTTOM:
					{
						top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
						bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
				
						if(!top_close)
							top = f.top; 
						
						if(!bottom_close)
							bottom = RealBounds.bottom - f.bottom;	
						
						if(top == bottom)	
							height = 0;
						else
							height = bottom;
						break;	
					}
				}
				
				if(height>0)
					widget->VMoveBy(0,height);	
			//}
			
			widget->Virtual2Real();	
			
		}
	}
}

/*
 * function    : ReGluing
 * purpose     : Redo the dynamic gluing of the window with all the gluing sequence
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Window::ReGluing()
{	
	resized_by = RESIZED_BY_GLUING;	

	if(!CountChildren())
		return;	
	
	//cout << "REGLUING\n";		
		
	DisableUpdates();
				
	RealBounds = empty;	
	
	// we have to change the min size of the window
		
	//if(constraint==AUTO)	
	SetSizeLimits(0,maxw,0,maxh);
	
	// now we seek through all the view to pack and we pack them	
		
	list<SQI_BWidget *>::const_iterator i;	
	list<SQI_BWidget *> *widgets;
	SQI_BWidget *widget;
	BRect f;
	char align;
	float horizontal,vertical,x,y;
	
	list<TGlue *> *Childs = MagicPtr->Contents;
	list<TGlue *>::const_iterator j;
	
	for(j=Childs->begin();j!=Childs->end();j++)
	{
		widgets = (*j)->elements;	
		align = (*j)->alignment;
	
		for(i=widgets->begin();i!=widgets->end();i++)
		{	
			// first thing, we check if we have already glued something	
								
			if(RealBounds == empty)
			{
				// first gluing so it's rather simple
					
				widget = *i;
				
				widget->VResizeToPreferred();
															
				// and we place the view to it position	
			
				widget->VMoveTo(0,0);
				
				// we get the frame size of the widget with padding
				
				f = widget->VTrueFrame();
				
				RealBounds = f;		
			}
			else
			{	
				// something has been allready glued so it's more compicated
				
				switch(align)
				{
					case ALIGN_TOP:
					{
						// GLUE on top	
																
						widget = *i;
						
						widget->VResizeToPreferred();
							
						// we get the bounds of the view to pack	
									
						f = widget->VTrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height() + 1;
																						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
											
						// we move all the view down
						
						MoveByChilds(MagicPtr->Contents,0,f.Height()+1);
						
						// now we add the view to the top	
						
						widget->VMoveTo(0,0);
						
						break;	
					}
					case ALIGN_BOTTOM:
					{
						// glue on bottom
						
						widget = *i;
						
						widget->VResizeToPreferred();
						
						// we get the bounds of the view to pack	
									
						f = widget->VTrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						horizontal = f.Width() - RealBounds.Width();
						if(horizontal<0)
							horizontal = 0;
						vertical = f.Height()+1;				
					
						y = RealBounds.bottom + 1;	
												
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
							
						// we move the view to the bottom
																
						widget->VMoveTo(0,y);
					
						break;	
					}
					case ALIGN_LEFT:
					{
						// glue on left
						
						widget = *i;
						
						widget->VResizeToPreferred();	
						
						// we get the bounds of the view to pack	
									
						f = widget->VTrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width() + 1;					
						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																				
						// we move all the view right
						
						MoveByChilds(MagicPtr->Contents,f.Width()+1,0);
						
						// now we add the view to the top/left	
						
						widget->VMoveTo(0,0);	
						
						break;	
					}
					case ALIGN_RIGHT:
					{
						// glue on right
												
						widget = *i;
						
						widget->VResizeToPreferred();
											
						// we get the bounds of the view to pack	
									
						f = widget->VTrueBounds();	
								
						// we going to increase the RealBounds of what is need
						
						vertical = fabs(f.Height() - RealBounds.Height());
						if(vertical>0)
							vertical = 0;
						horizontal = f.Width() + 1;
						
						x = RealBounds.right + 1;
										
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																
						// now we add the view to the top/left	
												
						widget->VMoveTo(x,0);	
												
						break;	
					}				
				}				
		
			}			
		}	
		
	}	
		
	// we resize the window
					
	resized_count++;
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();

	// we set the min size of the window 
		
	if(constraint==AUTO)
	{					
		minw = Frame().Width();
		minh = Frame().Height();	
		SetSizeLimits(minw,maxw,minh,maxh);
	}		
	else
	if(constraint==MANUAL)
		SetSizeLimits(minw,maxw,minh,maxh);
		
	// Time to see what could be expanded
	
	ExpandChilds();
			
	EnableUpdates();
	
	resized_by = RESIZED_BY_USER;	
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned window the same way than the original 
 * input       : 
 *
 * SQI_Window *clone, cloned window
 *
 * output      : none
 * side effect : none
 */
void SQI_Window::Clone(SQI_Window *clone)
{
	// some variable first
	clone->RealBounds = RealBounds;
	clone->restore = restore;
	clone->zoom_max_h = zoom_max_h;
	clone->zoom_max_w = zoom_max_w;	
	
	// now the hooks
	
	list<SQI_Object *> *extra;
	list<SQI_Object *>::const_iterator j;	
	
	
	for(int i=0;i<HOOKS_WINDOW;i++)
	{
		clone->Hooks[i].action = Hooks[i].action;
		clone->Hooks[i].extra = NULL;
		if(Hooks[i].extra)
		{
			extra = new list<SQI_Object *>;
			for(j=Hooks[i].extra->begin();j!=Hooks[i].extra->end();j++)
			{
				(*j)->AddRef();
				extra->push_back(*j);	
			}
			clone->Hooks[i].extra = extra;			
		}				
	}
}
