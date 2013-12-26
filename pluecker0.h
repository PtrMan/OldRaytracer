#ifndef _H_pluecker0
#define _H_pluecker0

#include "general.h"

// NOTE< this algo is wrong ? >

struct pluecker_test_ {
	struct v4 Di;
	struct v4 Dj;
	struct v4 Dk;
	
	struct v4 r1;
	struct v4 r2;
	struct v4 r3;
	
	struct v4 signFalse;

	struct v4 reta;

	struct v4 v11;
	struct v4 v21;
	struct v4 v31;
	struct v4 v41;
	struct v4 v51;
	struct v4 v61;

	struct v4 v12;
	struct v4 v22;
	struct v4 v32;
	struct v4 v42;
	struct v4 v52;
	struct v4 v62;

	struct v4 v13;
	struct v4 v23;
	struct v4 v33;
	struct v4 v43;
	struct v4 v53;
	struct v4 v63;
};

#ifdef __cplusplus
extern "C" {
#endif
 
void pluecker_calc_triangle(struct pluecker_test_* pt,
						   float p1x, float p1y, float p1z,
						   float p2x, float p2y, float p2z,
						   float p3x, float p3y, float p3z
);

void pluecker_calc_ray(struct pluecker_test_* pt,
					   struct v4 Ox, struct v4 Oy, struct v4 Oz,
					   struct v4 Dx, struct v4 Dy, struct v4 Dz
);

void pluecker_test(void*);
 
#ifdef __cplusplus
}
#endif

#endif
