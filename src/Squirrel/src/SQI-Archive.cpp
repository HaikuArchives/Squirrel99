/*
 * Squirrel project
 *
 * file ?	SQI-archive.h
 * what	?   Instantiate of archive functions and datas
 * who	?	jlv
 * when	?	11/13/99
 * last	?	11/13/99
 *
 *
 * (c) electron Technology 1999
 */

#include "SQI-Obj.h"

map<int,InstanceFunc,less<int> > InstantiateMap;

/*
 * function    : AddArchive
 * purpose     : Add an archive to the map of possible archive
 * input       : 
 *
 * int type, type of the object
 * InstanceFunc func, instantiate function
 *
 * output      : none
 * side effect : none
 */
void AddArchive(int type,InstanceFunc func)
{
	InstantiateMap[type] = func;	
}

/*
 * function    : Instantiate
 * purpose     : Instantiate an object from a BMessage
 * input       : 
 *
 * SQI_Squirrel *squirrel, squirrel running
 * BMessage *archive, the archived object
 *
 * output      : SQI_Object *, the object or NULL ig error
 * side effect : none
 */
SQI_Object *InstantiateAnObject(SQI_Heap *target,BMessage *archive)
{
	// first we need to check if the archive is a Squirrel object archived
		
	if(archive->HasInt8("SQI_Object"))
	{
		int8 type;
		InstanceFunc func = NULL;	
			
		archive->FindInt8("SQI_Object",0,&type);
		
		func = InstantiateMap[type];
		
		if(func)
			return (func)(target,archive);
		else
			return NULL;	
	}
	else
		return NULL;			
}

