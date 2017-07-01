/*
 * Squirrel project
 *
 * User Interface
 *
 * file ?	SQI-window.h
 * what	?	Window class 
 * who	?	jlv
 * when	?	08/05/99
 * last	?	08/14/99
 *
 *
 * (c) electron Technology 1999
 */

class MyWindow : public BWindow
{
	public:
		MyWindow(BRect frame,char *title);
		~MyWindow();
		bool QuitRequested();
		void MessageReceived(BMessage *message);
};
