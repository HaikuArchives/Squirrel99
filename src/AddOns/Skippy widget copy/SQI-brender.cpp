/*
 * Squirrel project
 *
 * file ?	SQI-brender.cpp
 * what	?   Skippy render in a bitmap
 * who	?	jlv
 * when	?	01/24/01
 * last	?	01/24/01
 *
 *
 * (c) Kirilla 2001
 */

#include "SQI-AddOn.h"
#include "SQI-playground.h" 
#include "SQI-brender.h"
#include "Skippy.h"

SQI_BRender::SQI_BRender(SQI_Image *img)
	:SQI_Playground()
{
	Img = img;
	Img->AddRef();
	
	bitmap = Img->GetBitmap();	
	
	bitmap->Lock();
	View = new BView(bitmap->Bounds(),NULL,B_FOLLOW_NONE,B_WILL_DRAW);
	bitmap->Unlock();	
	
	bitmap->AddChild(View);			
}

SQI_BRender::~SQI_BRender()
{
	bitmap->RemoveChild(View);
	delete View;
	Img->RemRef();	
}
 
BView *SQI_BRender::GetView()
{
	return View;	
}

void SQI_BRender::Register(Skippy *skippy)
{
	SQI_Playground::Register(skippy);
	
	bitmap->Lock();
	BRect bounds = View->Bounds();
	bitmap->Unlock();	
	BPoint home((float)(bounds.Width()) / 2.0 -1,(float)(bounds.Height()) / 2.0 -1);
	skippy->SetHome(home);
	skippy->SetPosition(home);	
}

void SQI_BRender::Unregister(Skippy *skippy)
{
	SQI_Playground::Unregister(skippy);	
}

void SQI_BRender::SetHome(BPoint pos)
{
	SQI_Playground::SetHome(pos);	
}

void SQI_BRender::Reserve()
{
	bitmap->Lock();	
}

void SQI_BRender::Terminate(BPoint from,BPoint to,int pen)
{
	//View->Sync();	
	bitmap->Unlock();	
}
 
 
