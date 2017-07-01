/*
 * Squirrel project
 *
 * file ?	SQI-ncontrol.h
 * what	?	SQI_nControl class object
 * who	?	jlv
 * when	?	07/20/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#define SQI_CONTROL_RUN       0
#define SQI_CONTROL_RUNRESULT 1
#define SQI_CONTROL_REPEAT    2
#define SQI_CONTROL_IF        3
#define SQI_CONTROL_IFELSE    4
#define SQI_CONTROL_IFTRUE    5
#define SQI_CONTROL_IFFALSE   6
#define SQI_CONTROL_CATCH     7
#define SQI_CONTROL_FOR       8
#define SQI_CONTROL_DOWHILE   9
#define SQI_CONTROL_WHILE     10
#define SQI_CONTROL_DOUNTIL   11
#define SQI_CONTROL_UNTIL     12
#define SQI_CONTROL_FOREACH   13
#define SQI_CONTROL_MAKE	  14
#define SQI_CONTROL_SWITCH	  15	


/*
 * Class   : SQI_nControl
 * Purpose : Builtin Node class 
 *
 */
class SQI_nControl : public SQI_Node {
 public: 
  SQI_nControl(SQI_Heap *h,char type);
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  char IsA() {return stype;};
 private:
  char stype;                       // Control sub type
};

