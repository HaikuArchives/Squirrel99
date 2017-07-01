/*
 * Squirrel project
 *
 * file ?	SQI-window.h
 * what	?   Window object
 * who	?	jlv
 * when	?	11/23/99
 * last	?	03/07/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Window.h>
#include "Hook.h"

/* hooks */

#define HOOK_WIN_ENTER				0
#define HOOK_WIN_LEAVE				1
#define HOOK_WIN_MAXIMIZE			2
#define HOOK_WIN_MESSAGERECEIVED 	3
#define HOOK_WIN_MINIMIZE 			4
#define HOOK_WIN_MOVED 			    5
#define HOOK_WIN_QUITREQUESTED		6
#define HOOK_WIN_RESIZED 		    7
#define HOOK_WIN_SCREENCHANGED		8
#define HOOK_WIN_WORKSPACEACTIVATED 9
#define HOOK_WIN_WORKSPACECHANGED	10
#define HOOK_WIN_ZOOM				11

#define HOOKS_WINDOW 12

// some flags

#define RESIZED_BY_GLUING	0
#define RESIZED_BY_USER		1

enum T_Win_Constraint {
	AUTO,
	MANUAL,
	NONE	
};

class SQI_BWindow; // foward def

class SQI_Window : public BWindow
{
	public:
		SQI_Window(SQI_BWindow *ptr,BRect frame,const char *title,window_look look,window_feel feel,uint32 flags);
		~SQI_Window();	
		void Quit(void);
		bool QuitRequested();
		void WindowActivated(bool active);
		void Minimize(bool minimize);
		void FrameMoved(BPoint screenPoint);
		void FrameResized(float width,float height);
		void WorkspaceActivated(int32 workspace,bool active);
		void WorkspacesChanged(uint32 oldWorkspaces,uint32 newWorkspaces);
		void Zoom(BPoint leftTop,float width,float height);
		void MessageReceived(BMessage *message);
		// Methods
		bool Hook(SQI_Keyword *event,SQI_Object *action,list<SQI_Object *> *extra);
		bool UnHook(SQI_Keyword *event);
		SQI_Object *UseHook(SQI_Node *node,list<SQI_Object *> *args,bool spawn = false);
		// Gluing methods
		void ReGluing();
		void DynamicGluing(char align,list<SQI_BWidget *> *widgets);
		void StaticGluing();
		void ExpandChilds();
		// Cloning methods
		void Clone(SQI_Window *clone);
		// variable
		SQI_BWindow *MagicPtr;		// Pointer to the SQI_BWindow
		float zoom_max_h;
		float zoom_max_w;				// workaround the minimize bug
		T_Win_Constraint constraint;	// say if the window has it size constraint
		float maxh,minh,maxw,minw;
	private: 	
		int resized_count;			// count the number of time the window is resized dynamically
		BRect RealBounds;
		char resized_by;
		THook Hooks[HOOKS_WINDOW]; 	// Table of hooks
		bool restore;
};

// functions

void InitBWindow();
