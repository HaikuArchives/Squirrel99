/*
 * Squirrel project
 *
 * file ?	SQI-squirrel.h
 * what	?	Squirrel class definition
 * who	?	jlv
 * when	?	07/28/99
 * last	?	02/23/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <OS.h> 
#include <Locker.h>

#define SQI_SQ_NOTHING  0
#define SQI_SQ_OUTPUT   1
#define SQI_SQ_STOP     2
#define SQI_SQ_BREAK    3
#define SQI_SQ_CONTINUE 4 
#define SQI_SQ_BYE      5
#define SQI_SQ_TAG      6
#define SQI_SQ_GOTO     7

class SQI_Squirrel {
 public:
  SQI_Squirrel(SQI_Heap *heap = NULL,SQI_VarMap *var = NULL);                      // Constructor
  ~SQI_Squirrel();                     	 										// Destructor
  SQI_Object *Release(SQI_Interp *interp,SQI_nBlock *tree);                       // Release on a tree
  SQI_Args *DerefArgs(SQI_Interp *interp,list<SQI_Object *> *args);// Build a new list with all the args replaced by there value
  deque<SQI_Object *> *BuildDeque(list<SQI_Object *> *args);                  // Build a new deque from the list without copy of the objects
  SQI_Object *DerefList(SQI_Interp *interp,SQI_List *src);                // Replace a list with a derefed list
  SQI_Object *DerefObject(SQI_Interp *interp,SQI_Object *obj);                // Replace an object by is deref value
  SQI_Object *DerefObjectLite(SQI_Interp *interp,SQI_Object *obj);            // Replace an object by is deref value (if not a block)
  bool ExistVar(SQI_Interp *interp,string *var);							  // Test if a variable exists
  SQI_Object *GetVarValue(SQI_Interp *interp,string *var);                     // Return the value of a var (local or global)
  void SetVarValue(SQI_Interp *interp,string *var,SQI_Object *value);              // Return the value of a var (local or global)
  void RemoveVar(SQI_Interp *interp,string *var);                                     // Delete a variable
  TVariable *GetVarBinding(SQI_Interp *interp,string *var);						// Return the TVariable of a variable (local or global)
  bool WaitingVar(SQI_Interp *interp,string *var,thread_id id);				// Add a waiting thread to a variable 
  SQI_Object *HopOnABlock(SQI_Interp *interp,SQI_nBlock *tree);              // Run on a tree
  SQI_Object *HopOnLeave(SQI_Interp *interp,SQI_Node *leave);                // Execute a node
  SQI_Object *HopOnLeaveLite(SQI_Interp *interp,SQI_Node *leave);                // Execute a node (but not the block)
  SQI_Object *HopOnABuiltin(SQI_Interp *interp,SQI_nBuiltin *leave);         // Execute a builtin node
  SQI_Object *HopOnAUfunc(SQI_Interp *interp,SQI_nUfunc *leave,bool reuse = false);         // Execute a ufunc node
  SQI_Object *HopOnAUnknow(SQI_Interp *interp,SQI_nUnknow *leave);         // Execute a unknow node
  SQI_Object *HopOnAVarRef(SQI_Interp *interp,SQI_nVarref *leave);           // Execute a varref node
  SQI_Object *HopOnAControl(SQI_Interp *interp,SQI_nControl *leave);         // Execute a control node
  SQI_Object *HopOnAExpr(SQI_Interp *interp,SQI_nExpr *expr);                 // Compute the value of a expression node
  SQI_Object *HopOnAMember(SQI_Interp *interp,SQI_nMember *leave);            // Execute a member node
  SQI_Object *HopOnAIf(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *block); // Execute a IF control node
  SQI_Object *HopOnAIfElse(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *ok,SQI_nBlock *nok); // Execute a IF control node
  SQI_Object *HopOnAFor(SQI_Interp *interp,SQI_Args *loop,SQI_nBlock *block); // Execute a For control node
  SQI_Object *HopOnADoWhile(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *block); // Execute a dowhile control node
  SQI_Object *HopOnADoUntil(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *block); // Execute a dountil control node
  SQI_Object *HopOnAWhile(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *block); // Execute a while control node
  SQI_Object *HopOnAUntil(SQI_Interp *interp,SQI_Object *expr,SQI_nBlock *block); // Execute a until control node
  SQI_Object *HopOnARun(SQI_Interp *interp,SQI_nBlock *block);   // Execute a run control node
  SQI_Object *HopOnARunResult(SQI_Interp *interp,SQI_nBlock *block);   // Execute a runresult control node
  SQI_Object *HopOnARepeat(SQI_Interp *interp,SQI_Number *num,SQI_nBlock *block); // Execute a repeat node
  SQI_Object *HopOnAForEach(SQI_Interp *interp,SQI_Keyword *var,SQI_List *values,SQI_nBlock *block); // Execute a foreach
  SQI_Object *HopOnACatch(SQI_Interp *interp,SQI_Keyword *tag,SQI_nBlock *block,SQI_nBlock *berr); // Execute a catch node
  SQI_Object *HopOnASwitch(SQI_Interp *interp,SQI_nSwitch *sw);   // Execute a switch control node
  void SameGCConfig(SQI_Squirrel *src);			// set GC parameters the same way than another squirrel 
  void SetGCConfig(bool b,long f,bool o);       // Set GC parameters
  void GetGCConfig(bool *b=NULL,long *f=NULL,bool *o=NULL);       // Get GC parameters 
  void SetPrecision(int8 prec);                       // Set float precision
  void SamePrecision(SQI_Squirrel *src);                       // Set float precision to the same than another squirrel
  void GC();										 // Performe a GC on the LocalHeap if it's time
  long Hop() {return cstep;};         // Return the number of step
  double Elapsed() {return elapsed;};  // Return the elapsed time of the last "Release"
  long Speed();                        // Return the speed of the last "Release"
  void Clean();						   // Clean the heap and the varmap	
  SQI_Heap *LocalHeap;                 // Local Heap
  SQI_VarMap *LocalVar;                // Local Variable Map
  bool gheap;                          // is true if the LocalHeap is the interp globalheap
  bool gvar;                           // is true if the LocalVar is the interp globalvar
  bool test;                           // result of the last evaluation using the builtin "test"
  char flag;                           // flag used by some builtin to inform the squirrel of some special case
  SQI_Object *context;                 // pointer to an object related to the flag
  int8 precision;                      // Float precision
  SQI_Interp *interpreter;             // Pointer to the interpreter
  thread_id thread;					   // thread id where the squirrel is running in
 private:
  int iteration;                       // Number of time , run has been called
  long t0;                             // time at the beginning og the last Run
  double elapsed;                      // Elapsed time at the end of the last Run (in second)
  long cstep;                          // Hop counter (number of time HopOnLeave had been called)
  bool gc_auto;                        // automatic gc or not
  long gc_freq;                        // garbage frequency
  bool gc_optimization;                // true if we optimize for speed rather than memory saving
};
