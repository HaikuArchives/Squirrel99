/*
 * Squirrel project
 *
 * file ?	sqi-font.cpp
 * what	?   Font object
 * who	?	jlv
 * when	?	11/22/99
 * last	?	12/06/99
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "SQI-font.h" 
#include "Family.h"

FontMembers FontMemberMap;

/*
 * function    : SQI_Font
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * BFont *font, font to use for this object
 *
 * output      : none
 * side effect : none
 */
SQI_Font::SQI_Font(SQI_Squirrel *squirrel,const BFont *font)
	:SQI_ThePointer(squirrel,SQI_FONT) , BFont(font)
{
	Members = &FontMemberMap;
	SetClass(CLASS_FONT);
}

/*
 * function    : SQI_Font
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 * font_family family, font family to use
 * font_style style, font style to use
 *
 * output      : none
 * side effect : none
 */
SQI_Font::SQI_Font(SQI_Squirrel *squirrel,const font_family family,const font_style style)
	:SQI_ThePointer(squirrel,SQI_FONT) , BFont()
{
	SetFamilyAndStyle(family,style);
	Members = &FontMemberMap;
	SetClass(CLASS_FONT);
}

/*
 * function    : SQI_Font
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *target, heap to use
 * BFont *font, font to use for this object
 *
 * output      : none
 * side effect : none
 */
SQI_Font::SQI_Font(SQI_Heap *target,const BFont *font)
	:SQI_ThePointer(target,SQI_FONT) , BFont(font)
{
	Members = &FontMemberMap;
}

/*
 * function    : SQI_Font
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Heap *heap, heap to use
 * font_family family, font family to use
 * font_style style, font style to use
 *
 * output      : none
 * side effect : none
 */
SQI_Font::SQI_Font(SQI_Heap *heap,const font_family family,const font_style style)
	:SQI_ThePointer(heap,SQI_FONT) , BFont()
{
	SetFamilyAndStyle(family,style);
	Members = &FontMemberMap;
}

/*
 * function    : SQI_Font
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Font::~SQI_Font()
{
	
}

/*
 * function    : Archive
 * purpose     : Archive the font in a message
 * input       : 
 *
 * BMessage *archive, archive message to use
 *
 * output      : none
 * side effect : none
 */
status_t SQI_Font::Archive(BMessage *archive)
{
	// first the usual Squirrel object flag
	archive->AddInt8("SQI_Object",SQI_FONT);
	// now the font data
	
	// font direction
	archive->AddBool("direction",Direction());
	// font encoding
	archive->AddInt8("encoding",Encoding());
	// font face
	archive->AddInt16("face",Face());
	// font family & style
	archive->AddInt32("family,style",FamilyAndStyle());
	// font flags
	archive->AddInt32("flags",Flags());
	// font rotation
	archive->AddFloat("rotation",Rotation());
	// font shear
	archive->AddFloat("shear",Shear());
	// font size
	archive->AddFloat("size",Size());
	// font spacing
	archive->AddInt8("spacing",Spacing());
	
	return B_OK;
}

/*
 * function    : Instantiate
 * purpose     : Fill the font from the archive message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_Font::Instantiate(SQI_Heap *target,BMessage *archive)
{
	bool direction;
	int8 encoding,spacing;
	int16 face;
	int32 family_style,flags;
	float rotation,shear,size;				

	// first we get all from the message
	
	archive->FindInt8("encoding",&encoding);	
	archive->FindInt16("face",&face);
	archive->FindInt32("family,style",&family_style);
	archive->FindInt32("flags",&flags);
	archive->FindFloat("rotation",&rotation);
	archive->FindFloat("shear",&shear);
	archive->FindFloat("size",&size);			
	archive->FindInt8("spacing",&spacing);
	
	// and now we set the font	
	
	SetFamilyAndStyle(family_style);
	SetEncoding(encoding);
	SetFace(face);
	SetFlags(flags);
	SetRotation(rotation);
	SetShear(shear);
	SetSize(size);
	SetSpacing(spacing);	
	
	return B_OK;
}	

/*
 * function    : Member
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, method name
 * deque<SQI_Object *> *inputs, arguments
 *
 * output      : SQI_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_Font::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_Font::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
		MemberHook = (*Members)[*member];
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
		{
			Members->erase(*member);
			throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unknow member"));
		}	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"member call","unsupported by the object"));
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Font::Print(int prec = 3)
{	
	font_family family;
	font_style style;
	ostrstream out;
  	string *str;

	GetFamilyAndStyle(&family,&style);
	
	out << family << " - " << style << '\0';
   
  	string *ret = new string(out.str());
  	out.freeze(false);
  	return ret;
}	
	
/*
 * function    : Show
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Font::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Clone
 * purpose     : clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_Font::Clone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
		
	SQI_Font *clone = new SQI_Font(hp,static_cast<BFont *>(this));

	return clone;
}

/*
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_Object *, the object created
 * side effect : none
 */
SQI_Object *SQI_Font::DeepClone(SQI_Heap *target = NULL)
{
	return Clone(target);
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Font::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "FONT[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

/*
 * function    : operator==
 * purpose     : test if the object == something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool SQI_Font::operator==(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_FONT)
		{
			SQI_Font *a = dynamic_cast<SQI_Font *>(right);

			return (*(static_cast<BFont *>(this)) == static_cast<BFont *>(a));
		}
		else
			return false;
	}
	else
		return false;	
}

/*
 * function    : operator!=
 * purpose     : test if the object != something
 * input       : none
 *
 * SQI_Object op2, the object to use
 *
 * output      : SQI_Integer
 * side effect : none
 */
bool SQI_Font::operator!=(SQI_Object *op2)
{
	SQI_ThePointer *right = IsThePointer(op2);
	
	if(right)
	{
		if(right->IsA()==SQI_FONT)
		{
			SQI_Font *a = dynamic_cast<SQI_Font *>(right);

			return (*(static_cast<BFont *>(this)) == static_cast<BFont *>(a));
		}
		else
			return true;
	}
	else
		return true;	
}

// Member function of the object

/*
 * function    : MemberSize
 * purpose     : Get or set the font size
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberSize(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
				return squirrel->LocalHeap->Number(Size());
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_Number *num = IsNumber((*inputs)[1]);
					if(num)
					{
						SetSize(Number2Float(num));
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~size","the size must be a number"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~size","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~size","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~size","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~size","need one argument at least"));
}

/*
 * function    : MemberDirection
 * purpose     : Get or set the font direction
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberDirection(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
			{
				if(Direction()==B_FONT_LEFT_TO_RIGHT)
					return squirrel->LocalHeap->Keyword("left2right");
				else
					return squirrel->LocalHeap->Keyword("right2left");
			}	
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~direction","first input must be \"get"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~direction","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~direction","need one input"));
}

/*
 * function    : MemberEncoding
 * purpose     : Get or set the font encoding
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberEncoding(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
				return squirrel->LocalHeap->Number(Encoding());
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_Number *num = IsInteger((*inputs)[1]);
					if(num)
					{
						SetEncoding(Number2Int(num));
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~encoding","the encoding must be a number"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~encoding","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~encoding","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~encoding","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~encoding","need one argument at least"));
}

/*
 * function    : MemberFamily
 * purpose     : Get or set the font family
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberFamily(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
			{
				font_family family;
				
				GetFamilyAndStyle(&family,NULL);	
				
				return squirrel->LocalHeap->String(family);
			}	
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_String *family = IsString((*inputs)[1]);
					if(!family)
						family = IsKeyword((*inputs)[1]);
					
					if(family)
					{
						string *name = family->Data();
						
						if(ExistFont(name))
						{	
							SetFamilyAndStyle(name->c_str(),NULL);
							return squirrel->interpreter->True;
						}
						else
							return squirrel->interpreter->False;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~family","the family must be a string"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~family","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~family","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~family","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~family","need one argument at least"));
}

/*
 * function    : MemberStyle
 * purpose     : Get or set the font style
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberStyle(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
			{
				font_style style;
				
				GetFamilyAndStyle(NULL,&style);	
				
				return squirrel->LocalHeap->String(style);
			}	
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_String *style = IsString((*inputs)[1]);
					if(!style)
						style = IsKeyword((*inputs)[1]);
					
					if(style)
					{
						font_family family;
						string *name = style->Data();
						
						GetFamilyAndStyle(&family,NULL);	
						
						if(ExistStyle(family,name))
						{	
							SetFamilyAndStyle(NULL,name->c_str());
							return squirrel->interpreter->True;
						}
						else
							return squirrel->interpreter->False;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~style","the style must be a string/word"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~style","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~style","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~style","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~style","need one argument at least"));
}

/*
 * function    : MemberAliasing
 * purpose     : Get or set the font anti-aliasing
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberAliasing(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
			{
				uint32 flag = Flags();	
				
				if(flag==B_DISABLE_ANTIALIASING)
					return squirrel->LocalHeap->Keyword("off");
				else
					return squirrel->LocalHeap->Keyword("on");
			}	
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_String *flag = IsString((*inputs)[1]);
					if(!flag)
						flag = IsKeyword((*inputs)[1]);
					
					if(flag)
					{
						string *f = flag->Data();
						
						if(*f == "on")
							SetFlags(B_FORCE_ANTIALIASING);
						else
						if(*f == "off")
							SetFlags(B_DISABLE_ANTIALIASING);
						else
							throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~aliasing","second input must be \"on or \"off"));		
						
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~aliasing","the flag must be a string/word"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~aliasing","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~aliasing","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~aliasing","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~aliasing","need one argument at least"));
}

/*
 * function    : MemberRotation
 * purpose     : Get or set the font rotation
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberRotation(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
				return squirrel->LocalHeap->Number(Rotation());
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_Number *num = IsNumber((*inputs)[1]);
					if(num)
					{
						SetRotation(Number2Float(num));
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~rotation","the rotation must be a number"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~rotation","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~rotation","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~rotation","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~rotation","need one argument at least"));
}

/*
 * function    : MemberSpacing
 * purpose     : Get or set the font spacing
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberSpacing(SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
			{
				uint8 mode = Spacing();	
				
				switch(mode)
				{
					case B_CHAR_SPACING:
						return squirrel->LocalHeap->Keyword("char");
					case B_STRING_SPACING:
						return squirrel->LocalHeap->Keyword("string");
					case B_BITMAP_SPACING:
						return squirrel->LocalHeap->Keyword("bitmap");
					case B_FIXED_SPACING:
						return squirrel->LocalHeap->Keyword("fixed");							
				}
			}	
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_String *flag = IsString((*inputs)[1]);
					if(!flag)
						flag = IsKeyword((*inputs)[1]);
					
					if(flag)
					{
						string *f = flag->Data();
						
						if(*f == "char")
							SetSpacing(B_CHAR_SPACING);
						else
						if(*f == "string")
							SetSpacing(B_STRING_SPACING);
						else
						if(*f == "bitmap")
							SetSpacing(B_BITMAP_SPACING);
						else
						if(*f == "fixed")
							SetSpacing(B_FIXED_SPACING);
						else						
							throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~spacing","second input must be \"char , \"string , \"bitmap or \"fixed"));		
						
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~spacing","the mode must be a string/word"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~spacing","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~spacing","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~spacing","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~spacing","need one argument at least"));
}

/*
 * function    : MemberShear
 * purpose     : Get or set the font shear
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel
 * SQI_Args *input, input to the method
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Font::MemberShear(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=1)
	{
		SQI_Keyword *action = IsKeyword((*inputs)[0]);	
		if(action)
		{
			string *cmd = action->Data();	
			
			if(*cmd == "get")
				return squirrel->LocalHeap->Number(Shear());
			else
			if(*cmd == "set")
			{
				if(inputs->Length()==2)
				{
					SQI_Number *num = IsNumber((*inputs)[1]);
					if(num)
					{
						SetShear(Number2Float(num));
						return NULL;
					}
					else
						throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~shear","the angle must be a number"));
				}
				else
					throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~shear","need two arguments"));	
			}
			else
				throw(new SQI_Exception(SQI_EX_INVALIDE,"FONT~shear","first input must be \"get or \"set"));
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"FONT~shear","first input must be a word"));
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"FONT~shear","need one argument at least"));
}



// some used function

/*
 * function    : InstantiateAFont
 * purpose     : Instantiate a font from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateAFont(SQI_Heap *target,BMessage *archive)
{
	SQI_Font *font = new SQI_Font(target,be_plain_font);
	
	if(font->Instantiate(target,archive)==B_OK)
		return font;
	else
	{
		font->Suicidal(true);
		return NULL;	
	}	
}

// Fill up the member map
void InitSQIFont()
{
	AddArchive(SQI_FONT,InstantiateAFont);	
	
	FontMemberMap[string("aliasing")] = &SQI_Font::MemberAliasing;
	FontMemberMap[string("direction")] = &SQI_Font::MemberDirection;
	FontMemberMap[string("encoding")] = &SQI_Font::MemberEncoding;	
	FontMemberMap[string("family")] = &SQI_Font::MemberFamily;
	FontMemberMap[string("rotation")] = &SQI_Font::MemberRotation;
	FontMemberMap[string("shear")] = &SQI_Font::MemberShear;
	FontMemberMap[string("size")] = &SQI_Font::MemberSize;
	FontMemberMap[string("spacing")] = &SQI_Font::MemberSpacing;
	FontMemberMap[string("style")] = &SQI_Font::MemberStyle;
}

// Return the font if the object is a font
SQI_Font *IsFont(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_FONT)
    	return dynamic_cast<SQI_Font *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}
