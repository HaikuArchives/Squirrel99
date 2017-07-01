/*
 * Squirrel project
 *
 * file ?	SQI-nbuiltin.h
 * what	?	SQI_nBuiltin class object
 * who	?	jlv
 * when	?	07/20/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

class SQI_Squirrel;
class SQI_Interp;

/*
 * Class   : SQI_nBuiltin
 * Purpose : Builtin Node class 
 *
 */
class SQI_nBuiltin : public SQI_Node {
 public: 
  SQI_nBuiltin(SQI_Heap *h,char *str);
  ~SQI_nBuiltin();
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *cmd; // func name (temp)
  SQI_Object *(*function)(SQI_Args *,SQI_Squirrel *,SQI_Interp *); // pointer to the function
};

