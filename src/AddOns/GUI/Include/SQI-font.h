/*
 * Squirrel project
 *
 * file ?	SQI-font.h
 * what	?   Font object
 * who	?	jlv
 * when	?	11/22/99
 * last	?	02/11/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#ifndef _SQI_FONT
#define _SQI_FONT  
 
#include <Font.h>

#define SQI_FONT	9
#define CLASS_FONT	6

class SQI_Font; // forwar def

typedef map<string,SQI_Object *(SQI_Font::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > FontMembers;

class SQI_Font : public SQI_ThePointer , public BFont
{
	public:
		SQI_Font(SQI_Squirrel *squirrel,const BFont *font);
		SQI_Font(SQI_Squirrel *squirrel,const font_family family,const font_style style);
		SQI_Font(SQI_Heap *target,const BFont *font);
		SQI_Font(SQI_Heap *target,const font_family family,const font_style style);
		~SQI_Font();
		// Archive methods
		status_t Archive(BMessage *archive);
  		status_t Instantiate(SQI_Heap *target,BMessage *archive);	
		// Member methods
		SQI_Object *MemberSize(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberDirection(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberEncoding(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberFamily(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberStyle(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberAliasing(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberRotation(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberSpacing(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *MemberShear(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		// expr operations
		bool operator==(SQI_Object *op2);
  		bool operator!=(SQI_Object *op2);			
		FontMembers *Members;
};

// functions

SQI_Font *IsFont(SQI_Object *object);

void InitSQIFont();

SQI_Object *InstantiateAFont(SQI_Heap *target,BMessage *archive);

#endif
