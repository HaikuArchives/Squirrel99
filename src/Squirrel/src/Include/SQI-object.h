/*
 * Squirrel project
 *
 * file ?	SQI-object.h
 * what	?	Objects class definition
 * who	?	jlv
 * when	?	06/22/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <stdlib.h>
#include <deque>

class SQI_Squirrel; // forward definition

#define SQI_BOOL    	0
#define SQI_INT     	1
#define SQI_LONG    	2
#define SQI_FLOAT   	3
#define SQI_DOUBLE  	4
#define SQI_STRING  	5
#define SQI_NUMBER  	6
#define SQI_KEYWORD 	7
#define SQI_LIST    	8
#define SQI_NODE    	9
#define SQI_THEPOINTER 	10

#define SQI_ETERNAL 0
#define SQI_NORMAL  1

class SQI_Heap; // forward def

/*
 * Class   : SQI_Object
 * Purpose : Class Object 
 *
 */
class SQI_Object {
 public:
  // link to the other element of the list and the heap where the object is
  SQI_Heap *heap;
  SQI_Object *previous;
  SQI_Object *next;
  // methods
  SQI_Object(void) {};
  SQI_Object(SQI_Heap *h,char type);
  ~SQI_Object();
  // Archivable methods 
  virtual status_t Archive(BMessage *archive) {return B_ERROR;};
  virtual status_t Instantiate(SQI_Heap *target,BMessage *archive) {return B_ERROR;};
  // other
  virtual bool Suicidal(bool force=false);
  void AddRef() {REF++;};
  void RemRef(bool force=false);
  bool IsEternal() {return !status;};
  char Is() {return TYPE;};
  bool IsAlone() {return (contained==0);};
  void Contained() {contained++;};
  void Alone() {contained--;};
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  virtual SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
  // with other object
  virtual SQI_Object *Inv() {return NULL;};
  virtual SQI_Object *Neg() {return NULL;};
  virtual SQI_Object *Div(SQI_Object *left) {return NULL;};
  virtual SQI_Object *InvDiv(SQI_Object *right) {return NULL;};
  virtual SQI_Object *operator=(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator+(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator-(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator*(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator/(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator%(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *Pow(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator^(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator&(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator|(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator<<(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator>>(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator+=(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator-=(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator*=(SQI_Object *op2) {return NULL;};
  virtual SQI_Object *operator/=(SQI_Object *op2) {return NULL;};
  virtual bool operator==(SQI_Object *op2) {return false;};
  virtual bool operator!=(SQI_Object *op2) {return false;};
  virtual bool operator>(SQI_Object *op2) {return false;};
  virtual bool operator<(SQI_Object *op2) {return false;};
  virtual bool operator>=(SQI_Object *op2) {return false;};
  virtual bool operator<=(SQI_Object *op2) {return false;};
  // with int
  virtual SQI_Object *operator=(int op2) {return NULL;};
  virtual SQI_Object *operator+(int op2) {return NULL;};
  virtual SQI_Object *operator-(int op2) {return NULL;};
  virtual SQI_Object *operator*(int op2) {return NULL;};
  virtual SQI_Object *operator/(int op2) {return NULL;};
  virtual SQI_Object *Pow(int op2) {return NULL;};
  virtual SQI_Object *operator^(int op2) {return NULL;};
  virtual SQI_Object *operator+=(int op2) {return NULL;};
  virtual SQI_Object *operator-=(int op2) {return NULL;};
  virtual SQI_Object *operator*=(int op2) {return NULL;};
  virtual SQI_Object *operator/=(int op2) {return NULL;};
  virtual bool operator==(int op2) {return false;};
  virtual bool operator!=(int op2) {return false;};
  virtual bool operator>(int op2) {return false;};
  virtual bool operator<(int op2) {return false;};
  virtual bool operator>=(int op2) {return false;};
  virtual bool operator<=(int op2) {return false;};
  // with long
  virtual SQI_Object *operator=(long op2) {return NULL;};
  virtual SQI_Object *operator+(long op2) {return NULL;};
  virtual SQI_Object *operator-(long op2) {return NULL;};
  virtual SQI_Object *operator*(long op2) {return NULL;};
  virtual SQI_Object *operator/(long op2) {return NULL;};
  virtual SQI_Object *operator%(long op2) {return NULL;};
  virtual SQI_Object *Pow(long op2) {return NULL;};
  virtual SQI_Object *operator+=(long op2) {return NULL;};
  virtual SQI_Object *operator-=(long op2) {return NULL;};
  virtual SQI_Object *operator*=(long op2) {return NULL;};
  virtual SQI_Object *operator/=(long op2) {return NULL;};
  virtual bool operator==(long op2) {return false;};
  virtual bool operator!=(long op2) {return false;};
  virtual bool operator>(long op2) {return false;};
  virtual bool operator<(long op2) {return false;};
  virtual bool operator>=(long op2) {return false;};
  virtual bool operator<=(long op2) {return false;};
  // with float
  virtual SQI_Object *operator=(float op2) {return NULL;};
  virtual SQI_Object *operator+(float op2) {return NULL;};
  virtual SQI_Object *operator-(float op2) {return NULL;};
  virtual SQI_Object *operator*(float op2) {return NULL;};
  virtual SQI_Object *operator/(float op2) {return NULL;};
  virtual SQI_Object *Pow(float op2) {return NULL;};
  virtual SQI_Object *operator+=(float op2) {return NULL;};
  virtual SQI_Object *operator-=(float op2) {return NULL;};
  virtual SQI_Object *operator*=(float op2) {return NULL;};
  virtual SQI_Object *operator/=(float op2) {return NULL;};
  virtual bool operator==(float op2) {return false;};
  virtual bool operator!=(float op2) {return false;};
  virtual bool operator>(float op2) {return false;};
  virtual bool operator<(float op2) {return false;};
  virtual bool operator>=(float op2) {return false;};
  virtual bool operator<=(float op2) {return false;}; 
  // with double
  virtual SQI_Object *operator=(double op2) {return NULL;};
  virtual SQI_Object *operator+(double op2) {return NULL;};
  virtual SQI_Object *operator-(double op2) {return NULL;};
  virtual SQI_Object *operator*(double op2) {return NULL;};
  virtual SQI_Object *operator/(double op2) {return NULL;};
  virtual SQI_Object *Pow(double op2) {return NULL;};
  virtual SQI_Object *operator+=(double op2) {return NULL;};
  virtual SQI_Object *operator-=(double op2) {return NULL;};
  virtual SQI_Object *operator*=(double op2) {return NULL;};
  virtual SQI_Object *operator/=(double op2) {return NULL;};
  virtual bool operator==(double op2) {return false;};
  virtual bool operator!=(double op2) {return false;};
  virtual bool operator>(double op2) {return false;};
  virtual bool operator<(double op2) {return false;};
  virtual bool operator>=(double op2) {return false;};
  virtual bool operator<=(double op2) {return false;}; 
  // other method
  virtual string *Print(int prec = 3) {return NULL;};                        // return a string version of the value
  virtual string *Show(int prec = 3) {return NULL;};                         // return a string version of the value
  virtual string *Dump() {return NULL;};                                     // return info about the object and the value
  virtual SQI_Object *Clone(SQI_Heap *target = NULL) {return NULL;};       // return a new object, clone of the object
  virtual SQI_Object *DeepClone(SQI_Heap *target = NULL) {return NULL;};   // return a new object, deepclone of the object
  virtual long FootPrint() {return 0;};                        // return the size in byte of the object in memory
  char TYPE;                            // object TYPE
  long ID;                              // object ID
  int REF;                              // number of ref to this objet
  short status;                         // inform if the object is eternal or not
  int contained;						// number of time the object is an element of another object
};





