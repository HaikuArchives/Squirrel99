/*
 * Squirrel project
 *
 * file ?	SQI-ufunc.h
 * what	?	Interface for handling the user function
 * who	?	jlv
 * when	?	07/30/99
 * last	?	12/06/00
 *
 *
 * (c) electron Technology 1999-2000
 */

#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-ufunc.h"

/*
 * function    : SQI_UFuncMap
 * purpose     : initialise the list of user function map
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_UFuncMap::SQI_UFuncMap()
{
	functions = hash_create(HASHCOUNT_T_MAX, NULL, NULL);
}

/*
 * function    : ~SQI_UFuncMap
 * purpose     : destroy the list of function map
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_UFuncMap::~SQI_UFuncMap()
{
 	SQI_Defunc *obj;
	hscan_t hs;
    hnode_t *hn;
    
    hash_scan_begin(&hs,functions);		
	while ((hn = hash_scan_next(&hs)))
	{
		obj = (SQI_Defunc *)hnode_get(hn);
      	delete obj;
      	free((char *)hnode_getkey(hn));		
	}
 
 	hash_free(functions);
}

/*
 * function    : List
 * purpose     : List the content of the map
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_UFuncMap::List()
{
  	string *str = new string("");
  	char *name;
	hscan_t hs;
    hnode_t *hn;  
  
	hash_scan_begin(&hs, functions);		
	while ((hn = hash_scan_next(&hs)))
	{
    	name = (char *)hnode_getkey(hn);
      	*str += name;
      	*str += " ";   			
	} 	 
      
  	return str;
}

/*
 * function    : AddFunc
 * purpose     : Add a function to the map
 * input       : 
 *
 * char *name, the function name
 * SQI_Defunc *func, pointer to the function
 *
 * output      : none
 * side effect : none
 */
void SQI_UFuncMap::AddFunc(char *name,SQI_Defunc *func)
{	
	SQI_Defunc *data = NULL;
 	hnode_t *hn;	
 	
	hn = hash_lookup(functions,name);
	if(hn)
	{
		data = (SQI_Defunc *)hnode_get(hn);	
		delete data;
		free((char *)hnode_getkey(hn));
		hash_scan_delfree(functions, hn);		
	}
	
	hash_alloc_insert(functions,strdup(name),func);	
}

/*
 * function    : RemoveFunc
 * purpose     : Remove a function from the map
 * input       : 
 *
 * string key, the function name
 *
 * output      : none
 * side effect : none
 */
void SQI_UFuncMap::RemoveFunc(string *name)
{
	SQI_Defunc *data = NULL;
 	hnode_t *hn;	
 	
	hn = hash_lookup(functions,name);
	if(hn)
	{
		data = (SQI_Defunc *)hnode_get(hn);	
		delete data;
		free((char *)hnode_getkey(hn));
		hash_scan_delfree(functions, hn);		
	}	
}

/*
 * function    : GetFunc
 * purpose     : Return a pointer to the function
 * input       : 
 *
 * string *key, name of the function
 *
 * output      : SQI_Defunc, pointer to the function , or NULL if none find
 * side effect : none
 */
SQI_Defunc *SQI_UFuncMap::GetFunc(string *key)
{
	SQI_Defunc *data = NULL;
 	hnode_t *hn;	
 	
	hn = hash_lookup(functions,key->c_str());
	if(hn)
		return data = (SQI_Defunc *)hnode_get(hn);		
	else
		return NULL;
}
