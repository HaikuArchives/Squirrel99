/*
 * Squirrel project
 *
 * file ?	sqi-locker.cpp
 * what	?   locker object
 * who	?	jlv
 * when	?	02/10/00
 * last	?	02/10/00
 *
 *
 * (c) electron Technology 1999
 */
 
#include "SQI-AddOn.h"
#include "sqi-locker.h"

MemberMap LockerMember;

/*
 * function    : SQI_Locker
 * purpose     : Constructor
 * input       : 
 *
 * SQI_Squirrel *squirrel, running squirrel
 *
 * output      : none
 * side effect : none
 */
SQI_Locker::SQI_Locker(SQI_Squirrel *squirrel)
	:SQI_ThePointer(squirrel,SQI_LOCKER)
{
	SetClass(CLASS_LOCKER);	
	
  	Members = &LockerMember;	
}

/*
 * function    : SQI_Locker
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_Locker::~SQI_Locker()
{
}

// Legacy member functions

/*
 * function    : Suicidal
 * purpose     : The object will commit suicide if no reference to it and not eternal
 * input       : none
 * output      : bool, true if the object had commit suicide
 * side effect : the object  may be deleted
 */
bool SQI_Locker::Suicidal(bool force=false)
{
    if(status || force)
      if(!REF)
      {
			delete this;
			return true;
      }
      
      return false;
}

/*
 * function    : Print
 * purpose     : Create a string from the value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Locker::Print(int prec = 3)
{
  	ostrstream out;
  
	out << "Locker" << '\0';

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
string *SQI_Locker::Show(int prec = 3)
{
  return Print(prec);
}

/*
 * function    : Dump
 * purpose     : Create a string with info/value of the object
 * input       : none
 * output      : string *, a string
 * side effect : none
 */
string *SQI_Locker::Dump()
{ 
	  string *value = this->Print();
	  ostrstream out;
	 
	  out << "LOCKER[" << ID << "][" << REF << "][" << status << "] - " << *value << '\0';
	 
	  delete value;
	  
	  string *ret = new string(out.str());
	  out.freeze(false);
	
	  return ret;
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
SQI_Object *SQI_Locker::Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs)
{	
	if(Members)
	{
		SQI_Object *(SQI_Locker::*MemberHook)(SQI_Squirrel *squirrel,SQI_Args *inputs);		
		
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

// Member methods

/*
 * function    : Lock
 * purpose     : Lock the locker 
 * input       : none
 *
 * SQI_Squirrel *squirrel, squirrel running
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, the object created if any
 * side effect : none
 */
SQI_Object *SQI_Locker::Lock(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	if(lock.Lock())
		return squirrel->interpreter->True;
	else
		return squirrel->interpreter->False;			
}

/*
 * function    : Unlock
 * purpose     : Unlock the locker 
 * input       : none
 *
 * SQI_Squirrel *squirrel, squirrel running
 * SQI_Args *inputs, inputs of the methods
 *
 * output      : SQI_Object *, the object created if any
 * side effect : none
 */
SQI_Object *SQI_Locker::Unlock(SQI_Squirrel *squirrel,SQI_Args *inputs)
{
	lock.Unlock();
	return NULL;			
}


void InitLockerMember()
{
	LockerMember[string("lock")] = &SQI_Locker::Lock;
	LockerMember[string("unlock")] = &SQI_Locker::Unlock;	
}






