/*
 * Squirrel project
 *
 * file ?	SQI-picture.h
 * what	?   Picture object
 * who	?	jlv
 * when	?	03/01/00
 * last	?	03/01/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include <Picture.h>

#define SQI_PICTURE		32
#define CLASS_PICTURE	8

class SQI_Picture : public SQI_ThePointer
{
	public:
		SQI_Picture(SQI_Squirrel *squirrel,BPicture *picture);
		SQI_Picture(SQI_Heap *target,BPicture *picture);
		~SQI_Picture();
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		// expr operations
		bool operator==(SQI_Object *op2);
  		bool operator!=(SQI_Object *op2);
		BPicture *picture;
};

// functions

SQI_Picture *IsPicture(SQI_Object *object);
