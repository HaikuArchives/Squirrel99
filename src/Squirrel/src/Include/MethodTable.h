/*
 * Squirrel project
 *
 * file ?	MethodTable.h
 * what	?   Hash Method table
 * who	?	jlv
 * when	?	02/03/01
 * last	?	03/21/01
 *
 *
 * (c) Kirilla 2001
 */
 
#ifndef _METHODTABLE
#define _METHODTABLE

class SQI_ThePointer; // forward def

typedef SQI_Object *(SQI_ThePointer::*OMeth)(SQI_Squirrel *squirrel,SQI_Args *inputs);

#include "hash.h"

class MethodTable {
	public:
		MethodTable();
		~MethodTable();
		
		void Add(const char *name,OMeth func);
		void Rem(const char *name);
		OMeth Seek(const char *name);
				
	private:
		hash_t *funcs;
	
};


#endif 
   
