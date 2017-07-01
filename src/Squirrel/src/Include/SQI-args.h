/*
 * Squirrel project
 *
 * file ?	SQI-args.h
 * what	?   Arguments array
 * who	?	jlv
 * when	?	11/02/99
 * last	?	11/02/99
 *
 *
 * (c) electron Technology 1999
 */

class SQI_Object; // forward def

/*
 * Class   : SQI_Args
 * Purpose : Array of Arguments 
 *
 */
class SQI_Args
{
	public:
		SQI_Args(long n);
		~SQI_Args();
		SQI_Object *operator[](long i);
		void push_back(SQI_Object *obj);
		long Length() {return size;};	
	private:
		long pos;
		long size;
		SQI_Object **args;
};

