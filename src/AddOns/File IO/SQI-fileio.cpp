/*
 * Squirrel project
 *
 * file ?	SQI-fileio.cpp
 * what	?	file IO builtin predicates
 * who	?	jlv
 * when	?	10/01/99
 * last	?	03/22/01
 *
 *
 * (c) Kirilla 1999-2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "FileIO.h"

// now some infos on the package

char *FileIO_Pkg_name = "File IO";
char *FileIO_Pkg_purpose = "File Input/Output builtins";
char *FileIO_Pkg_author = "Kirilla";
char *FileIO_Pkg_version = "0.6";
char *FileIO_Pkg_date = "03/22/01";

/*
 * function    : SQI_FileIO_Open
 * purpose     : Builtin predicate "fopen"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_FileIO_Open(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=2)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"fopen","need 2 arguments"));
  
  SQI_Keyword *mode = IsKeyword((*args)[0]);
  SQI_String *path = IsString((*args)[1]);

  if(mode && path)
	return OpenSeqFile(squirrel,path->Data()->c_str(),mode->Data()->c_str()); 
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fopen","need a word and a string"));
}

/*
 * function    : SQI_FileIO_Close
 * purpose     : Builtin predicate "fclose"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_FileIO_Close(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"fclose","need 1 argument"));
  
  SQI_ThePointer *ptr = IsThePointer((*args)[0]);
 
  if(ptr)
  {
	if(ptr->IsA()==SQI_SEQFILE)
	{
		SequentialFile *file = dynamic_cast<SequentialFile *>(ptr);
		
		file->Close();
			
		return NULL;		
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fclose","need a file object"));	
  }		
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fclose","need a file object"));
}

/*
 * function    : SQI_FileIO_Eof
 * purpose     : Builtin predicate "feof"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_FileIO_Eof(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"feof","need 1 argument"));
  
  SQI_ThePointer *ptr = IsThePointer((*args)[0]);
 
  if(ptr)
  {
	if(ptr->IsA()==SQI_SEQFILE)
	{
		SequentialFile *file = dynamic_cast<SequentialFile *>(ptr);
		
		return new SQI_Number(squirrel->LocalHeap,file->Eof());			
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"feof","need a file object"));	
  }		
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"feof","need a file object"));
}

/*
 * function    : SQI_FileIO_Gets
 * purpose     : Builtin predicate "fgets"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_FileIO_Gets(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()!=1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"feof","need 1 argument"));
  
  SQI_ThePointer *ptr = IsThePointer((*args)[0]);
 
  if(ptr)
  {
	if(ptr->IsA()==SQI_SEQFILE)
	{
		SequentialFile *file = dynamic_cast<SequentialFile *>(ptr);
	
		string *str = file->Gets();	
	
		if(str)
		{	
	 		SQI_String *obj = new SQI_String(squirrel->LocalHeap,*str);		
			delete str;
			return obj;
		}
		else
			throw(new SQI_Exception(SQI_EX_ERROR,"fgets","can't read the file"));			
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"feof","need a file object"));	
  }		
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"feof","need a file object"));
}

/*
 * function    : SQI_FileIO_Puts
 * purpose     : Builtin predicate "fputs"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_FileIO_Puts(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(args->Length()<1)
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"fputs","need at least 1 argument"));
  
  SQI_ThePointer *ptr = IsThePointer((*args)[0]);
 
  if(ptr)
  {
	if(ptr->IsA()==SQI_SEQFILE)
	{
		SequentialFile *file = dynamic_cast<SequentialFile *>(ptr);
	
		long i = 0;
		string str;
		string *tmp;
		SQI_Object *obj;

		for(i++;i<args->Length();i++)
		{
			obj = (*args)[i];
			if(obj)
			{
		      	tmp = obj->Print();
				str += *tmp;
				delete tmp;
			}
		}
		
		file->Puts(&str);
		return NULL;
	}
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fputs","need a file object"));	
  }		
  else
    throw(new SQI_Exception(SQI_EX_BADARGTYPE,"fputs","need a file object"));
}

/*
 * function    : SQI_DataS_IsFile
 * purpose     : Builtin predicate "is.file"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_FileIO_IsFile(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.file","need 1 argument"));

  SQI_ThePointer *ptr = IsThePointer((*args)[0]);

  if(ptr)
  {
  	if(ptr->IsA()==SQI_SEQFILE)
    	return interp->True;
    else
    	return interp->False;
  } 
  else
    return interp->False;
}

/*
 * function    : SQI_FileIO_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_FileIO_Remove(SQI_BuiltInMap *Map)
{
  UninitSequentialFileObject();	
	
  Map->RemoveFunc("fopen");
  Map->RemoveFunc("fclose");
  Map->RemoveFunc("feof");
  Map->RemoveFunc("fgets");
  Map->RemoveFunc("fputs");
  Map->RemoveFunc("is.file");
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
	Map->AddAddOn(addon_id,FileIO_Pkg_name,FileIO_Pkg_purpose,FileIO_Pkg_author,FileIO_Pkg_version,FileIO_Pkg_date,SQI_FileIO_Remove);
  
	Map->AddFunc("fopen",
	       FileIO_Pkg_name,
	       "Open a file in a certain mode and output a file object",
	       "fopen mode path",
	       SQI_FileIO_Open
	       );
	       
	Map->AddFunc("fclose",
	       FileIO_Pkg_name,
	       "Close a file",
	       "fclose file_object",
	       SQI_FileIO_Close
	       );
	       
	Map->AddFunc("feof",
	       FileIO_Pkg_name,
	       "Ouput true if there's no more characters to read from the file",
	       "feof file_object",
	       SQI_FileIO_Eof
	       );	  
	       
	Map->AddFunc("fgets",
	       FileIO_Pkg_name,
	       "Read a line from the file and output a string object",
	       "fgets file_object",
	       SQI_FileIO_Gets
	       );
	       	       
	Map->AddFunc("fputs",
	       FileIO_Pkg_name,
	       "Write a line in the file",
	       "fputs file_object",
	       SQI_FileIO_Puts
	       ); 
	       
	Map->AddFunc("is.file",
	       FileIO_Pkg_name,
	       "Output true if the input is a file",
	       "is.file thing",
	       SQI_FileIO_IsFile
	       );    
}
