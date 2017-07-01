/*
 * Squirrel project
 *
 * file ?	SQI-nufunc.h
 * what	?	SQI_nUfunc class object
 * who	?	jlv
 * when	?	07/20/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_nUfunc
 * Purpose : Builtin Node class 
 *
 */
class SQI_nUfunc : public SQI_Node {
 public: 
  SQI_nUfunc(SQI_Heap *h,string *n);
  SQI_nUfunc(SQI_Heap *h,string *n,SQI_Defunc *f);
  ~SQI_nUfunc();
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *name;
  SQI_Defunc *func;
};

