/*
 * Squirrel project
 *
 * file ?	Squirrel.cpp
 * what	?	Main program
 * who	?	jlv
 * when	?	08/02/99
 * last	?	08/09/99
 *
 *
 * (c) electron Technology 1999
 */

#include <iostream.h>
#include <ctime>
#include "tokens.h"
#include "SQIParser.h"
#include "DLexerBase.h"
#include "DLGLexer.h"
#include "SQI-parser.h"

#define VER 0.16
#define STATUS "Dev"

char *CleanString(char *src)
{
 int size = strlen(src) - 1;
 char *str;
 int i;
 
 str = (char *)malloc(sizeof(char)*size);

 for(i=1;i<size;i++)
  str[i-1] = src[i];

 str[size-1] = '\0';

 return str;
}

// remove " from the begin of the string
char *CleanWord(char *src)
{
 int size = strlen(src);
 char *str;
 int i;
 
 str = (char *)malloc(sizeof(char)*size);

 for(i=1;i<size;i++)
   str[i-1] = src[i];

 str[size-1] = '\0';

 return str;
}	

/* package init function */

extern void SQI_Comm_Init(SQI_BuiltInMap *Map);
extern void SQI_WorkS_Init(SQI_BuiltInMap *Map);
extern void SQI_Arith_Init(SQI_BuiltInMap *Map);
extern void SQI_Control_Init(SQI_BuiltInMap *Map);
extern void SQI_Insp_Init(SQI_BuiltInMap *Map);
extern void SQI_ListH_Init(SQI_BuiltInMap *Map);
extern void SQI_AddOn_Init(SQI_BuiltInMap *Map);
extern void SQI_DataH_Init(SQI_BuiltInMap *Map);
extern void SQI_Time_Init(SQI_BuiltInMap *Map);

main(int argc,char **argv)
{
    SQI_Interp *Interp = new SQI_Interp();
    SQI_Parser<DLGLexer, SQIParser, ANTLRToken> *p;
    SQI_Squirrel *squirrel;
    try {
	
    long t0;
    
    cout << "Squirrel " << VER << " " << STATUS << "\n";
    cout << "Aloha !\n";
    cout << "\n";  

    // now we loading the builtin functions

    t0 = clock();
    SQI_AddOn_Init(Interp->BuiltInMap);	
    SQI_Comm_Init(Interp->BuiltInMap);
    SQI_WorkS_Init(Interp->BuiltInMap);	
    SQI_Arith_Init(Interp->BuiltInMap);
    SQI_Control_Init(Interp->BuiltInMap);
    SQI_Insp_Init(Interp->BuiltInMap);
    SQI_DataH_Init(Interp->BuiltInMap);
    SQI_ListH_Init(Interp->BuiltInMap);
    SQI_Time_Init(Interp->BuiltInMap);	
   
    cout << "Addons (Loaded in " << static_cast<double>(static_cast<double>(clock()-t0)/CLOCKS_PER_SEC) <<"s) : " << *(Interp->BuiltInMap->ListAddOn()) << "\n";
    	
    // we create the parser

    //ParserBlackBox<DLGLexer, SQIParser, ANTLRToken> p(stdin);

    p = new SQI_Parser<DLGLexer, SQIParser, ANTLRToken>;
   	squirrel = new SQI_Squirrel(Interp->GlobalHeap,Interp->GlobalVar);
    FILE *f;
    int errcode;

    for(int i=1;i<argc;i++)
      {
	if(!(f = fopen(argv[i], "r")))
	  {
	    cerr << "Cannot open file " << argv[i] << "\n";
	  }
	else
	  {
	    p->Stream(f);
	    
	    // and start it
	    t0 = clock();
	    SQI_nBlock *tree = p->parser()->interp(&errcode,Interp);
	    
	   	if(!errcode)
	   	{ 
	    	cout << "Tree builded in " << static_cast<double>(static_cast<double>(clock()-t0)/CLOCKS_PER_SEC) <<"s\n\n";
	    	//cout << *(tree->Print());
	    	
	    	// 	and starting it
	    
	    	cout << "Releasing the Squirrel\n\n";
	    
	    	squirrel->Release(Interp,tree);
	    
	    	cout << "\nThe Squirrel is done in " << squirrel->Elapsed() << " seconds (" << squirrel->Speed() << " hps)\n";
	   	}
	   	else
	   	{
	   		if(tree)
	   			tree->Forget();
	   	}
	   	
	    fclose(f);
	  }
      }

    p->Stream("print 'Thanks for trying me !!!\n'");
    SQI_nBlock *tree = p->parser()->interp(&errcode,Interp);
	    
    if(!errcode)
	squirrel->Release(Interp,tree);

    // that's all
    delete p;
    delete squirrel;
    //cout << "BEFORE\n" << *(Interp->Eternal->Dump()) << "\n";

    } catch (SQI_Exception *ex) {
      cout << "ERROR : " << *(ex->Print()) << "\n";
      delete p;
      delete squirrel;
      delete ex;
    }

    
    delete Interp;
    cout << "Aloha !\n";
}

