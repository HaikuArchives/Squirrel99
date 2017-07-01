/*
 * Squirrel project
 *
 * file ?	SQI-glue.cpp
 * what	?   Gluing usefull functions
 * who	?	jlv
 * when	?	11/30/99
 * last	?	11/30/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-glue.h"
#include "SQI-utils.h"

BRect empty(0,0,0,0);
  
/*
 * function    : MoveByChilds
 * purpose     : Move all the childs of the view
 * input       :
 *
 * float h, horizontal value to add
 * float v, vertical value to add 
 *
 * output      : none
 * side effect : none
 */
void MoveByChilds(list<TGlue *> *Childs,float h,float v,char except = -1)
{
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *>::const_iterator j;
	list<SQI_BWidget *> *widgets;
	BView *child;	
			
	for(i=Childs->begin();i!=Childs->end();i++)
	{
		if((*i)->alignment != except)
		{
			widgets = (*i)->elements;
			for(j=widgets->begin();j!=widgets->end();j++)
			{
				child = (*j)->GiveBView();
				child->MoveBy(h,v);	
			}	
		}	
	}		
}

/*
 * function    : MoveByChilds
 * purpose     : Move all the childs of the view which are glued top, bottom, right or left
 * input       :
 *
 * char align, alignment to move only
 * float h, horizontal value to add
 * float v, vertical value to add 
 *
 * output      : none
 * side effect : none
 */
void MoveByChilds(list<TGlue *> *Childs,char align,float h,float v,char except = -1)
{
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *>::const_iterator j;
	list<SQI_BWidget *> *widgets;
	BView *child;	
			
	for(i=Childs->begin();i!=Childs->end();i++)
	{
		if((*i)->alignment == align && align != except)
		{
			widgets = (*i)->elements;
			for(j=widgets->begin();j!=widgets->end();j++)
			{
				child = (*j)->GiveBView();
				child->MoveBy(h,v);	
			}	
		}	
	}	
}

/*
 * function    : AreaChilds
 * purpose     : Compute the area covered by the childs of the window
 * input       : none
 * output      : double
 * side effect : none
 */
double AreaChilds(list<TGlue *> *Childs)
{
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *>::const_iterator j;
	list<SQI_BWidget *> *widgets;
		
	double area = 0;
	
	for(i=Childs->begin();i!=Childs->end();i++)
	{
		widgets = (*i)->elements;
		for(j=widgets->begin();j!=widgets->end();j++)
			area += (*j)->Area();	
	}	
	
	return area;
}

/*
 * function    : FindClosest
 * purpose     : Return the SQI_BWidget which is the closest view
 * input       :
 *
 * SQI_BWidget *origin, view to search for closest of
 * char side, where to search
 * float *dist, set to the distance between the closest
 *
 *
 * output      : SQI_BWidget *, the closes view, or NULL if none
 * side effect : none
 */
SQI_BWidget *FindClosest(list<TGlue *> *Childs,SQI_BWidget *origin,char side,float *dist)
{
	SQI_BWidget *closest = NULL;	
	float xo,yo,distance,d;
	BRect of,wf;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *> *widgets;
	list<SQI_BWidget *>::const_iterator j;
	SQI_BWidget *widget;
	SQI_Widget *w;				

	// we get the frame of the origin
	
	of = origin->TrueFrame();
					
	switch(side)
	{
		case LEFT:
		{		
			xo = of.left;		
		
			for(i=Childs->begin();i!=Childs->end();i++)
			{
	
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
				{	
					widget = *j;
					if(widget != origin)
					{
						// we need to know if the widget is on the left of the origin					
						
						wf = widget->TrueFrame();
																	
						if(OnLeft(&of,&wf))
						{														
							d = xo - wf.right; // + 1;				
																						
							if(!closest)
							{
								closest = widget;
								distance = d;	
							}
							else
							{
								if(distance > d)
								{
									distance = d;
									closest = widget;	
								}	
							}									
						}	
					}	
				}
			}

			break;
		}
		case RIGHT:
		{
			xo = of.right;		
		
			for(i=Childs->begin();i!=Childs->end();i++)
			{
	
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
				{	
					widget = *j;
					if(widget != origin)
					{
						// we need to know if the widget is on the right of the origin					
						
						wf = widget->TrueFrame();	
												
						if(OnRight(&of,&wf))
						{
							d = wf.left - xo; // + 1;				
						
							if(!closest)
							{
								closest = widget;
								distance = d;	
							}
							else
							{
								if(distance > d)
								{
									distance = d;
									closest = widget;	
								}	
							}									
						}	
					}	
				}
			}
			
			break;
		}
		case TOP:
		{
			yo = of.top;		
				
			for(i=Childs->begin();i!=Childs->end();i++)
			{
	
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
				{	
					widget = *j;
					if(widget != origin)
					{
						// we need to know if the widget is on the right of the origin					
						
						wf = widget->TrueFrame();
																							
						if(OnTop(&of,&wf))
						{
							d = yo - wf.bottom; // + 1;				
						
							if(!closest)
							{
								closest = widget;
								distance = d;	
							}
							else
							{
								if(distance > d)
								{
									distance = d;
									closest = widget;	
								}	
							}									
						}	
					}	
				}
			}	
					
			break;
		}
		case BOTTOM:		
		{
			yo = of.bottom;	
								
			for(i=Childs->begin();i!=Childs->end();i++)
			{
	
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
				{	
					widget = *j;
					if(widget != origin)
					{
						// we need to know if the widget is on the right of the origin					
						
						wf = widget->TrueFrame();
																							
						if(OnBottom(&of,&wf))
						{
							d = wf.top - yo; // + 1;
																		
							if(!closest)
							{
								closest = widget;
								distance = d;	
							}
							else
							{
								if(distance > d)
								{
									distance = d;
									closest = widget;	
								}	
							}									
						}	
					}	
				}
			}				
			break;
		}
	}		
	
	*dist = distance-1;
	
	return closest;	
}



