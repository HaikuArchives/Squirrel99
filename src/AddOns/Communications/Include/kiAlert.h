/*
 * Squirrel project
 *
 * file ?	kiAlert.h
 * what	?	Kirilla BAlert style window
 * who	?	jlv
 * when	?	02/17/01
 * last	?	02/17/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include <Window.h>
#include <TextView.h>
#include <StringView.h>
#include <Bitmap.h>
#include <Button.h>

#include "TwoGreyIconView.h"

#define ki_ALERT_BUTTON 'kiBA'

class kiAlert : public BWindow 
{
	public:
		kiAlert(BBitmap *icon,const char *title,const char *text,const char *fb,const char *sb = NULL,const char *tb = NULL);
		//~kiAlert();
		void MessageReceived(BMessage *message);
		int32 Go(bool care = true);
	private:
		thread_id caller;
		int32 id;
		TwoGreyIconView	*main;
		BTextView *textview;
		BStringView *titleview;
		BButton *but1;
		BButton *but2;
		BButton *but3;	
}; 
