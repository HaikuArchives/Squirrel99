/*
 * YataBe project
 *
 * file ?	lut.h
 * what	?	Define LUT class
 * who	?	jlv
 * when	?	05/13/99
 * last	?	03/20/01
 *
 *
 * (c) Kirilla 2001
 */

#define STD 0
#define INV 255

class TLut {

public:
  TLut(BEntry *lut);
  ~TLut();
  double Step(char min,char max);
  void Get(int order,char value,char min,double step,unsigned char *r,unsigned char *g,unsigned char *b);
  double Step(int min,int max);
  void Get(int order,int value,int min,double step,unsigned char *r,unsigned char *g,unsigned char *b);
  double Step(long min,long max);
  void Get(int order,long value,long min,double step,unsigned char *r,unsigned char *g,unsigned char *b);
  double Step(float min,float max);
  void Get(int order,float value,float min,double step,unsigned char *r,unsigned char *g,unsigned char *b);
  double Step(double min,double max);
  void Get(int order,double value,double min,double step,unsigned char *r,unsigned char *g,unsigned char *b);
  char *name;
private:
  	unsigned char **rgb;
};
