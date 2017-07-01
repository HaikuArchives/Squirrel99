/*
 * Squirrel project
 *
 * file ?	SQI-image.h
 * what	?   Image object
 * who	?	jlv
 * when	?	12/04/00
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include <Bitmap.h> 
#include "Trans.h"

extern TMTranslators Translators;
 
#define SQI_IMAGE	34
#define CLASS_IMAGE	12

class SQI_Image; // forwar def

class SQI_Image : public SQI_ThePointer
{
	public:
		SQI_Image(SQI_Squirrel *squirrel,BBitmap *src);
		SQI_Image(SQI_Squirrel *squirrel,int width,int height,color_space space);
		SQI_Image(SQI_Squirrel *squirrel,BRect frame,color_space space);
		SQI_Image(SQI_Squirrel *squirrel,const char *path);
		SQI_Image(SQI_Heap *target);
		~SQI_Image();
		// functions
		bool load(const char *path);
		bool save(const char *path,string *format);
		bool import(BBitmap *src);
		bool IsLocked() {return locked;};
		void Lock() {locked++;};
		void Unlock() {locked--;};
		BRect Bounds();
		// Member methods
		SQI_Object *Load(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Save(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *IsValid(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Length(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *thePath(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *theMime(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Width(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Height(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		BBitmap *GetBitmap() {return Image;};
  		status_t Archive(BMessage *archive);
  		status_t Instantiate(SQI_Heap *target,BMessage *archive);					
	private:
		long locked;
		BBitmap *Image;
		bool Valid;
		string *Path;
		string *Mime;
};

// functions

SQI_Image *IsImage(SQI_Object *obj);

SQI_ThePointer *NewImage(SQI_Squirrel *squirrel,int width,int height,color_space space);
SQI_ThePointer *NewImage(SQI_Squirrel *squirrel,const char *path);

void InitImage();
void UnInitImage();
int32 InitTranslator();

SQI_Object *InstantiateAnImage(SQI_Heap *target,BMessage *archive);
