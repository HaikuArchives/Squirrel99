/*
 * Squirrel project
 *
 * file ?	SQI-frame.cpp
 * what	?   frame object
 * who	?	jlv
 * when	?	11/30/99
 * last	?	03/04/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "Colors.h"
#include "SQI-AddOn.h"
#include "SQI-utils.h"
#include "SQI-widget.h"
#include "SQI-bwidget.h"
#include "SQI-bbox.h"
#include "SQI-glue.h"
#include "SQI-box.h"
#include "SQI-bwindow.h"
#include "SQI-font.h"
     
/*
 * function    : SQI_Box
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
SQI_Box::SQI_Box(SQI_Interp *interp,SQI_BBox *ptr,const char *label,BRect frame,uint32 resizingMode,uint32 flags,border_style b)
	:BBox(frame,NULL,resizingMode,flags | B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP ,b) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SetViewColor(BeButtonGrey); // default color
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();
			
	border = 4;	
	SetPenSize(border - 3);
	
	SetLabel(label); 
}

/*
 * function    : SQI_Box
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
SQI_Box::SQI_Box(SQI_Interp *interp,SQI_BBox *ptr,BView *label,BRect frame,uint32 resizingMode,uint32 flags,border_style b)
	:BBox(frame,NULL,resizingMode,flags | B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP , b) , 
	SQI_Widget(ptr)
{	
	Interpreter = interp;
	MagicPtr = ptr;
	SetViewColor(BeButtonGrey); // default color
	
	original_width = Bounds().Width();
	original_height = Bounds().Height();
	
	RealBounds = Bounds();
	
	border = 2;
	SetPenSize(border);	
	
	SetLabel(label); 
}


/*
 * function    : ~SQI_Box
 * purpose     : Box destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Box::~SQI_Box()
{
	MagicPtr->View = NULL;
} 

// Hooks

/*
 * function    : AttachedToWindow
 * purpose     : Called when the view is attached to a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::AttachedToWindow(void)
{
	BBox::AttachedToWindow();
	SQI_Widget::AttachedToWindow(this);			
				
	// we now calculate the place to reserve for the label
	
	if(MagicPtr->Label)
		MagicPtr->Label->ResizeToPreferred();	

	ReservedForLabel();	
		
	// we need to calculate the size of the box without widgets
	
	if(MagicPtr->Label)
	{
		RealBounds.right = max(RealBounds.right,MagicPtr->Label->TrueBounds().Width() + 20);
		RealBounds.bottom = max(RealBounds.bottom,MagicPtr->Label->TrueBounds().Height() + 5);	
	}	
	else
	{
	 	RealBounds.right = max(RealBounds.right,StringWidth(Label()) + 20);
	 	RealBounds.bottom = max(RealBounds.bottom,btop + 5);	
	}
									
}

/*
 * function    : AllAttached
 * purpose     : Called when the all the descendant of the view have been called with AttachedToWindow
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::AllAttached(void)
{		
	// All the child widget of this view have been attached
	
	// it may be time to compute the size of the view
	
	// for that we call  DynamicGluing	
	
	DynamicGluing();
	
	BWidget->vframe = Frame();
	
	BBox::AllAttached();	
}

/*
 * function    : DetachedToWindow
 * purpose     : Called when the view is detached from a window
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::DetachedFromWindow(void)
{				
	SQI_Widget::DetachedFromWindow(this);			
	BBox::DetachedFromWindow();	
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
void SQI_Box::Draw(BRect updateRect)
{
	SQI_Widget::Draw(this,updateRect);	
	BBox::Draw(updateRect);			
}

/*
 * function    : BoxMoved
 * purpose     : Called when the view position have changed
 * input       :
 *
 * BPoint parentPoint
 *
 * output      : none
 * side effect : none
 */
void SQI_Box::FrameMoved(BPoint parentPoint)
{
	SQI_Widget::FrameMoved(this,parentPoint);
	BBox::FrameMoved(parentPoint);	
}

/*
 * function    : BoxResized
 * purpose     : Called when the view size has changed
 * input       :
 *
 * float width, new width
 * float height, new height
 *
 * output      : none
 * side effect : none
 */
void SQI_Box::FrameResized(float width,float height)
{
	/*
	if(!resized_count)
	{
		*/
		SQI_Widget::FrameResized(this,width,height);		
		BBox::FrameResized(width,height);	
		if(Bounds() != RealBounds)	
			StaticGluing();
		
/*	}
	else
	{
		resized_count--;
		if(resized_count<0)
			resized_count = 0;
		BBox::FrameResized(width,height);	
	}
	*/				
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
void SQI_Box::KeyDown(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyDown(this,bytes,numBytes))
		BBox::KeyDown(bytes,numBytes);
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
void SQI_Box::KeyUp(const char *bytes,int32 numBytes)
{
	if(!SQI_Widget::KeyUp(this,bytes,numBytes))
		BBox::KeyUp(bytes,numBytes);
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
void SQI_Box::MakeFocus(bool focused = true)
{
	SQI_Widget::MakeFocus(this,focused);	
	BBox::MakeFocus(focused);		
}

/*
 * function    : MessageReceived
 * purpose     : A message has been received by the widget
 * input       : 
 *
 * BMessage *message, the message received
 *
 * output      : none
 * side effect : none
 */
void SQI_Box::MessageReceived(BMessage *message)
{			
	if(!SQI_Widget::MessageReceived(this,message))
		BBox::MessageReceived(message);	
					
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
void SQI_Box::MouseDown(BPoint point)
{
	if(!SQI_Widget::MouseDown(this,point))
		BBox::MouseDown(point);		
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
void SQI_Box::MouseMoved(BPoint point,uint32 transit,const BMessage *message)
{
	SQI_Widget::MouseMoved(this,point,transit,message);		
	BBox::MouseMoved(point,transit,message);
}

/*
 * function    : Pulse
 * purpose     : Executed at a certain interval
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::Pulse(void)
{
	SQI_Widget::Pulse(this);		
	BBox::Pulse();
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
void SQI_Box::WindowActivated(bool active)
{
	SQI_Widget::WindowActivated(this,active);	
	BBox::WindowActivated(active);
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
void SQI_Box::GetPreferredSize(float *width,float *height)
{
		
	// for now we return the original size, but when we will
	// have view attached to it, it will be diferrent	
		
	*width = original_width;
	*height = original_height;
}

/*
 * function    : DynamicGluing
 * purpose     : Glue a new set of views and resize the window according
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::DynamicGluing()
{	
	resized_by = RESIZED_BY_GLUING;	

	if(!MagicPtr->Contents)
		return;	
				
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
															
				// and we place the view to it position	
			
				widget->VMoveTo(border,btop);
				
				// we get the frame size of the widget with padding
				
				f = widget->VTrueFrame();
				
				f.InsetBy(-border,0);
				f.bottom += btop + border;
				
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
						
						if((f.Width()+2*border) > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
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
							
							widget->VMoveTo(border,btop);
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
						
						if((f.Width()+2*border) > RealBounds.Width())
							horizontal = f.Width() - RealBounds.Width() + 2*border;
						else
							horizontal = 0;
						if(f.Height()>0)						
							vertical = f.Height() + 1;
						else
							vertical = 0;	
						
						if(vertical>0 || horizontal>0)
						{ 			
							y = RealBounds.bottom - btop - border + 1;	
													
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
								
							// we move the view to the bottom
																	
							widget->VMoveTo(border,y);
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
						
						if((f.Height() + btop) > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + border + btop;
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
							
							widget->VMoveTo(border,btop);
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
						
						if((f.Height()+btop) > RealBounds.Height())
							vertical = f.Height() - RealBounds.Height() + border + btop;
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
							x = RealBounds.right - border + 1;
											
							RealBounds.right += horizontal;
							RealBounds.bottom += vertical;
																	
							// now we add the view to the top/left	
													
							widget->VMoveTo(x,btop);
						}	
												
						break;	
					}				
				}				
		
			}			
		}	
		
	}	

	// due to the fact that we could have a view as label we may increase the size
	// of the widget if it's too small
	
	if(MagicPtr->Label)
	{	
		int w = 10 + LabelView()->Bounds().Width();
		int h = 2 + LabelView()->Bounds().Height();	
			
		if(RealBounds.Width()<w)
			RealBounds.right += w - RealBounds.Width();	
		
		if(RealBounds.Height()<h)
			RealBounds.bottom += h - RealBounds.Height();	
	}	
	
	// we resize the view
					
	resized_count++;
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();
	
	original_width = RealBounds.Width();
	original_height = RealBounds.Height();
	
	// Time to see what could be expanded
	
	ExpandChilds();
			
	resized_by = RESIZED_BY_USER;	
}

/*
 * function    : StaticGluing
 * purpose     : The frame of the view have been changed by the user, we re glue everything
 * input       : 
 *
 * float width, new width of the window
 * float height, new height of the window
 * 
 * output      : none
 * side effect : none
 */
void SQI_Box::StaticGluing()
{
	// if no children, don't need to do anything
	
	if(!CountChildren())
		return;	
			
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
					
					widget->VResizeToPreferred();	
						
					// we get the bounds of the view to pack	
								
					f = widget->VTrueBounds();	
																	
					// we move all the view aligner TOP, down
						
					MoveByChilds(MagicPtr->Contents,0.0,f.Height()+1,(char)ALIGN_BOTTOM);
										
					// now we add the view to the top	
					
					widget->VMoveTo(border,btop);		
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
										
					y = RealBounds.bottom -f.Height() - border;
															
					widget->VMoveTo(border,y);
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
						
					x = RealBounds.right - f.Width() - border;
										
					widget->VMoveTo(x,btop);	
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
					
					widget->VMoveTo(border,btop);					
				}
				
				break;				
			}	
		}
	}
	
	// Time to see what could be expanded
	
	ExpandChilds();
}

/*
 * function    : Adapt
 * purpose     : Change RealBounds to the actual bounds
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::Adapt()
{
	// all childs must be resized to preferred
	
	double a1,a2;	
		
	a1 = RealBounds.Width() * RealBounds.Height();
	a2 = Bounds().Width() * Bounds().Height();	
	
	RealBounds = Bounds();

	if(a2<a1)
	{	
		list<TGlue *> *Childs;
		list<SQI_BWidget *> *widgets;
		list<TGlue *>::const_iterator i;
		list<SQI_BWidget *>::const_iterator j;	
		
		Childs = MagicPtr->Contents;
		
		if(Childs)
		{	
			for(i=Childs->begin();i!=Childs->end();i++)
			{
			
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
					(*j)->VResizeToPreferred();
			}
		}
	}
	else
		StaticGluing();
}

/*
 * function    : ExpandChilds
 * purpose     : Expand all the childs that could be expanded or aligned
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::ExpandChilds()
{	
	list<TGlue *> *Childs;
	list<TGlue *>::const_iterator i;
	list<SQI_BWidget *> *widgets;
	list<SQI_BWidget *>::const_iterator j;
	SQI_BWidget *widget;
	SQI_Widget *w;
	float left,right,top,bottom,xcenter,ycenter,width,height;
	int m;
	BRect f;
	bool expandx,expandy;
	
	SQI_BWidget *left_close,*right_close,*top_close,*bottom_close;
	
	Childs = MagicPtr->Contents;
		
	if(Childs)
	{	
		// we first compute the area convert by all the view in the Window
	
		BRect z = RealBounds;
		
		//z.InsetBy(border,border + btop); 	
	
		double area = (z.Width()-2*border) * (z.Height()-border-btop);
		
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
												
								if(!top_close)
								{	
									// we need to expand to the maximum possible that mean to the
									// window size
									
									top = f.top - border - btop; 
								}
								
								if(!bottom_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									bottom = RealBounds.bottom - border - f.bottom;
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
									
									left = f.left - border; 
								}
								
								if(!right_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									right = RealBounds.right - border - f.right;
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
									
									left = f.left - border; 
								}
								
								if(!right_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									right = RealBounds.right - border - f.right;
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
									
									top = f.top - border - btop; 
								}
								
								if(!bottom_close)
								{
									// we need to expand to the maximum possible that mean to the
									// window size
									
									bottom = RealBounds.bottom - border - f.bottom;
								}
								
								widget->VExpand(0,top,0,bottom);					
							}	
							
							break;
						}	
					}		
				}	
				else
				{
					for(i=Childs->begin();i!=Childs->end();i++)
					{
						widgets = (*i)->elements;
						for(j=widgets->begin();j!=widgets->end();j++)			
						{	
							widget = *j;
							widget->Virtual2Real();		
						}
					}	
					
					return;	
				}
			}	
		}
	
		if(area<=AreaChilds(MagicPtr->Contents))
		{
			for(i=Childs->begin();i!=Childs->end();i++)
			{
				widgets = (*i)->elements;
				for(j=widgets->begin();j!=widgets->end();j++)			
				{	
					widget = *j;
					widget->Virtual2Real();		
				}
			}	
			
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
								left = f.left - border; 
							
							if(!right_close)
								right = RealBounds.right - border - f.right;	
										
							if(left == right)
								width = 0;	
							else
							{	
								m = (left + f.Width() + right) / 2;
								width = (m - left) - (f.Width() / 2);		
							}	
								
							break;
						}
						case ALIGN_RIGHT:
						{
							left_close = FindClosest(MagicPtr->Contents,widget,LEFT,&left);
							right_close = FindClosest(MagicPtr->Contents,widget,RIGHT,&right);
					
							if(!left_close)
								left = f.left - border; 
							
							if(!right_close)
								right = RealBounds.right - border - f.right;	
																			
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
								top = f.top - border - btop; 
							
							if(!bottom_close)
								bottom = (RealBounds.bottom - border) - f.bottom;
															
							if(top == bottom)
								height = 0;
							else		
							{
								m = (top + f.Height() + bottom) / 2;
								height = (m - top) - (f.Height() / 2);	
							}	
								
							break;	
						}	
						case ALIGN_BOTTOM:
						{
							top_close = FindClosest(MagicPtr->Contents,widget,TOP,&top);
							bottom_close = FindClosest(MagicPtr->Contents,widget,BOTTOM,&bottom);
					
							if(!top_close)
								top = f.top - border; 
							
							if(!bottom_close)
								bottom = RealBounds.bottom - border - f.bottom;	
							
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
}

/*
 * function    : ReservedForLabel
 * purpose     : Compute in btop the space reserved for the label
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::ReservedForLabel()
{
	BView *view = LabelView();
	
	if(!view)
	{		
		const char *label = Label();
		
		if(label)
		{
			if(strlen(label))
			{
				font_height h;
			
				GetFontHeight(&h);
			
				btop = ceil(h.ascent)+ceil(h.descent)+ceil(h.leading) + 1;
			}
			else
				btop = border;	
		}	
		else
			btop = border;	
	}
	else
		btop = view->Frame().bottom + border;	
}

/*
 * function    : ReGluing
 * purpose     : redo the gluing of the view
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Box::ReGluing()
{	
	resized_by = RESIZED_BY_GLUING;	

	if(!MagicPtr->Contents)
		return;		
			
	RealBounds = empty;
		
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
			
				widget->VMoveTo(border,btop);
				
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
						vertical = f.Height();
																						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
											
						// we move all the view down
						
						MoveByChilds(MagicPtr->Contents,0,f.Height());
						
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
						vertical = f.Height();				
					
						y = RealBounds.bottom;	
												
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
						horizontal = f.Width();					
						
						RealBounds.right += horizontal;
						RealBounds.bottom += vertical;
																				
						// we move all the view right
						
						MoveByChilds(MagicPtr->Contents,f.Width(),0);
						
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
						horizontal = f.Width();
						
						x = RealBounds.right;
										
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
	
	// due to the fact that we could have a view as label we may increase the size
	// of the widget if it's too small
	
	if(MagicPtr->Label)
	{	
		int w = 10 + LabelView()->Bounds().Width();
		int h = 2 + LabelView()->Bounds().Height();	
			
		if(RealBounds.Width()<w)
			RealBounds.right += w - RealBounds.Width();	
		
		if(RealBounds.Height()<h)
			RealBounds.bottom += h - RealBounds.Height();	
	}		
		
	// we resize the view
					
	resized_count++;
	ResizeTo(RealBounds.Width(),RealBounds.Height());
	RealBounds = Bounds();
	
	original_width = RealBounds.Width();
	original_height = RealBounds.Height();
	
	// Time to see what could be expanded
	
	ExpandChilds();
			
	resized_by = RESIZED_BY_USER;	
}

// Cloning methods

/*
 * function    : Clone
 * purpose     : Set a cloned frame to be the same
 * input       : 
 *
 * SQI_View *clone, cloned view
 *
 * output      : none
 * side effect : none
 */
void SQI_Box::Clone(SQI_Box *clone)
{	
	// first we call the methods of SQI_Widget
	
	SQI_Widget::Clone(clone);
	
	// now we set the specific setting of a View
	
	clone->btop = btop;
	clone->border = border;
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
		
	// pen size
	
	clone->SetPenSize(PenSize());

	// that's all	
}

