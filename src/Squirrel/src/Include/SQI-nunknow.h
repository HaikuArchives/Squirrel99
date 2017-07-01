/*
 * Squirrel project
 *
 * file ?	SQI-nunknow.h
 * what	?	SQI_nUnknow class object
 * who	?	jlv
 * when	?	09/28/00
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 2000-2001
 */

class SQI_Squirrel;
class SQI_Interp;


/*
 * Class   : SQI_nUnknow
 * Purpose : Unknow Node class 
 *
 */
class SQI_nUnknow : public SQI_Node {
 public: 
  SQI_nUnknow(SQI_Heap *h,string *n);
  ~SQI_nUnknow();
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *name;
  SQI_Object *(*function)(SQI_Args *,SQI_Squirrel *,SQI_Interp *); // pointer to the function
  SQI_Defunc *func;
};

