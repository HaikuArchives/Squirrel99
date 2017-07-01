/*
 * Squirrel project
 *
 * file ?	SQI-utils.h
 * what	?	various usefull function
 * who	?	jlv
 * when	?	09/06/99
 * last	?	02/03/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <Rect.h> 
#include <Point.h>
#include <View.h>

void AddExtra(list<SQI_Object *> *args,list<SQI_Object *> *extra);
 
// List 2 Size conversion
void List2Size(SQI_List *size,int *width,int *height); 

// List 2 Pad conversion
void List2Pad(SQI_List *pad,float *padx,float *pady); 
 
// List 2 Position conversion
BRect Position2Frame(SQI_List *position);
 
 
// Frame 2 List conversion
BRect List2Frame(SQI_List *frame);
SQI_List *Frame2List(SQI_Squirrel *squirrel,BRect frame);
SQI_List *Frame2List(SQI_Heap *heap,BRect frame);

// Color 2 List conversion
bool GetColorFromList(SQI_List *clist,rgb_color *color);
bool GetListFromColor(SQI_Squirrel *squirrel,SQI_List *clist,rgb_color *color);
bool GetListFromColor(SQI_Heap *heap,SQI_List *clist,rgb_color *color);

// Point 2 List conversion
BPoint List2Point(SQI_List *point);
SQI_List *Point2List(SQI_Squirrel *squirrel,BPoint *point);
SQI_List *Point2List(SQI_Heap *heap,BPoint *point);

// Frame test

bool OnLeft(BRect *origin,BRect *test);
bool OnRight(BRect *origin,BRect *test);
bool OnTop(BRect *origin,BRect *test);
bool OnBottom(BRect *origin,BRect *test);

// Variable cloning (only the name)

SQI_Keyword *VariableClone(SQI_Keyword *var);

// Get Modifiers value
SQI_Object *GetModifiers(uint32 modifiers,SQI_Heap *heap);

