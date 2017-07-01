/*
 * YataBe project
 *
 * file ?	lut.cpp
 * what	?	Implement the LUT class
 * who	?	jlv
 * when	?	05/13/99
 * last	?	03/20/01
 *
 *
 * (c) Kirilla 2001
 */

#include <File.h>
#include <Entry.h>
#include <Path.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lut.h"
#include "Utils.h"

/*
 * function    : TLut
 * purpose     : Constructor
 * input       :
 *
 * BEntry *lut, lut file to create the object from
 *
 * output      : none
 * side effect : none
 */
TLut::TLut(BEntry *lut)
{
  int i,j;
  float r,g,b;
  char str[24];
  char nm[B_FILE_NAME_LENGTH];
  BPath path;
  
  lut->GetName(nm);
  lut->GetPath(&path);
  
  // we get the LUT name
   
  int lst = last(nm,'.');
  
  if(lst!=-1)
  	name = strsub(nm,0,lst-1);
  else
  	name = strdup(nm);
      
  BFile *file = new BFile(lut,B_READ_ONLY);
  
  if (file->InitCheck()==B_NO_ERROR)
  {
  	// alloc the rgb color array

  	rgb = (unsigned char **)malloc(sizeof(unsigned char *)*256);
  	for(i=0;i<256;i++)
    	rgb[i] = (unsigned char *)malloc(sizeof(unsigned char)*3);

 
    for(i=0;i<256;i++)
     {
     	// there's only 24 char by line 
     	file->Read(str,24);
		sscanf(str,"%f %f %f\n",&r,&g,&b);
		rgb[i][0] = (unsigned char)(floor(r * 255));
		rgb[i][1] = (unsigned char)(floor(g * 255));
		rgb[i][2] = (unsigned char)(floor(b * 255)); 
	 }

	file->Unset();
	delete file;  
  }
  else
  	rgb=NULL;
}

/*
 * function    : ~TLut
 * purpose     : Destructor
 * input       : none
 * output      : none
 * side effect : none
 */
TLut::~TLut()
{
  for(int i=0;i<256;i++)
    free(rgb[i]);

  free(rgb);
  delete [] name;

}

/*
 * function    : Step
 * purpose     : Compute size of a step
 * input       :
 *
 * char min, min value
 * char max, max value
 *
 * output      : double
 * side effect : none
 */
double TLut::Step(char min,char max)
{
  return ((double)(abs(abs(max)-min)))/256;
}

/*
 * function    : Get
 * purpose     : Get a color for a given value
 * input       :
 *
 * int order, 			?? (can't remember :(
 * char value,			value to get the color for
 * char min,			minimum value
 * double step,			value of a step
 * unsigned char *r,	red composant of the color
 * unsigned char *g,	green composant of the color
 * unsigned char *b,	blue composant of the color
 *
 * output      : none
 * side effect : none
 */
void TLut::Get(int order,char value,char min,double step,unsigned char *r,unsigned char *g,unsigned char *b)
{
  int i;

  i = abs(order - (int)(floor((abs(value-min))/step)));

  *r = rgb[i][0];
  *g = rgb[i][1];
  *b = rgb[i][2];
}

/*
 * function    : Step
 * purpose     : Compute size of a step
 * input       :
 *
 * int min, min value
 * int max, max value
 *
 * output      : double
 * side effect : none
 */
double TLut::Step(int min,int max)
{
  return ((double)(abs(abs(max)-min)))/256;
}

/*
 * function    : Get
 * purpose     : Get a color for a given value
 * input       :
 *
 * int order, 			?? (can't remember :(
 * int value,			value to get the color for
 * int min,				minimum value
 * double step,			value of a step
 * unsigned char *r,	red composant of the color
 * unsigned char *g,	green composant of the color
 * unsigned char *b,	blue composant of the color
 *
 * output      : none
 * side effect : none
 */
void TLut::Get(int order,int value,int min,double step,unsigned char *r,unsigned char *g,unsigned char *b)
{
  int i;

  i = abs(order - (int)(floor((abs(value-min))/step)));
 
 	//printf("%d\n",i);
 
  *r = rgb[i][0];
  *g = rgb[i][1];
  *b = rgb[i][2];
}

/*
 * function    : Step
 * purpose     : Compute size of a step
 * input       :
 *
 * long min, min value
 * long max, max value
 *
 * output      : double
 * side effect : none
 */
double TLut::Step(long min,long max)
{
  return ((double)(abs(abs(max)-min)))/256;
}

/*
 * function    : Get
 * purpose     : Get a color for a given value
 * input       :
 *
 * int order, 			?? (can't remember :(
 * long value,			value to get the color for
 * long min,			minimum value
 * double step,			value of a step
 * unsigned char *r,	red composant of the color
 * unsigned char *g,	green composant of the color
 * unsigned char *b,	blue composant of the color
 *
 * output      : none
 * side effect : none
 */
void TLut::Get(int order,long value,long min,double step,unsigned char *r,unsigned char *g,unsigned char *b)
{
  int i;

  i = abs(order - (int)(floor((abs(value-min))/step)));

  *r = rgb[i][0];
  *g = rgb[i][1];
  *b = rgb[i][2];
}

/*
 * function    : Step
 * purpose     : Compute size of a step
 * input       :
 *
 * float min, min value
 * float max, max value
 *
 * output      : double
 * side effect : none
 */
double TLut::Step(float min,float max)
{
  return ((double)(fabs(fabs(max)-min)))/256;
}

/*
 * function    : Get
 * purpose     : Get a color for a given value
 * input       :
 *
 * int order, 			?? (can't remember :(
 * float value,			value to get the color for
 * float min,			minimum value
 * double step,			value of a step
 * unsigned char *r,	red composant of the color
 * unsigned char *g,	green composant of the color
 * unsigned char *b,	blue composant of the color
 *
 * output      : none
 * side effect : none
 */
void TLut::Get(int order,float value,float min,double step,unsigned char *r,unsigned char *g,unsigned char *b)
{
  int i;

  i = abs(order - (int)(floor((fabs(value-min))/step)));

  *r = rgb[i][0];
  *g = rgb[i][1];
  *b = rgb[i][2];
}

/*
 * function    : Step
 * purpose     : Compute size of a step
 * input       :
 *
 * double min, min value
 * double max, max value
 *
 * output      : double
 * side effect : none
 */
double TLut::Step(double min,double max)
{
  return ((double)(fabs(fabs(max)-min)))/256;
}

/*
 * function    : Get
 * purpose     : Get a color for a given value
 * input       :
 *
 * int order, 			?? (can't remember :(
 * double value,			value to get the color for
 * double min,			minimum value
 * double step,			value of a step
 * unsigned char *r,	red composant of the color
 * unsigned char *g,	green composant of the color
 * unsigned char *b,	blue composant of the color
 *
 * output      : none
 * side effect : none
 */
void TLut::Get(int order,double value,double min,double step,unsigned char *r,unsigned char *g,unsigned char *b)
{
  int i;

  i = abs(order - (int)(floor((fabs(value-min))/step)));

  *r = rgb[i][0];
  *g = rgb[i][1];
  *b = rgb[i][2];
}
