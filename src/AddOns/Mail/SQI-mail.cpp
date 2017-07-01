/*
 * Squirrel project
 *
 * file ?	SQI-mail.cpp
 * what	?	Mail builtin predicates
 * who	?	jlv
 * when	?	10/03/99
 * last	?	03/25/01
 *
 *
 * (c) Kirilla 2000-2001
 */

// First the needed header

#include "SQI-AddOn.h"
#include "Mail.h"

#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <Query.h>
#include <Node.h>
#include <Volume.h>
#include <VolumeRoster.h>
#include <FindDirectory.h>

// now some infos on the package

char *Mail_Pkg_name = "Mail";
char *Mail_Pkg_purpose = "Mail builtins";
char *Mail_Pkg_author = "Kirilla";
char *Mail_Pkg_version = "0.4";
char *Mail_Pkg_date = "03/25/01";

BDirectory *InFolder = NULL;


/*
 * function    : SQI_Mail_Mail
 * purpose     : Builtin predicate "Mail"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Mail_Mail(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  return NewMailObject(squirrel);
}

/*
 * function    : SQI_Mail_Send
 * purpose     : Builtin predicate "Send"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Mail_Send(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	if(args->Length()<1)
    {
    	send_queued_mail();
    	return interp->True;	
    }
    
    MailObject *mail = IsMail((*args)[0]);
    if(!mail)
   		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"mail.send","first input must be a Mail object"));
   	else
   	{
   		BMailMessage *msg;
   		bool flag = true;
   		
   		if(args->Length()==2)
   		{
   			SQI_Keyword *word = IsKeyword((*args)[1]);
   			
   			if(word)
   			{
   				string *str = word->Data();
   				
   				if(*str == "now")
   					flag = true;
   				else
   				if(*str == "later")
   					flag = false;
   				else 
   					throw(new SQI_Exception(SQI_EX_INVALIDE,"mail.send","second input must be the word : now or later"));			
   			}
   			else
   				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"mail.send","second input must be the word : now or later"));			
   		}	
   		
   		msg = mail->Build();
   		 		
   		if(msg)
   		{		   		
	   		if(msg->Send(flag) == B_OK)
	   			return interp->True;
	   		else
	   			return interp->False;	
   		}
   		else
   			return interp->False;	
   	}
}

/*
 * function    : SQI_Mail_Recent
 * purpose     : Builtin predicate "mail.recent"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Mail_Recent(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
	BEntry entry;
	BNode node;
	int32 messages = 0;
	static char buffer[256];
	bool found = false;	
	
	if(!InFolder)
	{
		// get the "in" directory for the mail
		BPath path;
		if(find_directory(B_USER_DIRECTORY, &path) != B_OK)
			throw(new SQI_Exception(SQI_EX_ERROR,"mail.recent","cannot find home directory"));	
		
		path.Append("mail/in");
		
		InFolder = new BDirectory(path.Path());
			
		if(InFolder->InitCheck() != B_OK)
			throw(new SQI_Exception(SQI_EX_ERROR,"mail.recent","enable to access mail directory"));			
	}	
	
	while(InFolder->GetNextEntry(&entry) != B_ENTRY_NOT_FOUND)
	{
		// seek the next NEW email
		node.SetTo(&entry);
		node.ReadAttr(B_MAIL_ATTR_STATUS,B_STRING_TYPE,0,buffer,256);
		if(buffer[0] == 'N')
		{
			found = true;
			break;	
		}
	}
	
	if(found)
	{
		// mark as read
		//node.WriteAttr(B_MAIL_ATTR_STATUS,B_STRING_TYPE,0,"Read",4);
		return new MailObject(squirrel,&node,&entry);
	}	
	else
		return interp->False;
	
	/*
	BQuery query;
	BNode node;
	BVolume vol;
	BVolumeRoster vroster;
	BEntry entry;
	BPath path;
	entry_ref ref;
	char buf[256];
	int32 message_count = 0;
	
	vroster.GetBootVolume(&vol);
	query.SetVolume(&vol);
	
	if(query.SetPredicate("MAIL:status = New") != B_OK)
		throw(new SQI_Exception(SQI_EX_ERROR,"mail.recent","internal error :("));				
	
	if(query.Fetch() != B_OK)
		throw(new SQI_Exception(SQI_EX_ERROR,"mail.recent","internal error :("));
		
	while (query.GetNextEntry(&entry) == B_OK)
	{
		// only the one in the /Mail/in folder	
		entry.GetPath(&path);
		cout << path.Path() << "\n";	
		
		message_count++;	
	}	
	
	cout << message_count << "new emails\n";
		
	return NULL;
	
	*/	
}

/*
 * function    : SQI_Mail_IsMail
 * purpose     : Builtin predicate "is.mail"
 * input       : 
 *
 * deque<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Mail_IsMail(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  if(!args->Length())
    throw(new SQI_Exception(SQI_EX_BADARGSNUM,"is.mail","need 1 argument"));

  SQI_ThePointer *ptr = IsThePointer((*args)[0]);

  if(ptr)
  {
  	if(IsMail(ptr))
    	return interp->True;
    else
    	return interp->False;
  } 
  else
    return interp->False;
}

/*
 * function    : SQI_Mail_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Mail_Remove(SQI_BuiltInMap *Map)
{
	UninitMailObject();		
	
  	Map->RemoveFunc("mail.new");
  	Map->RemoveFunc("mail.send");
  	Map->RemoveFunc("is.mail");
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
  	Map->AddAddOn(addon_id,Mail_Pkg_name,Mail_Pkg_purpose,Mail_Pkg_author,Mail_Pkg_version,Mail_Pkg_date,SQI_Mail_Remove);
  
  	Map->AddFunc("mail.new",
	       Mail_Pkg_name,
	       "Output a new Mail object",
	       "mail.new",
	       SQI_Mail_Mail
	       );
	       
	Map->AddFunc("mail.send",
	       Mail_Pkg_name,
	       "Send an email or send all the queued mail",
	       "mail.send (mail (word))",
	       SQI_Mail_Send
	       );
	       
	Map->AddFunc("mail.recent",
	       Mail_Pkg_name,
	       "Get the last unread message",
	       "mail.recent",
	       SQI_Mail_Recent
	       );	       
	       
	Map->AddFunc("is.mail",
	       Mail_Pkg_name,
	       "Output true if the input is a mail",
	       "is.mail thing",
	       SQI_Mail_IsMail
	       );		       
}
