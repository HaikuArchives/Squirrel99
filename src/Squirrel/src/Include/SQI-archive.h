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
 
#include <Message.h> 
 
class SQI_Heap;     // forward def 
class SQI_Object;   // forward def

typedef SQI_Object *(*InstanceFunc)(SQI_Heap *target,BMessage *archive);

void AddArchive(int type,InstanceFunc func);
SQI_Object *InstantiateAnObject(SQI_Heap *target,BMessage *archive);


