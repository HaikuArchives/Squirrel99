/*
 * Squirrel project
 *
 * file ?	SQI-utils.cpp
 * what	?	various usefull function
 * who	?	jlv
 * when	?	09/06/99
 * last	?	12/09/99
 *
 *
 * (c) electron Technology 1999
 */

#include "SQI-AddOn.h"

#include <ctype.h>
#include <stdio.h>

#include <View.h>
 
 
// List to position conversion 
 
BRect Position2Frame(SQI_List *position)
{
	if(position->Length()!=2)
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Position","position list must have a size of 2"));	
		
	list<SQI_Object *>::const_iterator *j = position->Iterator();
	
	SQI_Number *left = IsNumber(*(*j));
	(*j)++;
	SQI_Number *top = IsNumber(*(*j));
			
	delete j;	
	
	if(left && top)	
	{
		float l = Number2Float(left);
		float t = Number2Float(top); 	
		
		return BRect(l,t,l+100,t+100);
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Position","position element must be number"));		
} 
 
// Frame 2 List conversion

/*
 * function    : List2Frame
 * purpose     : Convert a list in a frame
 * input       : 
 *
 * SQI_List *frame, the list to convert
 *
 * output      : BRect, the frame
 * side effect : none
 */
BRect List2Frame(SQI_List *frame)
{
	if(frame->Length()!=4)
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Frame","frame list must have a size of 4"));	

		
	list<SQI_Object *>::const_iterator *j = frame->Iterator();
	
	SQI_Number *left = IsNumber(*(*j));
	(*j)++;
	SQI_Number *top = IsNumber(*(*j));
	(*j)++;	
	SQI_Number *right = IsNumber(*(*j));
	(*j)++;
	SQI_Number *bottom = IsNumber(*(*j));
			
	delete j;	
	
	if(left && top && right && bottom)	
		return BRect(Number2Float(left),Number2Float(top),Number2Float(right),Number2Float(bottom));
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Frame","frame element must be number"));		
}

/*
 * function    : Frame2List
 * purpose     : Convert a frame in a list
 * input       : 
 *
 * SQI_Squirrel *squirrel, the squirrel
 * BRect frame, the frame to convert
 *
 * output      : BRect, the frame
 * side effect : none
 */
SQI_List *Frame2List(SQI_Squirrel *squirrel,BRect frame)
{
	SQI_List *f = new SQI_List(squirrel->LocalHeap);	
	SQI_Number *val;
	
	val = new SQI_Number(squirrel->LocalHeap,frame.left);
	f->Add2End(val);
	val = new SQI_Number(squirrel->LocalHeap,frame.top);
	f->Add2End(val);
	val = new SQI_Number(squirrel->LocalHeap,frame.right);
	f->Add2End(val);
	val = new SQI_Number(squirrel->LocalHeap,frame.bottom);
	f->Add2End(val);			
	
	return f;	
}

/*
 * function    : Frame2List
 * purpose     : Convert a frame in a list
 * input       : 
 *
 * SQI_Heap *heap, the heap
 * BRect frame, the frame to convert
 *
 * output      : BRect, the frame
 * side effect : none
 */
SQI_List *Frame2List(SQI_Heap *heap,BRect frame)
{
	SQI_List *f = new SQI_List(heap);	
	SQI_Number *val;
	
	val = new SQI_Number(heap,frame.left);
	f->Add2End(val);
	val = new SQI_Number(heap,frame.top);
	f->Add2End(val);
	val = new SQI_Number(heap,frame.right);
	f->Add2End(val);
	val = new SQI_Number(heap,frame.bottom);
	f->Add2End(val);			
	
	return f;	
}

/*
 * function    : GetColorFromList
 * purpose     : Convert a list in a color
 * input       : 
 *
 * SQI_List *frame, the list to convert
 * rgb_color *color, the color find
 *
 * output      : bool, true if ok , false else
 * side effect : none
 */
bool GetColorFromList(SQI_List *clist,rgb_color *color)
{
	int size = clist->Length();
	SQI_Number *val;	
	
	if(size==3)
	{
		list<SQI_Object*>::const_iterator i = clist->Data()->begin();
		
		val = IsInteger(*i);
		if(val)
		{
			i++;
			color->red = Number2Int(val);
			val = IsInteger(*i);
			if(val)
			{
				i++;
				color->green = Number2Int(val);
				val = IsInteger(*i);
				if(val)
				{
					color->blue = Number2Int(val);
					color->alpha = 255;
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}		
		else
			return false;
	}
	else
	if(size==4)
	{
		list<SQI_Object*>::const_iterator i = clist->Data()->begin();
		
		val = IsInteger(*i);
		if(val)
		{
			i++;
			color->red = Number2Int(val);
			val = IsInteger(*i);
			if(val)
			{
				i++;
				color->green = Number2Int(val);
				val = IsInteger(*i);
				if(val)
				{
					i++;
					color->blue = Number2Int(val);
					val = IsInteger(*i);
					if(val)
					{
						color->alpha = Number2Int(val);
						return true;	
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}		
		else
			return false;	
	}
	else
		return false;	
}

/*
 * function    : GetListFromColor
 * purpose     : Convert a color in a list
 * input       : 
 *
 * SQI_Squirrel *squirrel in use
 * SQI_List *clist, the list to store color in
 * rgb_color *color, the color
 *
 * output      : bool, true if ok , false else
 * side effect : none
 */
bool GetListFromColor(SQI_Squirrel *squirrel,SQI_List *clist,rgb_color *color)
{
	clist->Add2End(new SQI_Number(squirrel->LocalHeap,color->red));
	clist->Add2End(new SQI_Number(squirrel->LocalHeap,color->green));
	clist->Add2End(new SQI_Number(squirrel->LocalHeap,color->blue));
	clist->Add2End(new SQI_Number(squirrel->LocalHeap,color->alpha));
	return true;
}

/*
 * function    : GetListFromColor
 * purpose     : Convert a color in a list
 * input       : 
 *
 * SQI_Heap *heap, heap to use
 * SQI_List *clist, the list to store color in
 * rgb_color *color, the color
 *
 * output      : bool, true if ok , false else
 * side effect : none
 */
bool GetListFromColor(SQI_Heap *heap,SQI_List *clist,rgb_color *color)
{
	clist->Add2End(new SQI_Number(heap,color->red));
	clist->Add2End(new SQI_Number(heap,color->green));
	clist->Add2End(new SQI_Number(heap,color->blue));
	clist->Add2End(new SQI_Number(heap,color->alpha));
	return true;
}

/*
 * function    : List2Point
 * purpose     : Convert a list in a BPoint
 * input       : 
 *
 * SQI_List *point, list to use 
 *
 * output      : BPoint, the point
 * side effect : none
 */
BPoint List2Point(SQI_List *point)
{
	if(point->Length()!=2)
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Point","point list must have a size of 2"));	

	list<SQI_Object *>::const_iterator *j = point->Iterator();
	
	SQI_Number *x = IsNumber(*(*j));
	(*j)++;
	SQI_Number *y = IsNumber(*(*j));
	(*j)++;	
			
	delete j;	
	
	if(x && y)	
		return BPoint(Number2Float(x),Number2Float(y));
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"Point","point element must be number"));		
}

/*
 * function    : Point2List
 * purpose     : Convert a BPoint in a list
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel to use
 * BPoint *point, the point
 *
 * output      : SQI_List *, the created list
 * side effect : none
 */
SQI_List *Point2List(SQI_Squirrel *squirrel,BPoint *point)
{
	SQI_List *f = new SQI_List(squirrel->LocalHeap);	
	SQI_Number *val;
	
	val = new SQI_Number(squirrel->LocalHeap,point->x);
	f->Add2End(val);
	val = new SQI_Number(squirrel->LocalHeap,point->y);
	f->Add2End(val);
		
	return f;	
}

/*
 * function    : Point2List
 * purpose     : Convert a BPoint in a list
 * input       : 
 *
 * SQI_Heap *heap, heap to use
 * BPoint *point, the point
 *
 * output      : SQI_List *, the created list
 * side effect : none
 */
SQI_List *Point2List(SQI_Heap *heap,BPoint *point)
{
	SQI_List *f = new SQI_List(heap);	
	SQI_Number *val;
	
	val = new SQI_Number(heap,point->x);
	f->Add2End(val);
	val = new SQI_Number(heap,point->y);
	f->Add2End(val);
	
	return f;	
}

/*
 * function    : AddExtra
 * purpose     : Add the extra input suuplied by the user to the argument of the hook
 * input       : 
 *
 * list<SQI_Object *> *args, the argument
 * list<SQI_Object *> *extra, the extra input to add
 *
 * output      : none
 * side effect : none
 */ 
void AddExtra(list<SQI_Object *> *args,list<SQI_Object *> *extra)
{
	if(extra)
	{
		list<SQI_Object *>::const_iterator i = extra->begin();
		list<SQI_Object *>::const_iterator e = extra->end();
	
		for(i;i!=e;i++)
		{
			args->push_back(*i);
			(*i)->AddRef();
		}	
	}
}

/*
 * function    : List2Size
 * purpose     : Convert a list in size
 * input       : 
 *
 * SQI_List *point, list to use 
 * int *width, where to return the first element
 * int *height, where to return the second element
 *
 * output      : none
 * side effect : none
 */
void List2Size(SQI_List *size,int *width,int *height)
{
	if(size->Length()==2)
	{
		list<SQI_Object *>::const_iterator *j = size->Iterator();
		
		SQI_Number *w = IsInteger(*(*j));
		(*j)++;
		SQI_Number *h = IsInteger(*(*j));
		(*j)++;	
				
		delete j;	
		
		if(w && h)	
		{
			*width = Number2Int(w);
			*height = Number2Int(h);
		}	
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"list size","list element must be number"));		
	}
}

/*
 * function    : List2Pad
 * purpose     : Convert a list in pad
 * input       : 
 *
 * SQI_List *point, list to use 
 * float *padx, where to return the first element
 * float *pady, where to return the second element
 *
 * output      : none
 * side effect : none
 */
void List2Pad(SQI_List *pad,float *padx,float *pady)
{
	if(pad->Length()==2)
	{
		list<SQI_Object *>::const_iterator *j = pad->Iterator();
		
		SQI_Number *w = IsNumber(*(*j));
		(*j)++;
		SQI_Number *h = IsNumber(*(*j));
		(*j)++;	
				
		delete j;	
		
		if(w && h)	
		{
			*padx = Number2Float(w);
			*pady = Number2Float(h);
		}	
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"list pad","list element must be number"));		
	}
}

// frame test

/*
 * function    : OnLeft
 * purpose     : Check if a frame is on the left of another
 * input       : 
 *
 * BRect *origin, frame to use
 * BRect *test, frame to test
 *
 * output      : bool, true if on left
 * side effect : none
 */
bool OnLeft(BRect *origin,BRect *test)
{
	// the view must be on the left
	if(test->right <= origin->left)	
	{
		// now we check that the bottom of the view is after the top or our origin
		if(test->bottom > origin->top)
		{
			// new we check that the top of the view is before the bottom of our origin	
			if(test->top < origin->bottom)
				return true;
			else
				return false;
		}
		else
			return false;	
	}
	else
		return false;
}

/*
 * function    : OnRight
 * purpose     : Check if a frame is on the right of another
 * input       : 
 *
 * BRect *origin, frame to use
 * BRect *test, frame to test
 *
 * output      : bool, true if on right
 * side effect : none
 */
bool OnRight(BRect *origin,BRect *test)
{
	// the view must be on the right
	if(test->left >= origin->right)	
	{
		// now we check that the bottom of the view is after the top or our origin
		if(test->bottom > origin->top)
		{
			// new we check that the top of the view is before the bottom of our origin	
			if(test->top < origin->bottom)
				return true;
			else
				return false;
		}
		else
			return false;	
	}
	else
		return false;	
}

/*
 * function    : OnTop
 * purpose     : Check if a frame is on the top of another
 * input       : 
 *
 * BRect *origin, frame to use
 * BRect *test, frame to test
 *
 * output      : bool, true if on top
 * side effect : none
 */
bool OnTop(BRect *origin,BRect *test)
{
	// the view must be above
	if(test->bottom <= origin->top)
	{
		// now we check that the right of the view is after the left
		if(test->right >= origin->left)
		{
			// now we check that the left of the view is before the right
			if(test->left <= origin->right)
				return true;
			else
				return false;	
		}
		else
			return false;	
	}
	else
		return false;
}

/*
 * function    : OnBottom
 * purpose     : Check if a frame is under another
 * input       : 
 *
 * BRect *origin, frame to use
 * BRect *test, frame to test
 *
 * output      : bool, true if under
 * side effect : none
 */
bool OnBottom(BRect *origin,BRect *test)
{
	// the view must be under
	if(test->top >= origin->bottom)
	{
		// now we check that the right of the view is after the left
		if(test->right >= origin->left)
		{
			// now we check that the left of the view is before the right
			if(test->left <= origin->right)
				return true;
			else
				return false;	
		}
		else
			return false;	
	}
	else
		return false;	
}

// Variable cloning (only the name)


/*
 * function    : VariableClone
 * purpose     : Clone a word by adding a number at the end of the new name
 * input       : 
 *
 * SQI_Keyword *var, word to copy
 *
 * output      : SQI_Keyword *, the new word
 * side effect : none
 */
SQI_Keyword *VariableClone(SQI_Keyword *var)
{
	string name = *(var->Data());
	
	if(isdigit(name[name.length()-1]))
	{
		int pos = name.rfind(".");
		string num = name.substr(pos+1,name.length()-1-pos);
		name.erase(pos+1);
		int n = atoi(num.c_str()) + 1;
		char s[10];
		sprintf(s,"%d",n);
		name.append(s);								
	}
	else
		name.append(".1");			
	
	return new SQI_Keyword(var->heap,&name);	
}

// key modifiers

SQI_Keyword *TestMod(uint32 modifiers,SQI_Heap *heap,uint32 val,char *word)
{
	if(modifiers == (modifiers | val))
		return new SQI_Keyword(heap,word);
	else
		return NULL;			
}

/*
 * function    : GetModifiers
 * purpose     : Get the list of the modifiers used
 * input       : 
 *
 * uint32 modifiers, the modifiers
 * SQI_Heap *heap, the heap to use
 *
 * output      : none
 * side effect : none
 */
SQI_Object *GetModifiers(uint32 modifiers,SQI_Heap *heap)
{
	uint32 test;
	SQI_Keyword *modi;
	SQI_List *lst;
	
	lst = new SQI_List(heap);
	
	modi = TestMod(modifiers,heap,B_LEFT_SHIFT_KEY,"left_shift");
	if(modi)
		lst->Add2End(modi);
		
	modi = TestMod(modifiers,heap,B_RIGHT_SHIFT_KEY,"right_shift");
	if(modi)
		lst->Add2End(modi);		
	
	modi = TestMod(modifiers,heap,B_LEFT_CONTROL_KEY,"left_control");
	if(modi)
		lst->Add2End(modi);	
		
	modi = TestMod(modifiers,heap,B_RIGHT_CONTROL_KEY,"right_control");
	if(modi)
		lst->Add2End(modi);
		
	modi = TestMod(modifiers,heap,B_LEFT_OPTION_KEY,"left_option");
	if(modi)
		lst->Add2End(modi);				
		
	modi = TestMod(modifiers,heap,B_RIGHT_OPTION_KEY,"right_option");
	if(modi)
		lst->Add2End(modi);	

	modi = TestMod(modifiers,heap,B_LEFT_COMMAND_KEY,"left_alt");
	if(modi)
		lst->Add2End(modi);				
		
	modi = TestMod(modifiers,heap,B_RIGHT_COMMAND_KEY,"right_alt");
	if(modi)
		lst->Add2End(modi);			
		
	if(lst->Length()==1)
	{
		SQI_Object *obj;
		
		obj = (*lst)[0];
		
		obj->AddRef();	
		
		lst->RemRef();
	
		obj->REF--;
	
		return obj;		
	}
	else
	if(!lst->Length())
	{
		lst->RemRef();
		return new SQI_Keyword(heap,"none");		
	}
	else
		return lst;				
}
