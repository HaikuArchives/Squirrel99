/*
 * Squirrel project
 *
 * file ?	SQI-exception.h
 * what	?	Exception class definition
 * who	?	jlv
 * when	?	07/18/99
 * last	?	02/07/00
 *
 *
 * (c) electron Technology 1999
 */

#define SQI_EX_UNKNOW       0
#define SQI_EX_BADTYPE      1
#define SQI_EX_DIVIDEBYZERO 2
#define SQI_EX_HEAP         3
#define SQI_EX_INVALIDE     4
#define SQI_EX_UNKNOWVAR    5
#define SQI_EX_UNKNOWFUNC   6
#define SQI_EX_BADARGSNUM   7
#define SQI_EX_BADARGTYPE   8
#define SQI_EX_BADVALUE     9
#define SQI_EX_UNKNOWTAG    10
#define SQI_EX_OUTOFRANGE   11
#define SQI_EX_USERDEFINE   12
#define SQI_EX_DONTKNOW     13
#define SQI_EX_OUTOFMEMORY  14
#define SQI_EX_NOVALUE      15
#define SQI_EX_TYPEMISMATCH 16
#define SQI_EX_ERROR		17

class SQI_Exception {
 public:
  SQI_Exception(char type,const char *src,const char *detail,const char *extra = NULL);
  SQI_Exception(char type,const char *src,const string *detail,const char *extra = NULL);
  SQI_Exception(char type);
  SQI_Exception(char type,const char *src);
  SQI_Exception(const char *tag,SQI_Object *detail);
  ~SQI_Exception();
  void Add(char *src);
  string *What();
  char Type() {return what;};
  string *Detail() {return adetail;};
  SQI_Object *UDetail() {return udetail;};
  const string *Origin();
  const string *Next();
  string *Print();
 private:
  char what;
  string *adetail;
  SQI_Object *udetail;
  list<string> *where;
  list<string>::const_iterator next;
};

