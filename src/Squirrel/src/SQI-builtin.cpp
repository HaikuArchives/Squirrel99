/*
 * Squirrel project
 *
 * file ?	SQI-builtin.cpp
 * what	?	Interface for handling the builtin procedure
 * who	?	jlv
 * when	?	07/27/99
 * last	?	02/09/01
 *
 *
 * (c) Kirilla 1999-2001
 */

#include <stdio.h>
#include <string.h>
#include "SQI-Obj.h"
#include "SQI-heap.h"
#include "SQI-exception.h"
#include "SQI-varmap.h"
#include "SQI-builtin.h"
#include "SQI-ufunc.h"
#include "SQI-interp.h"
#include "SQI-squirrel.h"

/*
 * function    : SQI_BuiltInMap
 * purpose     : initialise the list of builtin map
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BuiltInMap::SQI_BuiltInMap()
{
	functions = hash_create(HASHCOUNT_T_MAX, NULL, NULL);
	AddOns = hash_create(HASHCOUNT_T_MAX, NULL, NULL);
}

/*
 * function    : ~SQI_BuiltInMap
 * purpose     : destroy the list of builtin map
 * input       : none
 * output      : none
 * side effect : none
 */
SQI_BuiltInMap::~SQI_BuiltInMap()
{
  	AddOnData *obj2;
	hscan_t hs;
    hnode_t *hn;
    image_id id;

	// separate remove function call for the add-on to the
	// actual add-on removal, as it was causing crash
			
	hash_scan_begin(&hs, AddOns);		
	while ((hn = hash_scan_next(&hs)))
	{
		obj2 = (AddOnData *)hnode_get(hn);
		//cout << (char *)hnode_getkey(hn) << "\n";
      	(*(obj2->function))(this);		
	}	
	
	hash_scan_begin(&hs, AddOns);		
	while ((hn = hash_scan_next(&hs)))
	{
		obj2 = (AddOnData *)hnode_get(hn);	
      	id = obj2->id;
      	//cout << (char *)hnode_getkey(hn) << "\n";
      	delete obj2;   
 		unload_add_on(id); 			
	}	
	
	
	hash_free(AddOns);
	hash_free(functions);
}

/*
 * function    : Print
 * purpose     : Print the content of the map
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::Print()
{
  	string *str = new string("");
  	char *name;
  	string *value;
	hscan_t hs;
    hnode_t *hn;  
  
	hash_scan_begin(&hs, functions);		
	while ((hn = hash_scan_next(&hs)))
	{
    	name = (char *)hnode_getkey(hn);
      	value = GetFuncInfo(name);
      	*str += name;
      	*str += " ->\n";
      	*str += *value;
      	*str += "\n";  
      	delete value;  			
	} 	 
      
  	return str;
}

/*
 * function    : ListFunc
 * purpose     : List the content of the builtin map
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::ListFunc()
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
	}; 	
     
  	return str;
}

/*
 * function    : ListAddOn
 * purpose     : List the content of the addon map
 * input       : none
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::ListAddOn()
{
  	string *str = new string("");
  	char *name;
	hscan_t hs;
    hnode_t *hn;   	
 
	hash_scan_begin(&hs, AddOns);		
	while ((hn = hash_scan_next(&hs)))
	{
    	name = (char *)hnode_getkey(hn);
    	*str += name;
      	*str += " ";
	}; 	

  	return str;
}

/*
 * function    : ListAddOn
 * purpose     : List the content of the addon map
 * input       : 
 *
 * SQI_List *lst, list to fill
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::ListAddOn(SQI_List *lst)
{
  	char *name;
	hscan_t hs;
    hnode_t *hn;   	
 
	hash_scan_begin(&hs, AddOns);		
	while ((hn = hash_scan_next(&hs)))
	{
    	name = (char *)hnode_getkey(hn);
 		lst->Add2End(new SQI_String(lst->heap,name));
	} 
}

/*
 * function    : AddFunc
 * purpose     : Add a function to the map
 * input       : 
 *
 * char *name, name of the function
 * char *pkg, name of the package the function is in
 * char *what, Purpose of the function
 * char *usage, usage of the function
 * BuiltInFunc *function, pointer to the function
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::AddFunc(char *name,char *pkg,char *what,char *usage,SQI_Object *(*function)(SQI_Args *,SQI_Squirrel *,SQI_Interp *))
{
	hnode_t *hn;
  	BuiltInData *data;
 
	hn = hash_lookup(functions,name);
	if(hn)
	{
		// replace it
		data = (BuiltInData *)hnode_get(hn);
		data->pkg = pkg;
  		data->purpose = what;
  		data->usage = usage;
  		data->function = function;			
	}
	else
	{
		// add it
		data = new BuiltInData;
		data->pkg = pkg;
  		data->purpose = what;
  		data->usage = usage;
  		data->function = function;
		hash_alloc_insert(functions,name,data);
	}
}

/*
 * function    : AddAddOn
 * purpose     : Add a addon to the map
 * input       : 
 *
 * char *name, name of the addon
 * char *purpose, purpose of the addon
 * char *who, author of the addon
 * char *version, version of the addon
 * char *when, last update of the addon
 * void (*function)(void) function to call when removing the addon
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::AddAddOn(image_id addon_id,char *name,char *purpose,char *author,char *version,char *date,void (*function)(SQI_BuiltInMap *))
{
	hnode_t *hn;
  	AddOnData *data;
 
	hn = hash_lookup(AddOns,name);
	if(hn)
	{
		// replace it
		data = (AddOnData *)hnode_get(hn);
  		data->id = addon_id;
  		data->purpose = purpose;
  		data->author = author;
  		data->version = version;
  		data->date = date;
  		data->function = function;			
	}
	else
	{
		// add it
		data = new AddOnData;
  		data->id = addon_id;
  		data->purpose = purpose;
  		data->author = author;
  		data->version = version;
  		data->date = date;
  		data->function = function;		
		hash_alloc_insert(AddOns,name,data);
	}	
}

/*
 * function    : RemoveFunc
 * purpose     : Remove a function from the map
 * input       : 
 *
 * string *name, name of the function to remove
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::RemoveFunc(string *name)
{
	BuiltInData *data;
	hnode_t *hn;
	
	hn = hash_lookup(functions,name->c_str());
	if(hn)
	{		
		data = (BuiltInData *)hnode_get(hn);
		delete data;
		hash_scan_delfree(functions, hn);	
	}
}

/*
 * function    : RemoveFunc
 * purpose     : Remove a function from the map
 * input       : 
 *
 * char *name, name of the function to remove
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::RemoveFunc(char *name)
{
	BuiltInData *data;
	hnode_t *hn;
	
	hn = hash_lookup(functions,name);
	if(hn)
	{		
		data = (BuiltInData *)hnode_get(hn);
		delete data;
		hash_scan_delfree(functions, hn);	
	}
}

/*
 * function    : RemoveAddOn
 * purpose     : Remove an addon from the map
 * input       : 
 *
 * string *name, name of the addon to remove
 *
 * output      : none
 * side effect : none
 */
void SQI_BuiltInMap::RemoveAddOn(string *name)
{
  	void (*f)(SQI_BuiltInMap *);
  	AddOnData *data;
	hnode_t *hn;
	image_id id;

	hn = hash_lookup(AddOns,name);
	if(hn)
	{		
		data = (AddOnData *)hnode_get(hn);
  		f = data->function;
  		(*f)(this); // calling the remove function of the AddOn
 		id = data->id;
 		delete data;
 		unload_add_on(id);
	}
}

/*
 * function    : GetFunc
 * purpose     : Return the function pointer for a builtin function
 * input       : 
 *
 * string *key, name of the function
 *
 * output      : BuiltInFunc, pointer to the function , or NULL if none find
 * side effect : none
 */
SQI_Object *(*SQI_BuiltInMap::GetFunc(string *key))(SQI_Args *args,SQI_Squirrel *,SQI_Interp *)
{
	BuiltInData *func = NULL;
	hnode_t *hn;

	hn = hash_lookup(functions,key->c_str());
	if(hn)
	{
		func = (BuiltInData *)hnode_get(hn);
		return func->function;	
	}
	else
		return NULL;	
}

/*
 * function    : GetFuncInfo
 * purpose     : Return the info for a builtin function
 * input       : 
 *
 * string *key, name of the function
 *
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::GetFuncInfo(const string *key)
{
  	BuiltInData *data;
  	hnode_t *hn;
  
	hn = hash_lookup(functions,key->c_str());
	if(hn)
	{ 
		data = (BuiltInData *)hnode_get(hn);
      	string *out = new string("AddOn   :\t");
      	*out += data->pkg;
      	*out += "\nPurpose :\t";
      	*out += data->purpose;
      	*out += "\nUsage   :\t";
 	  	*out += data->usage;
	  	return out;
    }
    else
    	return NULL;
}

/*
 * function    : GetFuncInfo
 * purpose     : Return the info for a builtin function
 * input       : 
 *
 * char *key, name of the function
 *
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::GetFuncInfo(char *key)
{
  	BuiltInData *data;
  	hnode_t *hn;
  
	hn = hash_lookup(functions,key);
	if(hn)
	{ 
		data = (BuiltInData *)hnode_get(hn);
      	string *out = new string("AddOn   :\t");
      	*out += data->pkg;
      	*out += "\nPurpose :\t";
      	*out += data->purpose;
      	*out += "\nUsage   :\t";
 	  	*out += data->usage;
	  	return out;
    }
    else
    	return NULL;
}

/*
 * function    : GetAddOnInfo
 * purpose     : Return the info for an addon
 * input       : 
 *
 * string key, name of the addon
 *
 * output      : string *
 * side effect : none
 */
string *SQI_BuiltInMap::GetAddOnInfo(const string *key)
{
  	AddOnData *data;
  	hnode_t *hn;
  	
	hn = hash_lookup(AddOns,key->c_str());
	if(hn)
	{ 
		data = (AddOnData *)hnode_get(hn);  
      	string *out = new string("Purpose  :\t");
  	  	*out += data->purpose;
      	*out += "\nAuthor   :\t";
	  	*out += data->author;
      	*out += "\nVersion  :\t";
 	  	*out += data->version;
      	*out += "\nUpdate   :\t";
	  	*out += data->date;
	  	return out;
    }
    else
    	return NULL;
}

/*
 * function    : ExistAddOn
 * purpose     : Check if an addon is allready loaded
 * input       : 
 *
 * string key, name of the addon
 *
 * output      : string *
 * side effect : none
 */
bool SQI_BuiltInMap::ExistAddOn(const string *key)
{
  	AddOnData *data;
  	hnode_t *hn;
  	
	hn = hash_lookup(AddOns,key->c_str());
	if(hn)
		return true;
	else
		return false; 		
}


/*
 * function    : GetAddOnFunc
 * purpose     : Return all the function from an AddOn
 * input       : 
 *
 * SQI_List *lst, list to fill
 * string *addon, name of the addon
 *
 * output      : string *
 * side effect : none
 */
void SQI_BuiltInMap::GetAddOnFunc(SQI_List *lst,string *addon)
{
  	string *str = new string("");
  	char *name;
  	BuiltInData *data;
	hscan_t hs;
    hnode_t *hn;   	
  
	hash_scan_begin(&hs, functions);		
	while ((hn = hash_scan_next(&hs)))
	{
		data = (BuiltInData *)hnode_get(hn);
    	name = (char *)hnode_getkey(hn);
    		
    	if(!strcmp(addon->c_str(),data->pkg))
    		lst->Add2End(new SQI_Keyword(lst->heap,name));				
	}  
}
