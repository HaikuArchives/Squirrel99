/*
 * Squirrel project
 *
 * file ?	SQI-thepointer.h
 * what	?   Message class definition
 * who	?	jlv
 * when	?	03/09/00
 * last	?	03/23/01
 *
 *
 * (c) Kirilla 2000-2001
 */

#include <Message.h>
 
#define SQI_MESSAGE			31
#define CLASS_MESSAGE		8 

class SQI_Message; //forward def

//typedef map<string,SQI_Object *(SQI_Message::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > MMemberMap;

/*
 * Class   : SQI_Message
 * Purpose : Class Message 
 *
 */
class SQI_Message : public SQI_ThePointer {
 public:
  SQI_Message(SQI_Squirrel *squirrel,const BMessage *message);
  SQI_Message(SQI_Heap *target,const BMessage *message);
  ~SQI_Message();
  //status_t Archive(BMessage *archive);
  //status_t Instantiate(SQI_Heap *target,BMessage *archive);
  bool Suicidal(bool force=false);
  string *Print(int prec=3);
  string *Show(int prec=3);
  string *Dump();
  void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Clone(SQI_Heap *target = NULL);
  SQI_Object *DeepClone(SQI_Heap *target = NULL);
  // methods
  SQI_Object *Add(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Find(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Has(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Rem(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Replace(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Reply(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Send(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Names(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *What(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *IsEmpty(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Empty(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Delivered(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *IsRemote(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *IsWaiting(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *IsReply(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *Previous(SQI_Squirrel *squirrel,SQI_Args *inputs);
  SQI_Object *TimeOut(SQI_Squirrel *squirrel,SQI_Args *inputs);
 private:
 	BMessage *Message;
 	bigtime_t tsend;
 	bigtime_t treply;
};

void InitMessage();
void UnInitMessage();
SQI_Message *IsMessage(SQI_Object *object);
