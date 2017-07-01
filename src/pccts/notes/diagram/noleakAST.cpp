//
//  noleakAST.cpp
//

//  28-May-97	clearList does removeNode on all elements of list 

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include "noleakAST.h"
#include <stdio.h>

NoLeakAST::NoLeakAST() : 
	next(0),
	prev(0),
        serial(++counter) {
   insertNode();
}

NoLeakAST::~NoLeakAST() {
////  dumpNode("NoLeakAST Destructor");
   removeNode();
};

NoLeakAST::NoLeakAST(const NoLeakAST & from) :
	ASTBase(from),
	next(0),
	prev(0),
	serial(++counter) {
   insertNode();
}

// no need to copy information since both nodes already on list

NoLeakAST & NoLeakAST::operator = (const NoLeakAST & from) {
   return *this;
}

void NoLeakAST::insertNode() {
   if (listHead == 0) {
      listHead=this;
      prev=this;
      next=this;
   } else {
      next=listHead;
      prev=listHead->prev;
      next->prev=this;
      prev->next=this;      
      listHead=this;
   };
}

void NoLeakAST::removeNode() {
   next->prev=prev;
   prev->next=next;
   if (listHead == this) {
     listHead=next;
     if (listHead == this) {
	listHead=0;
     };
   };
   prev=0;
   next=0;     
}

void NoLeakAST::destroyList() {

   while (listHead != 0) {
     delete listHead;		// uses side-effect of destructor
   };
}

void NoLeakAST::dumpNode(const char * s) {

   if (s != 0) {printf("%s ",s);};
   printf("AST # %d\n",serial);
}

void NoLeakAST::dumpList() {
    
   NoLeakAST *	element;

   if (listHead == 0) {
      printf("\n*** AST list is empty ***\n");
   } else {
      printf("\n*** Dump of ASTs on list ***\n\n");
      element=listHead;
      do {
        element->dumpNode("DumpList");
        element=element->next;
      } while (element != listHead);
      printf("\n*** End of Dump ***\n\n");
   };
}

void NoLeakAST::dumpTree1(int indent) {

   NoLeakAST *	tree=this;

   dumpTreeIndent(indent);
   dumpNode();
   tree=(NoLeakAST *)(tree->down());
   while (tree != 0) {
       ( (NoLeakAST *)tree )->dumpTree1(indent+1);
       tree=(NoLeakAST *)tree->right();
   };
}

void NoLeakAST::removeTree() {

   NoLeakAST *	subTree;

   if (down() != 0) {
      for (subTree=(NoLeakAST *)down();
	   subTree != 0;
	   subTree=(NoLeakAST *)subTree->right()) {
         subTree->removeTree();
      };
   };
   removeNode();
}

void NoLeakAST::dumpTree() {

   printf("\n");
   dumpTree1(0);
}

void NoLeakAST::dumpTreeIndent(int indent) {

   int		i;

   for (i=0;i<indent;i++) {
     printf("  ");
   };
}

void NoLeakAST::clearList() {

   while (listHead != 0) {	// uses side-effect of removeNode
     listHead->removeNode();
   };
}

void NoLeakAST::clearCounter() {
   counter=0;
}

NoLeakAST * NoLeakAST::listHead=0;
int NoLeakAST::counter=0;

