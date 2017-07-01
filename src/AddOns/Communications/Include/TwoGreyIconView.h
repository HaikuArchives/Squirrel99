#ifndef TWOGREYICONVIEW_H
#define TWOGREYICONVIEW_H

#include <View.h>

class BBitmap;


class TwoGreyIconView : public BView
{
	public:

		TwoGreyIconView(BRect frame, const char *name, uint32 resizingMode, uint32 flags, BBitmap *ic = NULL);
		TwoGreyIconView(BRect frame, const char *name, uint32 resizingMode, uint32 flags, rgb_color lg, rgb_color dg, BBitmap *ic = NULL);
		~TwoGreyIconView();
		virtual void Draw(BRect updateRect);

		void OverLay(const unsigned char *name);

	private:
		rgb_color darkgrey;
		rgb_color ltgrey;
		BBitmap *icon;
};

#endif