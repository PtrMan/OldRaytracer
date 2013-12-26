#ifndef _H_aabb
#define _H_aabb

#include "general.h"

// this is the structure for the AABB box
struct aabbBox {
	float boundsx[2];
	float boundsy[2];
	float boundsz[2];
};

// this is a (accellation) structure for a single ray for the test vs. a aabb
struct aabbRay {
	float ox;
	float oy;
	float oz;
	
	float dx;
	float dy;
	float dz;
	
	float invdx;
	float invdy;
	float invdz;
	
	int sign[3];
};



#ifdef __cplusplus
extern "C" {
#endif


void setup_aabb_ray(float Ox, float Oy, float Oz,
                    float Dx, float Dy, float Dz,
                    struct aabbRay* AABBRay
);

bool test_aabb_ray(struct aabbRay* Ray, struct aabbBox* Box, float T0, float T1);

#ifdef __cplusplus
}
#endif

#endif