/*
 * YataBe project
 *
 * file ?	fits2bitmap.h
 * what	?	Create a bitmap from a file
 * who	?	jlv
 * when	?	05/12/99
 * last	?	03/21/01
 *
 *
 * (c) Kirilla 2001
 */

#include <Bitmap.h>
#include <Rect.h>

#include "SQI-AddOn.h"
#include "fits.h"
#include "lut.h"
#include "minmax.h"

/*
 * function    : fits2bitmap
 * purpose     : Create a bitmap from the image data using the lut for the color
 * input       :
 *
 * TFits *fits, 		fits object
 * BBitmap **bitmap,	bitamp created
 * TLut	*lut,			LUT to use
 *
 *
 * output      : int, status
 * side effect : none
 */
int fits2bitmap(TFits *fits,BBitmap **bitmap,TLut *lut)
{
	color_space color;
	int status;
		
	// we create the bitmap
	
	switch(fits->bitpix)
	{
		case BYTE_IMG:
			{
				color = B_COLOR_8_BIT;
				break;
			}
		case SHORT_IMG:
		case LONG_IMG:
		case FLOAT_IMG:
		case DOUBLE_IMG:
		default:
			{
				color = B_RGB_32_BIT;
				break;
			}
	}
	
	if(!*bitmap)
	{
		BRect rect(0.0,0.0,fits->naxisn[1]-1,fits->naxisn[0]-1);
		*bitmap = new BBitmap(rect,color,true);
	}
	
	// and now fill the bitmap
	
	long size = fits->naxisn[1]*fits->naxisn[0];
		
	switch(fits->bitpix)
	{
		case BYTE_IMG:
			{
				char *data = new char[size];
				double step;
				char min,max;
				if(status=fits->Image_read(TBYTE,1,size,data))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*fits->naxisn[0];
					
				int x,y;
				long i;
							
				for (y=0;y<fits->naxisn[1];y++) {
					for (x=0;x<fits->naxisn[0];x++) {
					
						rgb[0]=0;
						
						i = ((fits->naxisn[1]-y) * fits->naxisn[0]) - fits->naxisn[0] + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case SHORT_IMG:
			{
				short *data = new short[size];
				double step;
				short min,max;
				if(status=fits->Image_read(TSHORT,1,size,data))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				minmax(data,size,&min,&max);
						
				step = lut->Step(min,max);
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*fits->naxisn[0];
					
				int x,y;
				long i;
							
				for (y=0;y<fits->naxisn[1];y++) {
					for (x=0;x<fits->naxisn[0];x++) {
					
						rgb[0]=0;
						
						i = ((fits->naxisn[1]-y) * fits->naxisn[0]) - fits->naxisn[0] + x;
					
						//printf("%d\n",i);
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case LONG_IMG:
			{
				long *data = new long[size];
				double step;
				long min,max;
				if(status=fits->Image_read(TLONG,1,size,data))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*fits->naxisn[0];
					
				int x,y;
				long i;
							
				for (y=0;y<fits->naxisn[1];y++) {
					for (x=0;x<fits->naxisn[0];x++) {
					
						rgb[0]=0;
						
						i = ((fits->naxisn[1]-y) * fits->naxisn[0]) - fits->naxisn[0] + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case FLOAT_IMG:
			{
				float *data = new float[size];
				double step;
				float min,max;
				if(status=fits->Image_read(TFLOAT,1,size,data))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*fits->naxisn[0];
					
				int x,y;
				long i;
							
				for (y=0;y<fits->naxisn[1];y++) {
					for (x=0;x<fits->naxisn[0];x++) {
					
						rgb[0]=0;
						
						i = ((fits->naxisn[1]-y) * fits->naxisn[0]) - fits->naxisn[0] + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}		
		case DOUBLE_IMG:
			{
				double *data = new double[size];
				double step;
				double min,max;
				if(status=fits->Image_read(TDOUBLE,1,size,data))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*fits->naxisn[0];
					
				int x,y;
				long i;
							
				for (y=0;y<fits->naxisn[1];y++) {
					for (x=0;x<fits->naxisn[0];x++) {
					
						rgb[0]=0;
						
						i = ((fits->naxisn[1]-y) * fits->naxisn[0]) - fits->naxisn[0] + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
			break;
	}	
	
	return 0;
			
}

// 
/*
 * function    : fits2bitmap
 * purpose     : Create a bitmap from a part of the image data using the lut for the color 
 * input       :
 *
 * TFits *fits, 		fits object
 * BBitmap **bitmap,	bitamp created
 * TLut	*lut,			LUT to use
 * long sx,				start x coordinate (pixel)
 * long ex,				end x coordinate (pixel)
 * long sy,				start y coordinate (pixel)
 * long ey,				end y coordinate (pixel)
 *
 * output      : int, status
 * side effect : none
 */
int fits2bitmap(TFits *fits,BBitmap **bitmap,TLut *lut,long sx,long ex,long sy,long ey)
{
	color_space color;
	int status;
		
	// we create the bitmap
	
	switch(fits->bitpix)
	{
		case BYTE_IMG:
			{
				color = B_COLOR_8_BIT;
				break;
			}
		case SHORT_IMG:
		case LONG_IMG:
		case FLOAT_IMG:
		case DOUBLE_IMG:
		default:
			{
				color = B_RGB_32_BIT;
				break;
			}
	}
	
	//long wide = ex - sx + 1;
	//long height = ey - sy + 1;
	
	long height = ex - sx + 1;
	long wide = ey - sy + 1;
	
	if(!*bitmap)
	{
		BRect rect(0.0,0.0,wide-1,height-1);
		*bitmap = new BBitmap(rect,color,true);
	}
	
	// and now fill the bitmap
	
	long size = wide * height;
		
	switch(fits->bitpix)
	{
		case BYTE_IMG:
			{
				char *data = new char[size];
				double step;
				char min,max;
				if(status=fits->Image_read(TBYTE,1,size,data,fits->naxisn[1],fits->naxisn[0],sx,ex,sy,ey,&min,&max))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				//minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*height;
					
				long x,y;
				long i;
							
				for (y=0;y<wide;y++) {
					for (x=0;x<height;x++) {
					
						rgb[0]=0;
						
						i = ((wide-y) * height) - height + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case SHORT_IMG:
			{
				short *data = new short[size];
				double step;
				short min,max;
				
				if(status=fits->Image_read(TSHORT,1,size,data,fits->naxisn[1],fits->naxisn[0],sx,ex,sy,ey,&min,&max))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				//minmax(data,size,&min,&max);
						
				step = lut->Step(min,max);
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*height;
					
				long x,y;
				long i;
										
				for (y=0;y<wide;y++) {
					for (x=0;x<height;x++) {
					
						rgb[0]=0;
						
						i = ((wide-y) * height) - height + x;
						//printf("%ld\n",i);
						
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case LONG_IMG:
			{
				long *data = new long[size];
				double step;
				long min,max;
				if(status=fits->Image_read(TLONG,1,size,data,fits->naxisn[1],fits->naxisn[0],sx,ex,sy,ey,&min,&max))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				//minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*height;
					
				long x,y;
				long i;
							
				for (y=0;y<wide;y++) {
					for (x=0;x<height;x++) {
					
						rgb[0]=0;
						
						i = ((wide-y) * height) - height + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
		case FLOAT_IMG:
			{
				float *data = new float[size];
				double step;
				float min,max;
				if(status=fits->Image_read(TFLOAT,1,size,data,fits->naxisn[1],fits->naxisn[0],sx,ex,sy,ey,&min,&max))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				//minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*height;
					
				long x,y;
				long i;
							
				for (y=0;y<wide;y++) {
					for (x=0;x<height;x++) {
					
						rgb[0]=0;
						
						i = ((wide-y) * height) - height + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}		
		case DOUBLE_IMG:
			{
				double *data = new double[size];
				double step;
				double min,max;
				if(status=fits->Image_read(TDOUBLE,1,size,data,fits->naxisn[1],fits->naxisn[0],sx,ex,sy,ey,&min,&max))
					return status;
				
				// we need to convert the data en RGB color according to
				// the current lut
														
				//minmax(data,size,&min,&max);
							
				step = lut->Step(min,max);
			
				uint8 *rgb = (uint8*)((*bitmap)->Bits());
				int32 bpr = (*bitmap)->BytesPerRow()-4*height;
					
				int x,y;
				long i;
							
				for (y=0;y<wide;y++) {
					for (x=0;x<height;x++) {
					
						rgb[0]=0;
						
						i = ((wide-y) * height) - height + x;
					
						lut->Get(STD,data[i],min,step,&(rgb[2]),&(rgb[1]),&(rgb[0]));
						
						rgb+=4;
					}
					rgb+=bpr;
				}		
					
				delete [] data;
				break;
			}
			break;
	}	
	
	return 0;
			
}
