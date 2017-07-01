/*
 * Squirrel project
 *
 * file ?	SQI-nvarref.h
 * what	?	SQI_nVarref class object
 * who	?	jlv
 * when	?	07/20/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_nVarref
 * Purpose : Builtin Node class 
 *
 */
class SQI_nVarref : public SQI_Node {
 public: 
  SQI_nVarref(SQI_Heap *h,SQI_Keyword *obj);
  virtual bool Suicidal(bool force=false);
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *Name() {return ref->Data();};
 private:
  SQI_Keyword *ref;
};

