/*
 * Squirrel project
 *
 * file ?	SQI-interp.h
 * what	?	Logo Interpreter class
 * who	?	jlv
 * when	?	07/27/99
 * last	?	02/19/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include "SQI-stream.h"
#include <Looper.h>
//#include "SQI-parser.h"

class SQI_BuiltInMap; 	// forward def
class SQI_Pool;			// forward def	

/*
 * Class   : SQI_Interp
 * Purpose : Class Interp 
 *
 */
class SQI_Interp {
	public:
	  SQI_Interp();
	  ~SQI_Interp();
	  SQI_Object *ParseAString(string *str);
	  SQI_Object *(*IsBuiltIn(string *funcname))(SQI_Args *,SQI_Squirrel *,SQI_Interp *);
	  SQI_Defunc *IsUFunc(string *funcname);
	  SQI_Node *FindCall(const char *cmd);       // Create a SQI_Node when we have a call to a function
	  void RemoveFunc(string *name);              // Remove a procedure from the user function map
	  SQI_Heap *Eternal;                   // Heap for the object created by the tree builder
	  SQI_Heap *GlobalHeap;                // Heap for the global object
	  SQI_Heap *Tree;                      // Heap for the node of the tree
	  SQI_VarMap *GlobalVar;               // Global variable map
	  SQI_BuiltInMap *BuiltInMap;          // Builtin function map
	  SQI_UFuncMap *UFuncMap;              // User function map
	  SQI_Number *True;                    // True number
	  SQI_Number *False;                   // False number
	  SQI_Stream *Stream;                  // In/out stream to the Client
	  BLooper *Director;                   // Pointer to the Director of the Interpreter
	  SQI_Pool *Pool;					   // Pool of ready to be reused Squirrel
	  //SQI_Parser<DLGLexer, SQIParser, ANTLRToken> *TheParser;		
};

#include <stack>

// Squirrel Pool class
class SQI_Pool : public BLocker
{
	public:
		SQI_Pool();
		~SQI_Pool();
		SQI_Squirrel *Get(SQI_Interp *interpreter);
		void Put(SQI_Squirrel *squirrel);
	private:
		stack<SQI_Squirrel *> Pool;			
};
