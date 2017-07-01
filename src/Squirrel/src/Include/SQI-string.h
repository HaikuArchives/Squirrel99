/*
 * Squirrel project
 *
 * file ?	SQI-string.h
 * what	?       String class definition
 * who	?	jlv
 * when	?	06/22/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_String
 * Purpose : Class String 
 *
 */
class SQI_String : public SQI_Object {
 public:
  SQI_String(SQI_Heap *h,char *value);
  SQI_String(SQI_Heap *h,string value);
  SQI_String(SQI_Heap *h,SQI_String *str);
  virtual status_t Archive(BMessage *archive);
  virtual status_t Instantiate(SQI_Heap *target,BMessage *archive); 
  virtual string *Print(int prec = 3);
  virtual string *Show(int prec = 3);
  virtual string *Dump();
  int Length() {return data.length();};
  string *Data() {return &data;};
  virtual SQI_Object *operator=(SQI_Object *op2);
  virtual SQI_Object *operator+(SQI_Object *op2);
  virtual SQI_Object *operator-(SQI_Object *op2);
  virtual SQI_Object *operator*(SQI_Object *op2);
  virtual SQI_Object *operator/(SQI_Object *op2);
  virtual SQI_Object *operator%(SQI_Object *op2);
  virtual bool operator==(SQI_Object *op2);
  virtual bool operator!=(SQI_Object *op2);
  virtual bool operator>(SQI_Object *op2);
  virtual bool operator<(SQI_Object *op2);
  virtual bool operator>=(SQI_Object *op2);
  virtual bool operator<=(SQI_Object *op2);
  char &operator[](int i) {
    if((i<data.length())&&(i>=0))
      return data[i];
  }
  virtual SQI_Object *Clone(SQI_Heap *target = NULL);
  virtual SQI_Object *DeepClone(SQI_Heap *target = NULL);
 protected:
  string data;
};

SQI_Object *InstantiateAString(SQI_Heap *target,BMessage *archive);
