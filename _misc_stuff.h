#ifndef _H__MISC_STUFF
#define _H__MISC_STUFF

// this is everything i dunno about

#define MSVC
#include "intrinsic.h"

// this is a frustrum sphere (a object against that the frustum can be tested)
// NOTE< the radius is positive >
struct frustrumSphere {
	float posx;
	float posy;
	float posz;
	float radius;
};


// this is the structure for the frustrum culling algorithm for a "big ray-packet"

// (the planes points "inside" the frustrum)

// (allready simd)
struct frustrum {
	// each component (x,y,z) is the normalisized direction (vector) of the frustrum plan at each side
	// w is the dot-product of the Point of the Plane and the vector (so that x,y,z and w are the plane equation)

	// sides of frustrum (for screen space)
	//  -- 1 --
	// |       |
	// 4       2
	// |       |
	//  -- 3 --

	// so the direction of plane 1 is x[0], y[0], z[0] and so on

	//struct v4 x;
	//struct v4 y;
	//struct v4 z;

	//struct v4 w;
	Intrinsic::f4 x;
	Intrinsic::f4 y;
	Intrinsic::f4 z;

	Intrinsic::f4 w;

	// this are helpvariables for the calculation
	Intrinsic::f4 v1_x;
	Intrinsic::f4 v1_y;
	Intrinsic::f4 v1_z;

	Intrinsic::f4 v2_x;
	Intrinsic::f4 v2_y;
	Intrinsic::f4 v2_z;
};


// this is used to indicate a full array that don't have enougth space...
class ArrayOverflow {
};

// this is a 2d coordinate or something in long numbers
struct xyInt {
	unsigned long x;
	unsigned long y;
};


#define SKALAR _[0]

// define the size of a tile (for one thread)
#define TILESIZE_X 256
#define TILESIZE_Y 256

// math things
#define PI 3.1415926535f

// fasti = fast integer = native integer format
typedef unsigned long fasti ;


#endif
