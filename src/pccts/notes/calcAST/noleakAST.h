#ifndef NOLEAKAST_H
#define NOLEAKAST_H

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "ASTBase.h"

struct NoLeakAST : public ASTBase {

       /* constructor */	NoLeakAST();
       /* destructor */ virtual	~NoLeakAST();
       /* copy constructor */	NoLeakAST(const NoLeakAST &);
       NoLeakAST &	        operator = (const NoLeakAST &);
static void			clearList();		   // reset list
static void			clearCounter();
static void			destroyList();		   // over list
       virtual void		dumpNode(const char * s=0);
static void			dumpList();		   // over list
       virtual void		dumpTree();		   // 
       void			insertNode();		   // to list
       void			removeNode();		   // from list
       void			removeTree();		   // from list

static NoLeakAST *		listHead;		   // all NoLeakASTs
       NoLeakAST *		next;			   // on list
       NoLeakAST *		prev;			   // on list
    
protected:
       virtual void		dumpTree1(int indent);
       virtual void		dumpTreeIndent(int indent);      

static int			counter;		   // incr by ctor
       int			serial;
};

#endif
