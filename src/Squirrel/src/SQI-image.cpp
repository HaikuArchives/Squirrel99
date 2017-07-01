/*
 * Squirrel project
 *
 * file ?	SQI_Image.cpp
 * what	?   SQI_Image object
 * who	?	jlv
 * when	?	12/04/00
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 2000-2001
 */
  
#include "SQI-AddOn.h"  

#include <Entry.h>
#include <File.h>
#include <NodeInfo.h>
#include <BitmapStream.h>
#include <be/translation/TranslationUtils.h> 

TMTranslators Translators;

MethodTable *ImageMap = NULL;

status_t SetFileType(BFile * file, int32 translator, uint32 type) 
{ 
        const char *mime = NULL;
        TMTranslators::const_iterator i; 
       
       	for(i=Translators.begin();i!=Translators.end();i++)
       		if(i->second.type == type)
       			break;
        
       	if(i!=Translators.end())
       		mime = i->second.mime; 
         
        if (mime == NULL) {     /* this should not happen, but being defensive might be prudent */ 
                return B_ERROR; 
        } 

        /* use BNodeInfo to set the file type */ 
        BNodeInfo ninfo(file); 
        return ninfo.SetType(mime); 
} 

/*
 * function    : SQI_Image
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::SQI_Image(SQI_Squirrel *squirrel,int width,int height,color_space space)
	:SQI_ThePointer(squirrel,SQI_IMAGE)
{
	SetClass(CLASS_IMAGE);	
	
	Image = new BBitmap(BRect(0,0,width-1,height-1),space,true);
		
	Valid 	= Image->IsValid();
	Path 	= NULL;
	Mime 	= NULL;
	locked	= 0;
	
	if(!ImageMap)
		InitImage();	
	
	Methods = ImageMap;	
}

/*
 * function    : SQI_Image
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::SQI_Image(SQI_Squirrel *squirrel,BRect frame,color_space space)
	:SQI_ThePointer(squirrel,SQI_IMAGE)
{
	SetClass(CLASS_IMAGE);	
	
	Image = new BBitmap(frame,space,true);
		
	Valid 	= Image->IsValid();
	Path 	= NULL;
	Mime 	= NULL;
	locked 	= 0;
	
	if(!ImageMap)
		InitImage();	
	
	Methods = ImageMap;	
}

/*
 * function    : SQI_Image
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::SQI_Image(SQI_Squirrel *squirrel,const char *path)
	:SQI_ThePointer(squirrel,SQI_IMAGE)
{
	SetClass(CLASS_IMAGE);	
	
	Image = NULL;
	
	Valid = load(path);

	if(Valid)
		Path = new string(path);			
	else
	{
		Path = NULL;
		Mime = NULL;
	}

	if(!ImageMap)
		InitImage();	
	
	Methods = ImageMap;
}

/*
 * function    : SQI_Image
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::SQI_Image(SQI_Squirrel *squirrel,BBitmap *src)
	:SQI_ThePointer(squirrel,SQI_IMAGE)
{
	SetClass(CLASS_IMAGE);	
		
	src->Lock();
	
	Image = new BBitmap(src->Bounds(),src->ColorSpace(),true);
	
	uint8 *s = (uint8 *)(src->Bits());	
	uint8 *t = (uint8 *)(Image->Bits());
				
	memcpy(t,s,src->BitsLength());	
					
	src->Unlock();	
	
	Valid = Image->IsValid();
	Path = NULL;
	Mime = NULL;
	
	if(!ImageMap)
		InitImage();	
	
	Methods = ImageMap;	
}

/*
 * function    : SQI_Image
 * purpose     : Constructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::SQI_Image(SQI_Heap *target)
	:SQI_ThePointer(target,SQI_IMAGE)
{
	SetClass(CLASS_IMAGE);	
	
	Image = NULL;
	
	Path = NULL;
	Mime = NULL;

	if(!ImageMap)
		InitImage();	
	
	Methods = ImageMap;
}


/*
 * function    : SQI_Image
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Image::~SQI_Image()
{	
	if(Image)	
		delete Image;
	if(Path)
		delete Path;
	if(Mime)
		delete Mime;		
}

/*
 * function    : Archive
 * purpose     : Store the list in an message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_Image::Archive(BMessage *archive)
{
	if(Image)
	{
		// first we set the type if the object
		archive->AddInt8("SQI_Object",SQI_IMAGE);	
		
  		Image->Lock();

		Image->Archive(archive,false);
  		
  		Image->Unlock();
 
  		return B_OK;
	}
	else
		return B_ERROR;
}

/*
 * function    : Instantiate
 * purpose     : Fill the list with the element in the archive message
 * input       : 
 *
 * BMessage *archive
 *
 * output      : B_OK, if no error, B_ERROR else
 * side effect : none
 */
status_t SQI_Image::Instantiate(SQI_Heap *target,BMessage *archive)
{	
	if(Image)
	{
		static BBitmap *n;	
		
		Image->Lock();
	
		n = dynamic_cast<BBitmap *>(Image->Instantiate(archive));
		
		Image->Unlock();		
		
		if(n)
		{
			delete Image;
			Image = n;	
		}	
	}
	else
		Image = new BBitmap(archive);
	
	return B_OK;
}

/*
 * function    : Member
 * purpose     : Execute a member method on the object
 * input       : none
 *
 * string *member, 			method name
 * SQI_Squirrel *squirrel,  squirrel
 * SQI_Args *inputs, 		arguments
 *
 * output      : SQI_ *, the object created
 * side effect : none
 */
SQI_Object *SQI_Image::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Methods)
	{
		OMeth MemberHook = NULL;		
			
	    MemberHook = Methods->Seek(member->c_str());		
			
		if(MemberHook)	
			return (this->*MemberHook)(squirrel,inputs);
		else
			throw(new SQI_Exception(SQI_EX_INVALIDE,"IMAGE~","unknown member"));	
	}	
	else
		throw(new SQI_Exception(SQI_EX_INVALIDE,"IMAGE~","unsupported by the object"));
}

/*
 * function    : Load
 * purpose     : load an image
 * input       : 
 *
 * long index, index of the value
 * SQI_Oject *value, value to store
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::Load(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==1)
	{
		SQI_String *path = IsString((*inputs)[0]);	
		
		if(path)
		{
			Valid = load(path->Data()->c_str());
		
			if(Valid)
			{
				if(Mime)
					delete Mime;
				if(Path)
					delete Path;	
				
				Path = new string(*path->Data());			
			}	
			else
			{
				Path = NULL;
				Mime = NULL;
			}				
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Image~load","need a string as input"));			
	}	
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image~load","need one inputs"));		
}

/*
 * function    : Save
 * purpose     : save the image
 * input       : 
 *
 * long index, index of the value
 * SQI_Oject *value, value to store
 *
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::Save(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()==2)
	{
		SQI_String *path = IsString((*inputs)[0]);	
		SQI_String *typ = IsKeyword((*inputs)[1]);
		if(!typ)
			typ = IsString((*inputs)[1]);	
		
		if(path && typ)
		{
			string *p = path->Data();
			string *t = typ->Data();
			
			if(save(p->c_str(),t))
				return squirrel->interpreter->True;
			else
				return squirrel->interpreter->False;		
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Image~save","need two strings as inputs"));	
	}
	else
	{
		if(Path)
		{
			if(save(Path->c_str(),Mime))
				return squirrel->interpreter->True;
			else
				return squirrel->interpreter->False;				
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image~save","need two inputs"));		
	}
}

/*
 * function    : IsValid
 * purpose     : load an image in the bitmap
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::IsValid(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Valid)
		return squirrel->interpreter->True;
	else
		return squirrel->interpreter->False;
}

/*
 * function    : Length
 * purpose     : Return the length of the bitmap in byte
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::Length(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	int32 length = 0;
	
	if(Image)
		length = Image->BitsLength();
	
	return new SQI_Number(squirrel->LocalHeap,length);
		
}

/*
 * function    : thePath
 * purpose     : Set or get the path of the image
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::thePath(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=1)
	{
		SQI_Keyword *word = IsKeyword((*inputs)[0]);
		
		if(word)
		{
			string *str = word->Data();
			
			if(*str == "get")
			{
				if(Path)
					return new SQI_String(squirrel->LocalHeap,*Path);
				else
					return new SQI_String(squirrel->LocalHeap,"");				
			}
			else
			if(*str == "set")
			{
				SQI_String *path = IsString((*inputs)[1]);
				if(path)
				{
					Path = new string(*(path->Data()));
					return NULL;	
				}
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Image~path","second input must be a string"));		
			}
			else	
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Image~path","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Image~path","first input must be a word"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image~path","need an input at least"));			
}

/*
 * function    : theMime
 * purpose     : Set or get the mime of the image
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::theMime(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(inputs->Length()>=1)
	{
		SQI_Keyword *word = IsKeyword((*inputs)[0]);
		
		if(word)
		{
			string *str = word->Data();
			
			if(*str == "get")
			{
				if(Mime)
					return new SQI_String(squirrel->LocalHeap,*Mime);
				else
					return new SQI_String(squirrel->LocalHeap,"");				
			}
			else
			if(*str == "set")
			{
				SQI_String *mime = IsString((*inputs)[1]);
				if(mime)
				{
					Mime = new string(*(mime->Data()));
					return NULL;	
				}
				else
					throw(new SQI_Exception(SQI_EX_INVALIDE,"Image~mime","second input must be a string"));		
			}
			else	
				throw(new SQI_Exception(SQI_EX_INVALIDE,"Image~mime","first input must be the word \"get or \"set"));	
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Image~mime","first input must be a word"));	
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Image~mime","need an input at least"));			
}

/*
 * function    : Width
 * purpose     : Output the width of the image
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::Width(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Valid)
		return new SQI_Number(squirrel->LocalHeap,Image->Bounds().Width());
	else
		return squirrel->interpreter->False;
}

/*
 * function    : Height
 * purpose     : Output the width of the image
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Object *SQI_Image::Height(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(Valid)
		return new SQI_Number(squirrel->LocalHeap,Image->Bounds().Height());
	else
		return squirrel->interpreter->False;
}

// Legacy member functions

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Image::Print(int prec = 3)
{	
	if(Path)
  		return new string(*Path);
  	else
  		return new string("");
}	
	
/*
 * function    : Show
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Image::Show(int prec = 3)
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
 * output      : BG_ *, the object created
 * side effect : none
 */
SQI_Object *SQI_Image::Clone(SQI_Heap *target = NULL)
{
	SQI_Heap *hp = target;
	if(!hp)
		hp = heap;	
		


	//return clone;
}

/*
 * function    : DeepClone
 * purpose     : deep clone the object in another heap or the same heap
 * input       : none
 *
 * SQI_Heap *target, heap target where to create the object clone
 *
 * output      : BG_ *, the object created
 * side effect : none
 */
SQI_Object *SQI_Image::DeepClone(SQI_Heap *target = NULL)
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
string *SQI_Image::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "IMAGE[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
}

/*
 * function    : load
 * purpose     : load an image in the bitmap
 * input       :
 *
 * char *path, path of the image
 *
 * output      : bool
 * side effect : none
 */
bool SQI_Image::load(const char *path)
{
	if(Image)
		delete Image;
		
	Image = NULL;	
	
	BEntry entry(path);
	
	if(entry.InitCheck()==B_NO_ERROR)
	{	
		if(entry.Exists())
		{
			BFile file(&entry,B_READ_ONLY);
			BTranslatorRoster *roster = BTranslatorRoster::Default();
			BBitmapStream stream;
			BBitmap *src;
			
			if(roster->Translate(&file,NULL,NULL,&stream,B_TRANSLATOR_BITMAP) < B_OK)
				return false;
			else
			{
				stream.DetachBitmap(&src);
				// we make a copy of the bitmap for a very strange raison, using the one
				// from the stream was making segfault when the bitmap was destroyed at
				// the end of the script
				Image = new BBitmap(src->Bounds(),src->ColorSpace(),true);
				
				uint8 *s = (uint8 *)(src->Bits());	
				uint8 *t = (uint8 *)(Image->Bits());
						
				memcpy(t,s,src->BitsLength());				
				
				//Image->SetBits(src->Bits(),src->BitsLength(),0,src->ColorSpace());
				delete src;	
				
				BNode node(path);	
				BNodeInfo info(&node);
		  		char mime[B_MIME_TYPE_LENGTH];
		  	  		
		  		if(info.GetType(mime)==B_NO_ERROR)
		  			Mime = new string(mime);
		  		else
		  			Mime = NULL;
				  					
				return true;	
			}
		}
		else
			return false;
	}
	else
		return false;		
}

/*
 * function    : save
 * purpose     : save the image in a file
 * input       :
 *
 * char *path, path of the file to create
 * string *format, format of the file to create
 *
 *
 * output      : bool
 * side effect : none
 */
bool SQI_Image::save(const char *path,string *format)
{
	if(Image)
	{
		TMTranslators::const_iterator i;
		
		i = Translators.find(*format);
		
		if(i!=Translators.end())
		{
			int32 translator; 
	        uint32 type; 
	        status_t err; 
	        BFile outputFile(path,B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	       			
			translator = i->second.id;
			type = i->second.type;	
	
	        BBitmapStream input(Image); 
	        err = BTranslatorRoster::Default()->Translate(translator, &input, NULL, &outputFile, type);
	          
	        if (err == B_NO_ERROR) 
	        	err = SetFileType(&outputFile, translator, type); 
	              	
	        input.DetachBitmap(&Image);
	         
	        if(err==B_OK)
	        	return 	true;
	        else
	        	return false;	
		}
		else
			return false;			
	}
	else
		return false;
}

/*
 * function    : import
 * purpose     : Import a bitmap
 * input       :
 *
 * BBitmap *src, bitmap to use
 *
 * output      : bool
 * side effect : none
 */
bool SQI_Image::import(BBitmap *src)
{	
	if(Image)
		delete Image;

	src->Lock();
	
	Image = new BBitmap(src->Bounds(),src->ColorSpace(),true);

	uint8 *s = (uint8 *)(src->Bits());	
	uint8 *t = (uint8 *)(Image->Bits());
				
	memcpy(t,s,src->BitsLength());	
			
	src->Unlock();			
}

BRect SQI_Image::Bounds()
{
	if(Image)
	{
		BRect rect;
		
		Image->Lock();
		rect = Image->Bounds();
		Image->Unlock();
		
		return rect;	
	}
	else
		return BRect(0,0,0,0);	
}

// creation functions and init function

SQI_Image *IsImage(SQI_Object *object)
{
  SQI_ThePointer *ptr = IsThePointer(object);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_IMAGE)
    	return dynamic_cast<SQI_Image *>(ptr);
    else
    	return NULL;
  } 
  else
    return NULL;
}

SQI_ThePointer *NewImage(SQI_Squirrel *squirrel,int width,int height,color_space space)
{
	SQI_Image *img = new SQI_Image(squirrel,width,height,space);	
	return img;
}

SQI_ThePointer *NewImage(SQI_Squirrel *squirrel,const char *path)
{
	SQI_Image *img = new SQI_Image(squirrel,path);	
	return img;
}

// Fill up the member map
void InitImage()
{
	ImageMap = new MethodTable();		
	
	ImageMap->Add("load",(OMeth)(&SQI_Image::Load));
	ImageMap->Add("save",(OMeth)(&SQI_Image::Save));
	ImageMap->Add("valid?",(OMeth)(&SQI_Image::IsValid));
	ImageMap->Add("path",(OMeth)(&SQI_Image::thePath));
	ImageMap->Add("mime",(OMeth)(&SQI_Image::theMime));
	ImageMap->Add("length",(OMeth)(&SQI_Image::Length));
	ImageMap->Add("height",(OMeth)(&SQI_Image::Height));	
	ImageMap->Add("width",(OMeth)(&SQI_Image::Width));		
}

void UnInitImage()
{
	if(ImageMap)
		delete ImageMap;	
}

int32 InitTranslator()
{
	BTranslatorRoster *use;
	uint32 from_type = B_TRANSLATOR_BITMAP;
	
	use = BTranslatorRoster::Default();
	
	char *name;
	int j,k;
    translator_id * ids = NULL; 
    int32 count = 0; 
    status_t err = use->GetAllTranslators(&ids, &count); 
   	if (err < B_OK) return err; 
    for (int tix=0; tix<count; tix++) 
    { 
		const translation_format * formats = NULL; 
        int32 num_formats = 0; 
        bool ok = false; 
        err = use->GetInputFormats(ids[tix], &formats, &num_formats); 
        if (err == B_OK) for (int iix=0; iix<num_formats; iix++) 
        { 
        	if (formats[iix].type == from_type) 
        	{ 
                 ok = true; 
                 break; 
            } 
        } 
                
        if (!ok) continue; 
        err = use->GetOutputFormats(ids[tix], &formats, &num_formats); 
        if (err == B_OK) 
        	for (int oix=0; oix<num_formats; oix++) 
        	{ 
            	if (formats[oix].type != from_type) {            		
            		for(j=0;j<strlen(formats[oix].name);j++)
            			if(formats[oix].name[j]==' ')
            				break;	
            		
            		name = new char[j+1];
            		
            		for(k=0;k<j;k++)
            			name[k] = formats[oix].name[k];
            		
            		name[k] = '\0';	
            		 		
                	Translators[name].id = ids[tix];
                	Translators[name].type = formats[oix].type;	
                	Translators[name].mime = strdup(formats[oix].MIME);	
                	
                	delete name;                                              
                } 
           } 
    } 
    delete[] ids;	
}

/*
 * function    : InstantiateAList
 * purpose     : Instantiate a list from a message archive
 * input       : 
 *
 * SQI_Heap *target, the heap where the object must be created
 * BMessage *archive, the archived list
 *
 *
 * output      : SQI_Object *, the create list
 * side effect : none
 */
SQI_Object *InstantiateAnImage(SQI_Heap *target,BMessage *archive)
{
	SQI_Image *img = new SQI_Image(target);
	
	if(img->Instantiate(target,archive)==B_OK)
		return img;
	else
	{
		img->Suicidal(true);
		return NULL;	
	}	
}
