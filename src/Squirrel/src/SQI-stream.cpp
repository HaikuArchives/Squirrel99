/*
 * Squirrel project
 *
 * User Interface
 *
 * file ?	SQI-stream.cpp
 * what	?	Stream class 
 * who	?	jlv
 * when	?	08/08/99
 * last	?	02/09/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-stream.h"
#include "SQI-messtream.h"


/*
 * function    : SQI_Stream
 * purpose     : Create a Stream
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Stream::SQI_Stream(BHandler *target)
	:BLocker()
{	
	postoffice = new BMessenger(target,NULL,NULL);
	out = new ostrstream;
	err = new ostrstream;
	
	fin = NULL;
	fout = NULL;
}

/*
 * function    : SQI_Stream
 * purpose     : Create a Stream
 * input       : 
 *
 * FILE *in , input stream
 * FILE *out,
 *
 * output      : none
 * side effect : none
 */
SQI_Stream::SQI_Stream(FILE *sin,FILE *sout)
	:BLocker()
{	
	postoffice = NULL;
	out = new ostrstream;
	err = new ostrstream;
	
	fin = sin;
	fout = sout;		
}

/*
 * function    : SQI_Stream
 * purpose     : Destroy a Stream
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Stream::~SQI_Stream()
{	
	if(postoffice)
		delete postoffice;
		
	out->freeze(false);
	err->freeze(false);	
		
	delete out;
	delete err;
}

/*
 * function    : Flush
 * purpose     : Flush the out, err and send the data
 * input       : none
 * output      : none
 * side effect : none
 */
void SQI_Stream::Flush()
{		
	if(!fin)
	{
		if(out->str())
		{
			BMessage *msg = new BMessage(MSG_STREAM_OUT);
			
			msg->AddString("o",out->str());
				
			postoffice->SendMessage(msg);
			
			out->freeze(false);
			
			delete out;
			out = new ostrstream;	
		}
		else
			out->freeze(false);
		
	
		if(err->str())
		{
			BMessage *msg = new BMessage(MSG_STREAM_ERR);
			
			msg->AddString("e",err->str());
				
			postoffice->SendMessage(msg);
			
			err->freeze(false);
			
			delete err;
			err = new ostrstream;
		}	
		else
			err->freeze(false);		
	}
	else
	{
		if(out->str())
		{
			fprintf(fout,"%s",out->str());
					
			out->freeze(false);
			
			delete out;
			out = new ostrstream;	
		}
		else
			out->freeze(false);
		
		if(err->str())
		{
			fprintf(stderr,"%s",err->str());	
			
			err->freeze(false);
			
			delete err;
			err = new ostrstream;
		}	
		else
			err->freeze(false);			
	}
}

/*
 * function    : Wait
 * purpose     : Wait for data coming from the postoffice and set the in
 *				 with what we get
 *
 * input       : none
 * output      : char *, what we have receive
 * side effect : none
 */
char *SQI_Stream::Wait()
{	
	if(!fin)
	{
		const char *input;
		BMessage *msg = new BMessage(MSG_STREAM_IN);
		BMessage reply;		
		// we send a message to the console
		postoffice->SendMessage(msg,&reply);
		// and wait for the reply
			
		if(reply.what==B_NO_REPLY)
		{
			cout << "no reply\n";
			input = "";			
		}
		else
			if(B_OK!=reply.FindString("input",&input))	
				input = "";	
		
		return strdup(input);
	}
	else
	{
		char *str = (char *)malloc(sizeof(char)*256);	
			
		fgets(str,256,fin);
		
		return str;	
		
		//string s;
		
		//cout << "waiting input\n";
		
		//getline(cin,s);
		
		//cout << s.length() << "\n";
		
		//return (strdup(s.c_str()));		
	}
}


