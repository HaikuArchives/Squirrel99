
typedef long status_t;
typedef unsigned long uint32;

enum scale_method {
	IMG_SCALE_BILINEAR = 1
};

class BBitmap;
class BRect;

// compute x/y factor to scale a bitmap
void ComputeFactor(BRect src,BRect trg,float *x,float *y);

// Scale an image.  It will work in either direction.
// Scaling up or down.  Scaling by integer values will be
// most optimal.

status_t	scale(const BBitmap *source, BBitmap *dst, 
				const float xFactor = -1, const float yFactor = -1,
				scale_method = IMG_SCALE_BILINEAR);

// These would make the transformation manipulations complete
status_t	rotate(const BBitmap *source, BBitmap *dest, uint32 degrees);

status_t	shear();

status_t	translate();

status_t	transform();

