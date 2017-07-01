/*
 * Squirrel project
 *
 * file ?	MethodTable.cpp
 * what	?   Hash function table
 * who	?	jlv
 * when	?	02/03/01
 * last	?	03/21/01
 *
 *
 * (c) Kirilla 2001
 */
 
#include "SQI-AddOn.h"

#include "MethodTable.h"

typedef struct TMeth {
	
	OMeth thefunc;	
	
} TMeth;

MethodTable::MethodTable()
{
	funcs = hash_create(HASHCOUNT_T_MAX, NULL, NULL);	
}

MethodTable::~MethodTable()
{
	TMeth *f;
	hscan_t hs;
    hnode_t *hn;
			
	hash_scan_begin(&hs,funcs);		
	while ((hn = hash_scan_next(&hs)))
	{
		f = (TMeth *)hnode_get(hn);	
		delete f;			
	}		
		
	hash_free(funcs);		
}

void MethodTable::Add(const char *name,OMeth func)
{	
	TMeth *f = new TMeth;
	
	f->thefunc = func;	

	hash_alloc_insert(funcs,name,(char *)f);	
}

void MethodTable::Rem(const char *name)
{
	hnode_t *hn;
	TMeth *f;	
	
	hn = hash_lookup(funcs,name);
	if(hn)
	{		
		f = (TMeth *)hnode_get(hn);	
		delete f;		
		hash_scan_delfree(funcs, hn);	
	}
}

OMeth MethodTable::Seek(const char *name)
{
	hnode_t *hn;
	TMeth *f;
	hn = hash_lookup(funcs,name);
	if(hn)
	{
		f = (TMeth *)hnode_get(hn);
		return f->thefunc;			
	}		
	else
		return NULL;
}


