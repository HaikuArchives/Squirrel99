/*
 * Squirrel project
 *
 * file ?	SQI-glue.h
 * what	?	Glue type
 * who	?	jlv
 * when	?	11/26/99
 * last	?	02/03/01
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#define LEFT	0
#define RIGHT	1
#define TOP		2
#define BOTTOM 	3 

#define ALIGN_TOP		0
#define ALIGN_LEFT 		1
#define ALIGN_CENTER	2
#define ALIGN_RIGHT		3
#define ALIGN_BOTTOM	4
 
typedef struct TGlue {
	
	char alignment;
	list<SQI_BWidget *> *elements; 	
	
} TGlue;

extern BRect empty;

void MoveByChilds(list<TGlue *> *Childs,float h,float v,char except = -1);
void MoveByChilds(list<TGlue *> *Childs,char align,float h,float v,char except = -1);
double AreaChilds(list<TGlue *> *Childs);
SQI_BWidget *FindClosest(list<TGlue *> *Childs,SQI_BWidget *origin,char side,float *dist);
