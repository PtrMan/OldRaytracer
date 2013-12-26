#include "vector_cStyle.h"
#include <math.h>

float vector_dotproduct(float ax, float ay, float az, float bx, float by, float bz) {
	return ax*bx + ay*by + az*bz;
}

void vector_normalisize(float &x, float &y, float &z) {
	float s = 1.0f/sqrtf(x*x + y*y + z*z);

	x *= s;
	y *= s;
	z *= s;
}

void vector_crossproduct(float ax, float ay, float az, float bx, float by, float bz, float &resx, float &resy, float &resz) {
	resx = ay*bz - az*by;
	resy = az*bx - ax*bz;
	resz = ax*by - ay*bx;
}

float vector_length(float x, float y, float z) {
	return sqrtf(x*x + y*y + z*z);
}

float vector_length(float x1, float y1, float z1, float x2, float y2, float z2) {
	return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

