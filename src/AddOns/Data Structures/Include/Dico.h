/*
 * Squirrel project
 *
 * file ?	Dico.h
 * what	?   Dico object
 * who	?	jlv
 * when	?	11/12/99
 * last	?	03/21/00
 *
 *
 * (c) Kirilla 1999-2001
 */
 
#include <map>

#define SQI_DICO	5
#define CLASS_DSTRUCTURE	2

class DicoObject; // forwar def

//typedef map<string,SQI_Object *(DicoObject::*)(SQI_Squirrel *squirrel,SQI_Args *inputs),less<string> > DicoMemberDico;

typedef map<string,SQI_Object *,less<string> > ObjDico;

class DicoObject : public SQI_ThePointer
{
	public:
		DicoObject(SQI_Squirrel *squirrel);
		DicoObject(SQI_Heap *target);
		~DicoObject();
		void EmptyIt();
		void Export(SQI_Heap *nheap,char force = SQI_ETERNAL);
		// Archive methods
		status_t Archive(BMessage *archive);
  		status_t Instantiate(SQI_Heap *target,BMessage *archive);	
		// Member methods
		SQI_Object *Erase(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Empty(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		SQI_Object *Keys(SQI_Squirrel *squirrel,SQI_Args *inputs);	
		SQI_Object *Set(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Get(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Size(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Min(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Max(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Avg(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Exist(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Iterate(SQI_Squirrel *squirrel, SQI_Args *inputs);
		SQI_Object *IterateWithIndex(SQI_Squirrel *squirrel, SQI_Args *inputs);
		SQI_Object *Sort(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *SortNew(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Reverse(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *ReverseNew(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *Find(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *FindLast(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *FindAll(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *FindIf(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *FindIfAll(SQI_Squirrel *squirrel,SQI_Args *inputs);
		SQI_Object *FindIfLast(SQI_Squirrel *squirrel,SQI_Args *inputs);
		// legacy member from SQI_ThePointer
		SQI_Object *Member(string *member,SQI_Squirrel *squirrel,SQI_Args *inputs);
		bool Suicidal(bool force=false);
  		string *Print(int prec=3);
  		string *Show(int prec=3);
  		string *Dump();
  		SQI_Object *Clone(SQI_Heap *target = NULL);
  		SQI_Object *DeepClone(SQI_Heap *target = NULL);
  		// expr operations
  		SQI_Object *Inv();
  		SQI_Object *Neg();
		bool operator==(SQI_Object *op2);
  		bool operator!=(SQI_Object *op2);
		bool operator>(SQI_Object *op2);
		bool operator>=(SQI_Object *op2);
  		bool operator<(SQI_Object *op2);
  		bool operator<=(SQI_Object *op2);  		
  		SQI_Object *operator+(SQI_Object *op2);
  		SQI_Object *operator-(SQI_Object *op2);	
  		SQI_Object *operator*(SQI_Object *op2);
  		SQI_Object *operator/(SQI_Object *op2);
  		SQI_Object *Div(SQI_Object *op2);
  		SQI_Object *InvDiv(SQI_Object *op2);			
	private:				
		ObjDico *Dico;
		//DicoMemberDico *Members
};

// functions

SQI_ThePointer *NewDicoObject(SQI_Squirrel *squirrel);

void InitDicoObject();
void UninitDicoObject();

SQI_Object *InstantiateADico(SQI_Heap *target,BMessage *archive);
