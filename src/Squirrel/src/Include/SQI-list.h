/*
 * Squirrel project
 *
 * file ?	SQI-list.h
 * what	?       List class definition
 * who	?	jlv
 * when	?	07/16/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

/*
 * Class   : SQI_List
 * Purpose : List String 
 *
 */
class SQI_List : public SQI_Object 
{
 public:
  SQI_List(SQI_Heap *h);
  SQI_List(SQI_Heap *target,BMessage *archive);
  ~SQI_List();
  status_t Archive(BMessage *archive);
  status_t Instantiate(SQI_Heap *target,BMessage *archive);
  virtual bool Suicidal(bool force=false);
  virtual string *Print(int prec = 3);
  virtual string *Show(int prec = 3);
  virtual string *Dump();
  virtual void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
  int Length() {return data->size();};
  list<SQI_Object *> *Data() {return data;};
  bool IsComplex() {return complex;};
  void Complex() {complex = true;};
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
  SQI_Object *operator[](int i);
  void Add2Begin(SQI_Object *obj);
  void Add2End(SQI_Object *obj);
  void AddAt(SQI_Object *obj,int32 index);
  list<SQI_Object *>::const_iterator *Iterator()
    {
      list<SQI_Object *>::const_iterator *i = new list<SQI_Object *>::const_iterator;
      *i = data->begin();
      return i;
    }
  list<SQI_Object *>::const_iterator *End()
    {
      list<SQI_Object *>::const_iterator *i = new list<SQI_Object *>::const_iterator;
      *i = data->end();
      return i;
    }
  virtual SQI_Object *Clone(SQI_Heap *target = NULL);
  virtual SQI_Object *DeepClone(SQI_Heap *target = NULL);
  SQI_Number *Max(); // Return the maximum value of all the number element in the list
  SQI_Number *Min(); // Return the minimum value of all the number element in the list
  SQI_Number *Sum(long *num); // Return the sum of all the number element in the list
  SQI_Number *Product(); // Return the product of all the number element in the list
  bool Find(SQI_Object *object,int *position = NULL);
 protected:
  list<SQI_Object *> *data;
  bool complex;	// indicate if the list is a complex or not 
};

SQI_Object *InstantiateAList(SQI_Heap *target,BMessage *archive);
