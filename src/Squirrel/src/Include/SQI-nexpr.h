/*
 * Squirrel project
 *
 * file ?	SQI-nexpr.h
 * what	?	SQI_nExpr class object
 * who	?	jlv
 * when	?	07/23/99
 * last	?	03/29/01
 *
 *
 * (c) Kirilla 1999-2001
 */

//38
#define SQI_EXPR_ADD 39
#define SQI_EXPR_SUB 40
#define SQI_EXPR_MUL 41
#define SQI_EXPR_IDIV 42
#define SQI_EXPR_DIV 43
#define SQI_EXPR_MOD 44
#define SQI_EXPR_EQ  45
#define SQI_EXPR_NEQ 46
#define SQI_EXPR_GT  47
#define SQI_EXPR_GTE 48
#define SQI_EXPR_LT  49
#define SQI_EXPR_LTE 50
#define SQI_EXPR_AND 51
#define SQI_EXPR_OR  52
#define SQI_EXPR_NOT 53
#define SQI_EXPR_POW 54
#define SQI_EXPR_BAND 55
#define SQI_EXPR_BIOR 56
#define SQI_EXPR_BEOR 57
#define SQI_EXPR_BLSH 58
#define SQI_EXPR_BRSH 59
#define SQI_EXPR_NEG 60


/*
 * Class   : SQI_nExpr
 * Purpose : Builtin Node class 
 *
 */
class SQI_nExpr : public SQI_Node {
 public: 
  SQI_nExpr(SQI_Heap *h,char type,SQI_Object *obj);
  virtual string *Print(int prec = 3);
  virtual string *Dump();
  char IsA() {return stype;};
 private:
  char stype;                    // expression type
};

