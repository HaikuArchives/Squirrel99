//
//  myPanic.cpp
//

/*  This file should be accompanied by DISCLAIMER.TXT stating disclaimers */

#include <stdlib.h>
#include <stdio.h>

void myPanic(const char * s) {
   printf ("\nPanic: %s\n",s);
   exit(1);
};
