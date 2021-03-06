#include "aabb.h"
#include "general.h"

// the AABB test algorithm
// (the algorithm is from http://people.csail.mit.edu/amy//papers/box-jgt.pdf
// [An Efﬁcient and Robust Ray–Box Intersection Algorithm]
// )


// this function initialisizes the aabbRay structure
// O? : the ray orgin
// D? : the ray direction
// AABBRay : is the structure that should get initialisized

void setup_aabb_ray(float Ox, float Oy, float Oz,
                    float Dx, float Dy, float Dz,
                    struct aabbRay* AABBRay
) {
	AABBRay->ox = Ox;
	AABBRay->oy = Oy;
	AABBRay->oz = Oz;
	
	AABBRay->dx = Dx;
	AABBRay->dy = Dy;
	AABBRay->dz = Dz;
	
	AABBRay->invdx = 1.0f/Dx;
	AABBRay->invdy = 1.0f/Dy;
	AABBRay->invdz = 1.0f/Dz;
	
	AABBRay->sign[0] = AABBRay->invdx < 0.0f;
	AABBRay->sign[1] = AABBRay->invdy < 0.0f;
	AABBRay->sign[2] = AABBRay->invdz < 0.0f;
}

// this does the aabb test with the ray
// Ray : is the Ray-structure of the specific Ray
// Box : is the AABB-Box structure of the AABB Box
// T0      : minimal ray-interval
// T1      : maximal ray-interval

bool test_aabb_ray(struct aabbRay* Ray, struct aabbBox* Box, float T0, float T1) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	tmin  = (Box->boundsx[  Ray->sign[0]] - Ray->ox) * Ray->invdx;
	tmax  = (Box->boundsx[1-Ray->sign[0]] - Ray->ox) * Ray->invdx;
	tymin = (Box->boundsy[  Ray->sign[1]] - Ray->oy) * Ray->invdy;
	tymax = (Box->boundsy[1-Ray->sign[1]] - Ray->oy) * Ray->invdy;
	
	if( (tmin > tymax) || (tymin > tmax) ) {
		return false;
	}
	if( tymin > tmin ) {
		tmin = tymin;
	}
	if( tymax < tmax ) {
		tmax = tymax;
	}
	
	tzmin = (Box->boundsz[  Ray->sign[2]] - Ray->oz) * Ray->invdz;
	tzmax = (Box->boundsz[1-Ray->sign[2]] - Ray->oz) * Ray->invdz;
	
	if( (tmin > tzmax) || (tzmin > tmax) ) {
		return false;
	}
	if( tzmin > tmin ) {
		tmin = tzmin;
	}
	if( tzmax < tmax ) {
		tmax = tzmax;
	}
	
	return (tmin < T1) && (tmax > T0);	
}
