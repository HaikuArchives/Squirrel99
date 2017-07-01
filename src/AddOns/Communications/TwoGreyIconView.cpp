#include <Bitmap.h>
#include <be/translation/TranslationUtils.h>
#include <NodeInfo.h>
#include <Application.h>
#include <Roster.h>

#include "TwoGreyIconView.h"

TwoGreyIconView::TwoGreyIconView(BRect frame, const char *name, uint32 resizingMode, uint32 flags, rgb_color lg, rgb_color dg, BBitmap *ic)
	: BView(frame, name, resizingMode, flags)
{
	darkgrey = dg;
	ltgrey = lg;

	
/*
	if(ic == NULL) {
		app_info info;
	    be_app->GetAppInfo(&info); 

		icon = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);

		if(BNodeInfo::GetTrackerIcon(&info.ref, icon, B_LARGE_ICON) != B_OK) {
			icon->SetBits(kLargeIconBits, 32 * 32 * 8, 0, B_CMAP8);
		}
	} else {
		*/
		icon = ic;
//	}

	SetViewColor(ltgrey);
} 

void
TwoGreyIconView::OverLay(const unsigned char *over)
{
	for(int i = 0; i < 32*32; i++) {
		if(over[i] != 0xff) {
			((unsigned char *)icon->Bits())[i] = over[i];
		}
	}
}

TwoGreyIconView::~TwoGreyIconView()
{
	if(icon)
		delete icon;
}

TwoGreyIconView::TwoGreyIconView(BRect frame, const char *name, uint32 resizingMode, uint32 flags, BBitmap *ic)
	: BView(frame, name, resizingMode, flags)
{
	darkgrey.red = 200;
	darkgrey.blue = 200;
	darkgrey.green = 200;

	ltgrey.red = 220;
	ltgrey.blue = 220;
	ltgrey.green = 220;
	
	if(ic)
	{
		icon = new BBitmap(ic->Bounds(),ic->ColorSpace());
		icon->SetBits(ic->Bits(),ic->BitsLength(),0,ic->ColorSpace());
	}
	else
		icon = NULL;

	SetViewColor(ltgrey);	
}

void
TwoGreyIconView::Draw(BRect updateRect)
{
	SetDrawingMode(B_OP_COPY);
	SetHighColor(darkgrey);
	if(icon)
		FillRect(BRect(0, 0, icon->Bounds().right, Bounds().bottom));

	if(icon)
	{
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(icon, BPoint(icon->Bounds().right / 2, icon->Bounds().bottom / 2));
	}
}
