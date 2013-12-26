
#ifndef _H__RAY_STUFF
#define _H__RAY_STUFF

#include "general.h"
#include "v4ptr.h"
#define MSVC
#include "intrinsic.h"

// ptp : plueckertest-polygon
struct ptp {
	// first egde
	v4 a0;
	v4 a1;
	v4 a2;
	v4 a3;
	v4 a4;
	v4 a5;
	
	// secound egde
	v4 b0;
	v4 b1;
	v4 b2;
	v4 b3;
	v4 b4;
	v4 b5;

	// third egde
	v4 c0;
	v4 c1;
	v4 c2;
	v4 c3;
	v4 c4;
	v4 c5;
};

// this is a structure for a plane for the plane-ray test for the polygons
struct plane { // ASM
	// this is the normal (must be normalisized?)
	v4 nx;
	v4 ny;
	v4 nz;

	v4 d;
};



// this sets up a plane structure

// Plane : is a pointer at the plane data structure that should get filled
// p?    : is a point at the plane
// n?    : is the normal of that plane
void setup_plane(struct plane* Plane, float px, float py, float pz, float nx, float ny, float nz);

// this is a accelation structure for the Pluecker Polygon test (it saves the data of the ray(s))
struct ptpRay { // ASM
	Intrinsic::f4 r0; 
	Intrinsic::f4 r1;
	Intrinsic::f4 r2;
	Intrinsic::f4 r3;
	Intrinsic::f4 r4;
	Intrinsic::f4 r5;
};

struct rays2_ { // ASM
	// this is the hit information
	v4 deep; // is the hit deep

	v4l surface;        // is the id of the surface (polygon) that was hited
	v4ptr<struct BVH_Content*> contentPtr; // is the pointer to the content object that the surface is belonging to

	// this is the information of the ray
	//  for pluecker-test
	ptpRay ptrays;

	//  for the ray-plane test
	//  is the vector of the ray (unnormalisized)
	Intrinsic::f4 vx;
	Intrinsic::f4 vy;
	Intrinsic::f4 vz;
};

struct rays2 { // ASM
	 rays2_ rays[(32/4)*32]; // IMPORTANT< ASM< this need to be the first structure content for simplyfing the raypacket vs BVH_Content test > >

	//  for the ray-plane test
	//   pp? : primary rays - position ?
	// ASM< this is needed for asm, so don't change the offset of this >
	v4 ppx;
	v4 ppy;
	v4 ppz;
	v4l sse_mask_sign; // here only the sign-mask bits are seted, it is used to invert the signs
};

inline void pluecker_setup_ray2(struct ptpRay& Ray,
								const Intrinsic::f4& Px, const Intrinsic::f4& Py, const Intrinsic::f4& Pz,
								const Intrinsic::f4& Vx, const Intrinsic::f4& Vy, const Intrinsic::f4& Vz,
								const Intrinsic::f4& f4Minus
) {
	Ray.r0 = Intrinsic::mul(Vz, f4Minus);
	Ray.r1 = Intrinsic::load(Vy);
	Ray.r2 = Intrinsic::sub(Intrinsic::mul(Py, Vz), Intrinsic::mul(Vy, Pz));
	Ray.r3 = Intrinsic::mul(Vx, f4Minus);
	Ray.r4 = Intrinsic::sub(Intrinsic::mul(Px, Vy), Intrinsic::mul(Vx, Py));
	Ray.r5 = Intrinsic::sub(Intrinsic::mul(Px, Vz), Intrinsic::mul(Vx, Pz));
}

// this set ups the pluecker-test structure for a polygon
void pluecker_poly(struct ptp* Ptp,
				   float p1x, float p1y, float p1z,
				   float p2x, float p2y, float p2z,
				   float p3x, float p3y, float p3z
				   );

#endif