/*
 * Squirrel project
 *
 * file ?	kiAlert.cpp
 * what	?	Kirilla BAlert style window
 * who	?	jlv
 * when	?	02/17/01
 * last	?	02/17/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "kiAlert.h" 

#include <Screen.h>

char *strsub(const char *src,int i,int j)
{
	char *trg = new char[j-i+2];	
	int32 s=0;	
	
	for(int32 k=i;k<=j;k++)
		trg[s++] = src[k];	
	
	trg[s] = '\0';	
	
	return trg;	
}

void ComputeSize(const char *text,float *width,float *height)
{
	font_height ht;
	char *l;
	float max = 0,w;
	uint32 i,last = 0;
	int32 lines = 1;
	// how many line is there and how long are they ?
	for(i=0;i<strlen(text);i++)
	{
		if(text[i]=='\n')
		{
			if(last!=i)
			{
				l = strsub(text,last,i);
				w = be_plain_font->StringWidth(l);
				if(w > max)
					max = w;
				delete l;
			}
			lines++;
			last = i+1;		
		}	
	}
	
	if(last!=(i-1))
	{
		// take car of the last line
		l = strsub(text,last,i-1);
		w = be_plain_font->StringWidth(l);
		if(w > max)
			max = w;
		delete l;
		lines++;	
	}	
	
	be_plain_font->GetHeight(&ht);	
	
	*width = max;
	*height = lines * (be_plain_font->Size() + ht.descent);	
}

#include <iostream>

BRect ButtonBounds(const BFont *font,const char *label)
{
	BRect rect(0,0,0,0);
	font_height ht;	
	font->GetHeight(&ht);
	
	rect.right 	= font->StringWidth(label) + 20 - 1;
	rect.bottom	= font->Size() + ht.leading + 10 -1;
		
	return rect;	
}

BRect MaxRect(BRect r1,BRect r2)
{	
	if(r1.Width()>r2.Width())
		return r1;
	else
		return r2;	
}
 
kiAlert::kiAlert(BBitmap *icon,const char *title,const char *text,const char *fb,const char *sb = NULL,const char *tb = NULL)
	:BWindow(BRect(0,0,0,0),title,B_MODAL_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,B_NOT_RESIZABLE)
{	
	BMessage *message;
	BRect 	brect(0,0,0,0);	// button rect
	float xi,yi,yt,yspace,wb=0;	
	int ispace;
	
	if(icon)
	{
		xi = 54;
		yi = 10;
		yt = 15;
		ispace = 64;
		yspace = 59;	
	}	
	else
	{
		xi = 10;
		yi = 10;
		yt = 5;	
		ispace = 0;
		yspace = 20;
	}
	
	if(title)
	{
		if(strlen(title))
		{
			// create the title view
			titleview = new BStringView(BRect(xi,yi,0,0),NULL,title);
			// set it font
			titleview->SetFont(be_bold_font);
			// resize to preferred
			titleview->ResizeToPreferred();	
		}
		else
			titleview = NULL;
	}
	else
		titleview = NULL;	
		
	// get size of the textview
	float w,h;
	ComputeSize(text,&w,&h);		
	// create the TextView	
			
	if(titleview)		
		textview = new BTextView(BRect(xi,35,xi+w,35+h),NULL,BRect(1,1,w-1,h-1),B_FOLLOW_NONE);
	else
		textview = new BTextView(BRect(xi,yt,xi+w,yt+h),NULL,BRect(1,1,w-1,h-1),B_FOLLOW_NONE);	
			
	textview->SetText(text);
	textview->MakeEditable(false);
	textview->MakeSelectable(false);
	rgb_color c;
	c.red = 220;
	c.blue = 220;
	c.green = 220;
	textview->SetViewColor(c);
						
	// create the first button
	if(fb)
	{	
		message = new BMessage(ki_ALERT_BUTTON);
		message->AddInt32("which",0);
		
		but1 = new BButton(ButtonBounds(be_plain_font,fb),NULL,fb,message,B_FOLLOW_NONE);
		brect = but1->Bounds();	
		wb += brect.Width() + 10;
	}
	else
		but1 = NULL;
		
	// create the second button
	if(sb)
	{	
		message = new BMessage(ki_ALERT_BUTTON);
		message->AddInt32("which",1);
		but2 = new BButton(MaxRect(brect,ButtonBounds(be_plain_font,sb)),NULL,sb,message,B_FOLLOW_NONE);
		brect = but2->Bounds();
		wb += brect.Width() + 10;		
	}
	else
		but2 = NULL;				
	
	// create the third button
	if(tb)
	{	
		message = new BMessage(ki_ALERT_BUTTON);
		message->AddInt32("which",2);
		but3 = new BButton(MaxRect(brect,ButtonBounds(be_plain_font,tb)),NULL,tb,message,B_FOLLOW_NONE);
		brect = but3->Bounds();
		wb += brect.Width() + 10;		
	}
	else
		but3 = NULL;	
	
	//BRect rect(0,0,(brect.Width()+10)*3 + ispace,yspace); // 59
	BRect rect(0,0,wb + ispace,yspace);	
		
			
	float ypos;		
			
	if(titleview)		
		rect = rect | titleview->Frame();
	rect = rect | textview->Frame();
	ypos = rect.bottom + 5;
	rect.bottom += 10 + but1->Bounds().Height();
	rect.right += 20; // always add 20 pixels to the width
		
	ResizeTo(rect.Width(),rect.Height());	
		
	// create the 2greyiconview
	main = new TwoGreyIconView(rect,NULL,B_FOLLOW_ALL,B_WILL_DRAW,icon);	
	
	// place them on the main view
	if(titleview)		
		main->AddChild(titleview);	
	main->AddChild(textview);		
	
	float xpos = rect.right;	
	// place buttons	
	if(but3)
	{		
		xpos -= but3->Bounds().Width() + 10;
		but3->MoveTo(xpos,ypos);
		main->AddChild(but3);
		if(but2)
			but3->MakeDefault(true);
	}	
	if(but2)
	{		
		xpos -= but2->Bounds().Width() + 10;
		but2->MoveTo(xpos,ypos);
		main->AddChild(but2);
		if(but1)
			but2->MakeDefault(true);
	}	
	if(but1)
	{		
		xpos -= but1->Bounds().Width() + 10;
		but1->MoveTo(xpos,ypos);
		main->AddChild(but1);
		if(!but2)
			but1->MakeDefault(true);
	}	
	
	// add main and resize window
	AddChild(main);	
	
	// move the alert in the middle of the screen
	BScreen screen;
	
	BRect f = screen.Frame();
	
	BPoint p(f.Width()/2,f.Height()/2);
	
	p.x -= Frame().Width()/2;
	p.y -= Frame().Height()/2;
	
	MoveTo(p);
} 

int32 kiAlert::Go(bool care = true)
{
	
	Show();
	if(!care)
	{
		// suspend calling thread
		caller = find_thread(NULL);
		suspend_thread(caller);
		return id;
	}
	else
		return 0;	
}

void kiAlert::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case ki_ALERT_BUTTON:
		{
			id = message->FindInt32("which");
			resume_thread(caller);
			PostMessage(B_QUIT_REQUESTED);
			break;	
		}
		default:
			BWindow::MessageReceived(message);	
	}		
}
