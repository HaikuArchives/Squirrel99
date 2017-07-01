/*
 * Squirrel project
 *
 * file ?	SQI-keyword.h
 * what	?       Keyword class definition
 * who	?	jlv
 * when	?	07/16/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_Keyword
 * Purpose : Class Keyword 
 *
 */
class SQI_Keyword : public SQI_String {
 public:
  SQI_Keyword(SQI_Heap *h,char *value);
  SQI_Keyword(SQI_Heap *h,string *value);
  SQI_Keyword(SQI_Heap *h,SQI_Keyword *str);
  virtual ~SQI_Keyword() {};
  status_t Archive(BMessage *archive);
  status_t Instantiate(SQI_Heap *target,BMessage *archive); 
  virtual string *Dump();
  virtual SQI_Object *Clone(SQI_Heap *target = NULL);
  string *Show(int prec = 3);
};

SQI_Object *InstantiateAKeyword(SQI_Heap *target,BMessage *archive);
