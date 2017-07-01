/*
 * Extra Widgets
 *
 * Some more cool widgets
 *
 * file ?	RButton.h
 * what	?	Round Button class 
 * who	?	jlv
 * when	?	02/04/00
 * last	?	02/25/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Button.h> 
#include "Colors.h"
 
class RButton : public BButton {

	public:
		RButton(BRect frame,const char *name,const char *label,BMessage *message,uint32 resizingMode,uint32 flags);
		~RButton();
		virtual void GetPreferredSize(float *width,float *height);
		virtual void FrameResized(float width,float height);
		virtual void AttachedToWindow();
		virtual void Draw(BRect updateRect);
		virtual void MakeFocus(bool focused = true);
		virtual void SetEnabled(bool enabled);
		virtual void SetLabel(const char *string);
		virtual void SetValue(int32 value);
		virtual status_t Invoke(BMessage *message = NULL);
		void LabelPos();
		void SubLabel(char *label);
	private:	
		void DrawOff();
		void DrawOn();
		void DrawFocus();
		void ClearFocus();
		void DrawLabel();
		rgb_color	ShadowColor;	/* Shadow color */
		rgb_color	ButtonColor;	/* Button color */
		rgb_color	OnColor;		/* Button clicked color */
		int			Level;			/*  how high is the button */
		bool 		Status;			/* status of the button on or not */
		int *Labelx,*Labely;		/* Position of the pen for the labels */
		char **Labels;				/* Elements of the label */	
		int	NbLabels;				/* Number of labels */ 		
};
