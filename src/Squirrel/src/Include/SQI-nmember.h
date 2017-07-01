/*
 * Squirrel project
 *
 * file ?	SQI-nmember.h
 * what	?	SQI_nVarref class object
 * who	?	jlv
 * when	?	10/02/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_nMember
 * Purpose : Builtin Node class 
 *
 */
class SQI_nMember : public SQI_Node {
 public: 
  SQI_nMember(SQI_Heap *h,SQI_Keyword *obj);
  virtual bool Suicidal(bool force=false);
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL); // import the object to another heap
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  string *Name() {return ref->Data();};
  string *GetMember() {return &Member;};
  void SetMember(string *member) {Member = member->c_str();};
  void SetMember(char *member) {Member = member;};
 private:
  SQI_Keyword *ref;
  string Member;
};
