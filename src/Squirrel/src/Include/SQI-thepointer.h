/*
 * Squirrel project
 *
 * file ?	SQI-thepointer.h
 * what	?   ThePointer class definition
 * who	?	jlv
 * when	?	10/01/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */
  
class SQI_ThePointer; // forward def
class SQI_Squirrel;   // foward def

#include "MethodTable.h"

/*
 * Class   : SQI_ThePointer
 * Purpose : Class ThePointer 
 *
 */
class SQI_ThePointer : public SQI_Object {
 public:
  SQI_ThePointer(SQI_Squirrel *squirrel,char itype);
  SQI_ThePointer(SQI_Heap *target,char itype);
  virtual status_t Archive(BMessage *archive) {return B_ERROR;};
  virtual status_t Instantiate(SQI_Heap *target,BMessage *archive) {return B_ERROR;};
  virtual bool Suicidal(bool force=false);
  virtual string *Print(int prec=3);
  virtual string *Show(int prec=3);
  virtual string *Dump();
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  virtual SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  virtual SQI_Object *Clone(SQI_Heap *target = NULL);
  virtual SQI_Object *DeepClone(SQI_Heap *target = NULL);
  char IsA() {return stype;};
  char OfClass() {return klass;};
  void SetClass(char c) {klass = c;};
  virtual SQI_Object *Neg();
  virtual SQI_Object *Inv();
  virtual SQI_Object *Div(SQI_Object *left);
  virtual SQI_Object *InvDiv(SQI_Object *right);  
  virtual SQI_Object *operator=(SQI_Object *op2); 
  virtual SQI_Object *operator+(SQI_Object *op2);
  virtual SQI_Object *operator-(SQI_Object *op2);
  virtual SQI_Object *operator*(SQI_Object *op2);
  virtual SQI_Object *operator/(SQI_Object *op2);
  virtual SQI_Object *operator%(SQI_Object *op2);
  virtual SQI_Object *Pow(SQI_Object *op2);
  virtual SQI_Object *operator^(SQI_Object *op2);
  virtual SQI_Object *operator&(SQI_Object *op2);
  virtual SQI_Object *operator|(SQI_Object *op2);
  virtual SQI_Object *operator<<(SQI_Object *op2);
  virtual SQI_Object *operator>>(SQI_Object *op2);
  virtual SQI_Object *operator+=(SQI_Object *op2);
  virtual SQI_Object *operator-=(SQI_Object *op2);
  virtual SQI_Object *operator*=(SQI_Object *op2);
  virtual SQI_Object *operator/=(SQI_Object *op2);
  virtual bool operator==(SQI_Object *op2);
  virtual bool operator!=(SQI_Object *op2);
  virtual bool operator>(SQI_Object *op2);
  virtual bool operator<(SQI_Object *op2);
  virtual bool operator>=(SQI_Object *op2);
  virtual bool operator<=(SQI_Object *op2);
 protected:
  char stype;
  char klass;
  MethodTable *Methods;		 // method hash table 
};
