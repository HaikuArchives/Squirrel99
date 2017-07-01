/*
 * Squirrel project
 *
 * file ?	SQI-nindex.h
 * what	?	SQI_nIndex class object
 * who	?	jlv
 * when	?	10/18/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_nIndex
 * Purpose : Index node class 
 *
 */
class SQI_nIndex : public SQI_Node {
 public: 
  SQI_nIndex(SQI_Heap *h,char *ref,list<SQI_Object *> *obj);
  ~SQI_nIndex();
  virtual bool Suicidal(bool force=false);
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *GetName() {return &Name;};
  void GetIndex(SQI_Object **pindex,list<SQI_Object *> **plindex);
 private:
  string Name;
  list<SQI_Object *> *lindex;
  SQI_Object *index;
};
