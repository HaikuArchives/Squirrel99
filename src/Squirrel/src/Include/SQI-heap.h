/*
 * Squirrel project
 *
 * file ?	SQI-heap.h
 * what	?	heap class definition
 * who	?	jlv
 * when	?	07/02/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 2000-2001
 */
 
#include <Locker.h> 

/*
 * Class   : SQI_Heap
 * Purpose : Class Heap 
 *
 */
class SQI_Heap : public BLocker {
 public:
  SQI_Heap(char otype = SQI_NORMAL);
  ~SQI_Heap();
  long GC();                                         // Performe a Garbage collection
  /*
  SQI_Number *Number(bool value);                    // Create a new integer
  SQI_Number *Number(int value);                     // Create a new integer
  SQI_Number *Number(long value);                    // Create a new long 
  SQI_Number *Number(float value);                   // Create a new long 
  SQI_Number *Number(double value);                  // Create a new long
  SQI_String *String(char *value);                   // Create a new string
  SQI_String *String(string value);                   // Create a new string
  SQI_String *String(SQI_String *value);             // Create a new string
  SQI_Keyword *Keyword(char *value);                 // Create a new keyword
  SQI_Keyword *Keyword(string *value);                 // Create a new keyword
  SQI_Keyword *Keyword(SQI_Keyword *value);          // Create a new keyword
  SQI_List *List();                                  // Create a new list
  SQI_nBlock *Block();                               // Create a new block
  SQI_nBuiltin *Builtin(char *str);                  // Create a new node Builtin
  SQI_nExpr *Expression(char type,SQI_Object *obj);  // Create a new node Expr
  SQI_nUfunc *Ufunc(string *name);                   // Create a new node Ufunc with only the function name
  SQI_nUnknow *Unknow(string *name);                 // Create a new node Unknow with only the function name
  SQI_nUfunc *Ufunc(string *name,SQI_Defunc *ptr);   // Create a new node Ufunc with the function ptr and name
  SQI_nControl *Control(char type);                  // Create a new node Control
  SQI_nVarref *Varref(SQI_Object *obj);              // Create a new node Varref
  SQI_nTag *Tag(SQI_String *obj);                    // Create a new node Tag
  SQI_nGoto *Goto(SQI_Object *obj);                  // Create a new node Goto
  SQI_nSwitch *Switch(SQI_Object *obj);                  // Create a new node Switch
  void ThePointer(SQI_ThePointer *ptr);				 // Create a new ThePointer
  SQI_nMember *Member(SQI_Object *obj,char *member); // Create a new node Member
  */
  void Import(SQI_Object *obj);                      // Import an object from another heap
  void Free(SQI_Object *obj);                        // Delete an object
  void Display();                                    // Print the content of the heap on stdout
  string *Dump();                                      // Dump all the object in the heap
  string *Stats();                                     // Return a string which contain stats info on the heap
  SQI_Object *Top() {return top;};                   // Return a pointer on the first element of the heap
  long FootPrint();                                  // Return the memory used by the heap
  long Size() {return size;}						 // Return the size of the heap
  long AddObject(SQI_Object *last);                  // Insert an object in the list
 private:
  long size;                                         // current size of the heap
  long counter;                                      // last ID used for an object
  char status;                                       // status of the heap (eternal , normal)
  SQI_Object *top;                                   // first element of the heap
  SQI_Object *bottom;                                // last element of the heap
};

