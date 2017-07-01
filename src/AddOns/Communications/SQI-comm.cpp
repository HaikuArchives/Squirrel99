/*
 * Squirrel project
 *
 * file ?	SQI-comm.cpp
 * what	?	Communication (I/O) builtin predicate
 * who	?	jlv
 * when	?	07/27/99
 * last	?	03/24/01
 *
 *
 * (c) Kirilla 1999-2001
 */

// First the needed header

#include <Alert.h>
#include <Resources.h>
#include <File.h>
#include <Bitmap.h>

#include <iostream.h>
//#include "SQI-AddOn.h"

#include "myToken.h"
#include "tokens.h"
#include "SQIParser.h"
#include "DLexerBase.h"
#include "DLGLexer.h"
#include "NestedTokenStream.h"
#include "ATokenBuffer.h"
#include "SQI-parser.h"
#include <Locker.h>
#include <Message.h>

#include "kiAlert.h"

// now some infos on the package

char *Comm_Pkg_name = "Communication";
char *Comm_Pkg_purpose = "I/O builtin";
char *Comm_Pkg_author = "Kirilla";
char *Comm_Pkg_version = "0.18";
char *Comm_Pkg_date = "03/24/01";

extern SQI_Parser<DLGLexer, SQIParser, ANTLRToken> *TheParser;

//SQI_Parser<DLGLexer, SQIParser, ANTLRToken> *TheParser; // An parser for the own use of the interpreter (read.***)
BLocker ParserLocker; // locker of the parser

const char *ThePath = "/boot/home/config/add-ons/Squirrel/Mandatory/Communication.so"; //path to the add-on
BBitmap *Icons[6] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL	
};	// standard icon took from the add-resources

bool GetStdIcon()
{
	BFile file(ThePath,B_READ_ONLY);
	BResources res;
	
	if(file.InitCheck()==B_NO_ERROR)
	{
		if(res.SetTo(&file,false) == B_NO_ERROR)
		{
			void *data;
			size_t size;
			
			// get  all the icons
			for(int32 i=1;i<6;i++)
			{	
				if((data = res.FindResource('ICON',i-1,&size))!=NULL)
				{
					Icons[i] = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
					Icons[i]->SetBits(data,size,0,B_CMAP8);	
					delete data;	
				}
			}
				
			return true;
		}
		else
			return false;
	}
	else
		return false;	
}

void DelStdIcon()
{
	for(int32 i=1;i<6;i++)
		if(Icons[i])
			delete Icons[i];	
}


/*
 * function    : ParseAString
 * purpose     : Parse a string and return the object to it or NULL if an error had occured
 * input       : 
 *
 * char *str, the string
 *
 * output      : SQI_Object *, an object
 * side effect : none
 */
SQI_Object *ParseAString(SQI_Interp *interp,char *str)
{
	int errcode;
	SQI_Object *obj = NULL;
	// we set the stream to the string
	
	ParserLocker.Lock();	
	
	TheParser->Stream(str);
	
	obj = TheParser->parser()->elist(&errcode,interp);	
		
	if(errcode)
    {
    	if(obj)
    	{
    		obj->Suicidal(true);
    		obj = NULL;	
    	}	
    }
    
    ParserLocker.Unlock();
    
    return obj;	
}

/*
 * function    : ParseAString
 * purpose     : Parse a string and return the object to it or NULL if an error had occured
 * input       : 
 *
 * string *str, the string
 *
 * output      : SQI_Object *, an object
 * side effect : none
 */
SQI_Object *ParseAString(SQI_Interp *interp,string *str)
{
	int errcode;
	SQI_Object *obj = NULL;
	
	ParserLocker.Lock();	
	
	// we set the stream to the string
	TheParser->Stream(str);
	
	obj = TheParser->parser()->elist(&errcode,interp);	
		
	if(errcode)
    {
    	if(obj)
    	{
    		obj->Suicidal(true);
    		obj = NULL;	
    	}	
    }
    
    ParserLocker.Unlock();
    
    return obj;
	
}

/*
 * function    : SQI_Comm_Print
 * purpose     : Builtin predicate "print"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Print(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	SQI_Object *obj = NULL;		
  	long i;
  	string *str;
  	  	
  	interp->Stream->Lock();

  	for(i=0;i<args->Length();i++)
    {
   		obj = (*args)[i]; 	
      	if(obj)
		{
	  		str = obj->Print(squirrel->precision);
	  		*(interp->Stream->out) << *str << " ";
	  		delete str;
		}
    }
    
  	*(interp->Stream->out) << "\n" << '\0';
  	    
  	interp->Stream->Flush();
  	
  	interp->Stream->Unlock();

  	return NULL;
}

/*
 * function    : SQI_Comm_Show
 * purpose     : Builtin predicate "show"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Show(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	SQI_Object *obj=NULL;	
 	long i;
  	string *str;
  	
  	interp->Stream->Unlock();

  	for(i=0;i<args->Length();i++)
    {
    	obj = (*args)[i];
      	if(obj)
		{
	  		str = obj->Show(squirrel->precision);
	  		*(interp->Stream->out) << *str << " ";
	  		delete str;
		}
    }
  
  	*(interp->Stream->out) << "\n" << '\0';
  	  
  	interp->Stream->Flush();
  	
  	interp->Stream->Unlock();

  	return NULL;
}

/*
 * function    : SQI_Comm_Type
 * purpose     : Builtin predicate "type"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Type(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	SQI_Object *obj=NULL;
 	long i;
  	string *str;
  	
  	interp->Stream->Lock();

  	for(i=0;i<args->Length();i++)
    {
    	obj = (*args)[i];
      	if(obj)
		{
	  		str = obj->Print(squirrel->precision);
	  		*(interp->Stream->out) << *str;
	  		delete str;
		}
    }
       
  	*(interp->Stream->out) << '\0';
  	  
  	interp->Stream->Flush(); 
  	
  	interp->Stream->Unlock(); 

  	return NULL;
}

/*
 * function    : SQI_Comm_ReadNumber
 * purpose     : Builtin predicate "read.number"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadNumber(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	
	// first we wait for the input	
	
	ParserLocker.Lock();
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
      
    SQI_Object *obj = ParseAString(interp,str);
    
    free(str);
    
    ParserLocker.Unlock();
      
    if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.number","was waiting a number"));	
  	else
  	{
  		SQI_Number *num = IsNumber(obj);
  		if(num)
  		{
  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return num;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.number","was waiting a number"));		
  		}
  	}
}

/*
 * function    : SQI_Comm_ReadInteger
 * purpose     : Builtin predicate "read.integer"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadInteger(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	
	// first we wait for the input	
	
	ParserLocker.Lock();
		
 	str = interp->Stream->Wait();
 	    
    SQI_Object *obj = ParseAString(interp,str);
    
    free(str);
    
    ParserLocker.Unlock();
      
    if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.integer","was waiting an integer"));	
  	else
  	{
  		SQI_Number *num = IsInteger(obj);
  		if(num)
  		{
  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return num;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.integer","was waiting an integer"));		
  		}
  	}
}

/*
 * function    : SQI_Comm_ReadFloat
 * purpose     : Builtin predicate "read.float"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadFloat(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	
	ParserLocker.Lock();	
	
	// first we wait for the input	
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
     
    SQI_Object *obj = ParseAString(interp,str);
      
    free(str);
    
	ParserLocker.Unlock();     
      
    if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.float","was waiting a float"));	
  	else
  	{
  		SQI_Number *num = IsFloat(obj);
  		if(num)
  		{
  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return num;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.float","was waiting a float"));		
  		}
  	}
}

/*
 * function    : SQI_Comm_ReadString
 * purpose     : Builtin predicate "read.string"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadString(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	string thestring("'");
	
	// first we wait for the input
	
	ParserLocker.Lock();		
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
 	
 	thestring.append(str);
 	thestring.append("'");
 	
 	free(str);
 	
 	ParserLocker.Unlock();	
       
   	SQI_Object *obj = ParseAString(interp,&thestring);
   	
   	if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.string","was waiting a string"));	
  	else
  	{
  		SQI_String *s = IsString(obj);
  		if(s)
  		{
  			s->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return s;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.string","was waiting a string"));		
  		}
  	}
}

/*
 * function    : SQI_Comm_ReadList
 * purpose     : Builtin predicate "read.list"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	string thestring("[");
	
	// first we wait for the input
	
	ParserLocker.Lock();		
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
 	
 	thestring.append(str);
 	thestring.append("]");
 	
 	free(str);
       
   	SQI_Object *obj = ParseAString(interp,&thestring);
   	
	ParserLocker.Unlock();   	
   	
   	if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.list","was waiting a list"));	
  	else
  	{
  		SQI_List *s = IsList(obj);
  		if(s)
  		{
  			s->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return s;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.list","was waiting a list"));		
  		}
  	}

}

/*
 * function    : SQI_Comm_ReadWord
 * purpose     : Builtin predicate "read.word"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadWord(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	string thestring("\"");
	
	// first we wait for the input
	
	ParserLocker.Lock();	
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
 	
 	thestring.append(str);
 	
 	free(str);
       
   	SQI_Object *obj = ParseAString(interp,&thestring);
   
 	ParserLocker.Unlock();  
   	
   	if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.word","was waiting a word"));	
  	else
  	{
  		SQI_Keyword *s = IsKeyword(obj);
  		if(s)
  		{
  			s->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return s;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.word","was waiting a word"));		
  		}
  	}

}

/*
 * function    : SQI_Comm_ReadBlock
 * purpose     : Builtin predicate "read.block"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadBlock(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;
	string thestring("{");
	
	// first we wait for the input
	
	ParserLocker.Lock();		
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
 	
 	thestring.append(str);
 	thestring.append("}");
 	
 	free(str);
       
   	SQI_Object *obj = ParseAString(interp,&thestring);
 
	ParserLocker.Lock(); 
   	
   	if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.block","was waiting a block"));	
  	else
  	{
  		SQI_nBlock *s = IsBlock(obj);
  		if(s)
  		{
  			s->Export(squirrel->LocalHeap,SQI_NORMAL);
  			return s;	
  		}	
  		else
  		{
  			obj->Suicidal(true);
  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.list","was waiting a block"));		
  		}
  	}
}

/*
 * function    : SQI_Comm_ReadAnything
 * purpose     : Builtin predicate "read.anything"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ReadAnything(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	char *str;

	// first we wait for the input
	
	ParserLocker.Lock();		
		
 	str = interp->Stream->Wait();
 	
 	// we have it, so we
 
   	SQI_Object *obj = ParseAString(interp,str);
   	
   	free(str);
   	
	ParserLocker.Unlock();   	
   	
   	if(!obj) 
  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"read.anything","was waiting anything"));	
  	else
  		return obj;
}

/*
 * function    : SQI_Comm_ParseInteger
 * purpose     : Builtin predicate "parse.integer"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseInteger(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.integer","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
		ParserLocker.Lock(); 		
 		
	    SQI_Object *obj = ParseAString(interp,str->Data());
	   
	  	ParserLocker.Unlock(); 
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.integer","no integer found"));	
	  	else
	  	{
	  		SQI_Number *num = IsInteger(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.integer","no integer found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.integer","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseFloat
 * purpose     : Builtin predicate "parse.float"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseFloat(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.float","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		ParserLocker.Lock();	
 		
	    SQI_Object *obj = ParseAString(interp,str->Data());
	   
	  	ParserLocker.Unlock(); 
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.float","no float found"));	
	  	else
	  	{
	  		SQI_Number *num = IsFloat(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.float","no float found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.float","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseNumber
 * purpose     : Builtin predicate "parse.number"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseNumber(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.number","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{		
 		ParserLocker.Lock();
 			
	    SQI_Object *obj = ParseAString(interp,str->Data());
	   	      
		ParserLocker.Unlock();   	      
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.number","no number found"));	
	  	else
	  	{
	  		SQI_Number *num = IsNumber(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.number","no number found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.number","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseString
 * purpose     : Builtin predicate "parse.string"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseString(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.string","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		string thestring("'");
 		thestring.append(*str->Data());
 		thestring.append("'");
 		
 		ParserLocker.Lock();	
 		
	    SQI_Object *obj = ParseAString(interp,&thestring);
	   	      
	   	ParserLocker.Unlock();      
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.string","no string found"));	
	  	else
	  	{
	  		SQI_String *num = IsString(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.string","no string found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.string","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseList
 * purpose     : Builtin predicate "parse.list"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseList(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.list","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		string thestring("[");
 		thestring.append(*str->Data());
 		thestring.append("]");
 		
 		ParserLocker.Lock();	
 		
	    SQI_Object *obj = ParseAString(interp,&thestring);
	   	      
	   	ParserLocker.Unlock();      
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.list","no list found"));	
	  	else
	  	{
	  		SQI_List *num = IsList(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.list","no list found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.list","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseWord
 * purpose     : Builtin predicate "parse.word"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseWord(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.word","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		string thestring("\"");
 		thestring.append(*str->Data());
 		
 		ParserLocker.Lock();	
	    SQI_Object *obj = ParseAString(interp,&thestring);
		ParserLocker.Unlock();   	
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.word","no word found"));	
	  	else
	  	{
	  		SQI_Keyword *num = IsKeyword(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.word","no word found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.list","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseBlock
 * purpose     : Builtin predicate "parse.block"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseBlock(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.block","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		string thestring("{");
 		thestring.append(*str->Data());
 		thestring.append("}");
 		
 		ParserLocker.Lock();
	    SQI_Object *obj = ParseAString(interp,&thestring);
	  	ParserLocker.Unlock(); 
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.block","no block found"));	
	  	else
	  	{
	  		SQI_nBlock *num = IsBlock(obj);
	  		if(num)
	  		{
	  			num->Export(squirrel->LocalHeap,SQI_NORMAL);
	  			return num;	
	  		}	
	  		else
	  		{
	  			obj->Suicidal(true);
	  			throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.block","no block found"));		
	  		}
	  	}
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.block","need one string as input"));		
}

/*
 * function    : SQI_Comm_ParseAnything
 * purpose     : Builtin predicate "parse.anything"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_ParseAnything(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"parse.anything","need one string as input"));		

	SQI_String *str = IsString((*args)[0]); 
 
 	if(str)
 	{
 		ParserLocker.Lock();
	    SQI_Object *obj = ParseAString(interp,str->Data());
	  	ParserLocker.Unlock(); 
	   	      
	    if(!obj) 
	  		throw(new SQI_Exception(SQI_EX_TYPEMISMATCH,"parse.anything","nothing found"));	
	  	else
			return obj;
 	}
 	else
 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.anything","need one string as input"));		
}

/*
 * function    : LoadFile
 * purpose     : Load a file using the TheParser
 * input       : 
 *
 * SQI_Interp *interp, the interpreter
 * string *filename, the filename
 *
 * output      : SQI_Object *, the object result of the loading
 * side effect : none
 */
SQI_Object *LoadFile(SQI_Interp *interp,string *filename)
{
	int errcode;
	SQI_Object *obj = NULL;
	// we set the stream to the file string
	FILE *f;
	
	if(f=fopen(filename->c_str(),"r"))
	{
		int errcode;
		ParserLocker.Lock();
		TheParser->Stream(filename->c_str(),f);
		obj = TheParser->parser()->interp(&errcode,interp);	
		ParserLocker.Unlock();	
		if(!errcode)
    	{
  			fclose(f);
  			return obj;			
    	}
		else
			return NULL;
    }
    else
    {
    	*(interp->Stream->err) << "Cannot access " << *filename << "\n" << '\0';
       interp->Stream->Flush();	
    }  
}

/*
 * function    : ExecObject
 * purpose     : Execute a block loaded
 * input       : 
 *
 * SQI_Interp *interp, the interpreter
 * SQI_Squirrel *squirrel, the squirrel
 * SQI_Object *obj, the object loaded
 *
 * output      : none
 * side effect : none
 */
void ExecObject(SQI_Interp *interp,SQI_Squirrel *squirrel,SQI_Object *obj)
{
	SQI_nBlock *block = IsBlock(obj);
	if(block)
	{
		squirrel->HopOnABlock(interp,block);
		block->Suicidal(true);
	}	
}

/*
 * function    : SQI_Comm_Load
 * purpose     : Builtin predicate "load"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Load(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"load","need at least one string as input"));		

	for(int i=0;i<args->Length();i++)
	{
		SQI_String *str = IsString((*args)[i]); 
	 
	 	if(str)
	 	{
		    SQI_Object *obj = LoadFile(interp,str->Data());
		   	if(obj)
		   		ExecObject(interp,squirrel,obj);
	 	}
	 	else
	 		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"parse.anything","need one string as input"));
 		
	}
	
	return NULL;		
}

/*
 * function    : SQI_Comm_Precision
 * purpose     : Builtin predicate "Precision"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Precision(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<1)
		return new SQI_Number(squirrel->LocalHeap,squirrel->precision);
	else
	{
			SQI_Number *prec = IsInteger((*args)[0]);
			if(prec)
				squirrel->precision = (int8)Number2Int(prec);	
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Precision","need an integer as input"));		
	}	
	
	return NULL;		
}

/*
 * function    : SQI_Comm_Message
 * purpose     : Builtin predicate "Message"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Comm_Message(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()!=1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Message","need an integer as input"));			
	else
	{
			SQI_Number *cmd = IsInteger((*args)[0]);
			if(cmd)
			{	
				BMessage *msg = new BMessage((uint32)Number2Long(cmd));
				
				return new SQI_Message(squirrel,msg);	
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Message","need an integer as input"));		
	}	
	
	return NULL;		
}

/*
 * function    : SQI_GUI_Question
 * purpose     : Builtin predicate "Question"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Comm_Question(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Question","need at least 3 inputs"));	
		
	SQI_Keyword *type = IsKeyword((*args)[0]);
	
	if(type)
	{
		char *b[3];
		b[0] = NULL;
		b[1] = NULL;
		b[2] = NULL;
		int atype;
		long result;
		
		SQI_Object *button = (*args)[1];
		if(button->Is()==SQI_LIST)
		{
			SQI_List *alist = IsList(button);
			if(!alist->Length() || 	alist->Length()>3)
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Question","when using a list of button , the size of the list must be 1, 2 or 3"));	
			else
			{
				list<SQI_Object *>::const_iterator *j = alist->Iterator();
			    list<SQI_Object *>::const_iterator *e = alist->End();
				string *str;
							      
			    for(int i=0;*j!=*e;(*j)++,i++)
			    {
					str = (*(*j))->Print();	
					b[i] = strdup(str->c_str());
					delete str;	
				}
				
				delete j;
				delete e;
			}
		}
		else
		{
			string *str = button->Print();
			b[0] = strdup(str->c_str());
			delete str;
		}	
		
		if(*(type->Data())=="info")
			atype = B_INFO_ALERT;
		else
		if(*(type->Data())=="idea")
			atype = B_IDEA_ALERT;		
		else
		if(*(type->Data())=="warning")
			atype = B_WARNING_ALERT;			
		else
		if(*(type->Data())=="stop")
			atype = B_STOP_ALERT;
		else
		if(*(type->Data())=="bug")
			atype = 5;
		else
			atype = B_EMPTY_ALERT;		
		
		// now we concat all the other input in a string
		
		string *title;
		string txt;
		string *input;	
		
		title = ((*args)[2])->Print();		
		
		for(int i=3;i<args->Length();i++)
		{
			input = ((*args)[i])->Print();
			txt += *input;
			txt += " ";
			delete input;
		}	
		
		//BAlert *alert = new BAlert(NULL,txt.c_str(),b[0],b[1],b[2],B_WIDTH_FROM_WIDEST,atype);
		//result = alert->Go();
		kiAlert *alert = new kiAlert(Icons[atype],title->c_str(),txt.c_str(),b[0],b[1],b[2]);
		result = alert->Go(NULL);		
			
		delete title;	
		
		for(int i=0;i<3;i++)
			if(b[i])
				free(b[i]);	
					
		return new SQI_Number(squirrel->LocalHeap,static_cast<int>(result+1));		
	}
	else
	{
		SQI_Image *icon = IsImage((*args)[0]);
		if(icon)
		{
			char *b[3];
			b[0] = NULL;
			b[1] = NULL;
			b[2] = NULL;
			alert_type atype;
			long result;
			
			SQI_Object *button = (*args)[1];
			if(button->Is()==SQI_LIST)
			{
				SQI_List *alist = IsList(button);
				if(!alist->Length() || 	alist->Length()>3)
					throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Question","when using a list of button , the size of the list must be 1, 2 or 3"));	
				else
				{
					list<SQI_Object *>::const_iterator *j = alist->Iterator();
				    list<SQI_Object *>::const_iterator *e = alist->End();
					string *str;
								      
				    for(int i=0;*j!=*e;(*j)++,i++)
				    {
						str = (*(*j))->Print();	
						b[i] = strdup(str->c_str());
						delete str;	
					}
					
					delete j;
					delete e;
				}
			}
			else
			{
				string *str = button->Print();
				b[0] = strdup(str->c_str());
				delete str;
			}
				
			// now we concat all the other input in a string
			
			string txt;
			string *input;
			string *title;
			
			title = ((*args)[2])->Print();	
			
			for(int i=3;i<args->Length();i++)
			{
				input = ((*args)[i])->Print();
				txt += *input;
				txt += " ";
				delete input;
			}	
			
			kiAlert *alert = new kiAlert(icon->GetBitmap(),title->c_str(),txt.c_str(),b[0],b[1],b[2]);
			result = alert->Go(NULL);
			
			delete title;	
			
			for(int i=0;i<3;i++)
				if(b[i])
					free(b[i]);	
						
			return new SQI_Number(squirrel->LocalHeap,static_cast<int>(result+1));			
						
		}	
		else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Question","the alert type must be a word or an image"));
	}		
}

/*
 * function    : SQI_GUI_Info
 * purpose     : Builtin predicate "Info"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */
SQI_Object *SQI_Comm_Info(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<1)
		throw(new SQI_Exception(SQI_EX_BADARGSNUM,"Info","need at least 3 inputs"));	
		
	SQI_Image *type = IsImage((*args)[0]);
		
	if(type)
	{		
		// now we concat all the other input in a string
		string txt;
		string *input;	
		
		// button
		SQI_Object *button = (*args)[1];
		string *str = button->Print();	
		
		// title
		SQI_String *title = IsString((*args)[2]);
		if(!title)
			title = IsKeyword((*args)[2]);
		if(!title)
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Info","third input must be a word/string"));				
		
		for(int i=3;i<args->Length();i++)
		{
			input = ((*args)[i])->Print();
			txt += *input;
			txt += " ";
			delete input;
		}	
		
		kiAlert *alert = new kiAlert(type->GetBitmap(),title->Data()->c_str(),txt.c_str(),str->c_str());
		alert->Go();
				
		delete str;
		
		return NULL;		
	}
	else
	{
		SQI_Keyword *word = IsKeyword((*args)[0]);
		if(word)
		{
			int atype;
			
			if(*(word->Data())=="info")
				atype = B_INFO_ALERT;
			else
			if(*(word->Data())=="idea")
				atype = B_IDEA_ALERT;		
			else
			if(*(word->Data())=="warning")
				atype = B_WARNING_ALERT;			
			else
			if(*(word->Data())=="stop")
				atype = B_STOP_ALERT;
			else
			if(*(word->Data())=="bug")
				atype = 5;
			else				
				atype = B_EMPTY_ALERT;
				
			// now we concat all the other input in a string
			string txt;
			string *input;	
			
			// button
			SQI_Object *button = (*args)[1];
			string *str = button->Print();	
			
			// title
			SQI_String *title = IsString((*args)[2]);
			if(!title)
				title = IsKeyword((*args)[2]);
			if(!title)
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Info","third input must be a word/string"));				
			
			for(int i=3;i<args->Length();i++)
			{
				input = ((*args)[i])->Print();
				txt += *input;
				txt += " ";
				delete input;
			}	
			
			
			//BAlert *alert = new BAlert(NULL,txt.c_str(),str->c_str(),NULL,NULL,B_WIDTH_FROM_WIDEST,atype);
			//alert->Go();
			
			kiAlert *alert = new kiAlert(Icons[atype],title->Data()->c_str(),txt.c_str(),str->c_str());
			alert->Go();	
					
			delete str;
			
			return NULL;												
		}
		else	
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"Info","first input must be an image or a word"));		
	}
}
	
/*
 * function    : SQI_Comm_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Comm_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("print");
  Map->RemoveFunc("show");
  Map->RemoveFunc("type");
  Map->RemoveFunc("read.integer");
  Map->RemoveFunc("read.float");
  Map->RemoveFunc("read.number");
  Map->RemoveFunc("read.string");
  Map->RemoveFunc("read.list");
  Map->RemoveFunc("read.word");
  Map->RemoveFunc("read.block");
  Map->RemoveFunc("read.anything");
  Map->RemoveFunc("parse.integer");
  Map->RemoveFunc("parse.float");
  Map->RemoveFunc("parse.number");
  Map->RemoveFunc("parse.string");
  Map->RemoveFunc("parse.list");
  Map->RemoveFunc("parse.word");
  Map->RemoveFunc("parse.block");
  Map->RemoveFunc("parse.anything");
  Map->RemoveFunc("load");
  Map->RemoveFunc("Precision");
  Map->RemoveFunc("Message");
  Map->RemoveFunc("Info");
  Map->RemoveFunc("Question");
   
  DelStdIcon();
}

/*
 * function    : SQI_Comm_Init
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
	GetStdIcon();	
	
  Map->AddAddOn(addon_id,Comm_Pkg_name,Comm_Pkg_purpose,Comm_Pkg_author,Comm_Pkg_version,Comm_Pkg_date,SQI_Comm_Remove);

  Map->AddFunc("print",
	       Comm_Pkg_name,
	       "Print the input to the output stream",
	       "print a_thing another_thing ....",
	       SQI_Comm_Print
	       );

  Map->AddFunc("show",
	       Comm_Pkg_name,
	       "Print the input to the output stream (list are printed with [])",
	       "show a_thing another_thing ....",
	       SQI_Comm_Show
	       );

  Map->AddFunc("type",
	       Comm_Pkg_name,
	       "Print the input to the output stream (like print but without space and no newline)",
	       "type a_thing another_thing ....",
	       SQI_Comm_Type
	       );
	       
  Map->AddFunc("read.integer",
	       Comm_Pkg_name,
	       "Read an integer from the input stream and output a new integer object with this value",
	       "read.integer",
	       SQI_Comm_ReadInteger
	       );
	       
	Map->AddFunc("read.float",
	       Comm_Pkg_name,
	       "Read a float from the input stream and output a new float object with this value",
	       "read.float",
	       SQI_Comm_ReadFloat
	       );	       
	       	       
	Map->AddFunc("read.number",
	       Comm_Pkg_name,
	       "Read a number from the input stream and output a new number object with this value",
	       "read.number",
	       SQI_Comm_ReadNumber
	       );
	       
	Map->AddFunc("read.string",
	       Comm_Pkg_name,
	       "Read a string from the input stream and output a new string object with this value",
	       "read.string",
	       SQI_Comm_ReadString
	       );
	       
	Map->AddFunc("read.list",
	       Comm_Pkg_name,
	       "Read a list from the input stream and output a new list object with this value",
	       "read.list",
	       SQI_Comm_ReadList
	       );
	       
	Map->AddFunc("read.word",
	       Comm_Pkg_name,
	       "Read a word from the input stream and output a new word object with this value",
	       "read.word",
	       SQI_Comm_ReadWord
	       );
	       
	Map->AddFunc("read.block",
	       Comm_Pkg_name,
	       "Read a block from the input stream and output a new block object with this value",
	       "read.block",
	       SQI_Comm_ReadBlock
	       );
	       
	Map->AddFunc("read.anything",
	       Comm_Pkg_name,
	       "Read a anything from the input stream and output a new object with this value",
	       "read.anything",
	       SQI_Comm_ReadAnything
	       );
	       
	Map->AddFunc("parse.integer",
	       Comm_Pkg_name,
	       "Parse a string in input  as an integer and output the object",
	       "parse.integer string",
	       SQI_Comm_ParseInteger
	       );
	       
	Map->AddFunc("parse.float",
	       Comm_Pkg_name,
	       "Parse a string in input as a float and output the object",
	       "parse.float string",
	       SQI_Comm_ParseFloat
	       );
	       
	Map->AddFunc("parse.number",
	       Comm_Pkg_name,
	       "Parse a string in input as a number and output the object",
	       "parse.number string",
	       SQI_Comm_ParseNumber
	       );	       
	      
	 Map->AddFunc("parse.string",
	       Comm_Pkg_name,
	       "Parse a string in input as a string and output the object",
	       "parse.string string",
	       SQI_Comm_ParseString
	       );

	 Map->AddFunc("parse.list",
	       Comm_Pkg_name,
	       "Parse a string in input as a list and output the object",
	       "parse.list string",
	       SQI_Comm_ParseList
	       );	       
	      
	 Map->AddFunc("parse.word",
	       Comm_Pkg_name,
	       "Parse a string in input as a word and output the object",
	       "parse.word string",
	       SQI_Comm_ParseWord
	       );	      
	       
	 Map->AddFunc("parse.block",
	       Comm_Pkg_name,
	       "Parse a string in input as a block and output the object",
	       "parse.block string",
	       SQI_Comm_ParseBlock
	       );
	       
	 Map->AddFunc("parse.anything",
	       Comm_Pkg_name,
	       "Parse a string in input as a anykind of object and output it",
	       "parse.anything string",
	       SQI_Comm_ParseAnything
	       ); 
	       
	 Map->AddFunc("load",
	       Comm_Pkg_name,
	       "load all the input file name",
	       "load string1 string2 string3",
	       SQI_Comm_Load
	       ); 
	       
	 Map->AddFunc("Precision",
	       Comm_Pkg_name,
	       "Set or get the float precision",
	       "Precision (number)",
	       SQI_Comm_Precision
	       ); 
	       
	 Map->AddFunc("Message",
	       Comm_Pkg_name,
	       "Create a new message object",
	       "Message number",
	       SQI_Comm_Message
	       ); 
	       
	Map->AddFunc("Question",
	       Comm_Pkg_name,
	       "Display an Question window. The output is the index of the button clicked by the user",
	       "Question type [buttons_label] text1 text2 text3 text4 ....",
	       SQI_Comm_Question
	       );
	       
	Map->AddFunc("Info",
	       Comm_Pkg_name,
	       "Display an Info window.",
	       "Info image button_label text1 text2 text3 text4 ....",
	       SQI_Comm_Info
	       );	      
}


