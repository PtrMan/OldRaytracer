// this implements the plücker-test algorithm

// NOTE< this algo is wrong? >

#include "general.h"
#include "pluecker0.h"

// this calculates the triagle data for the pluecker-test
// pt : is a pointer to the structure that should be actualisized
// p?! : is the ! coordinate of the ? egde of the polygon
void pluecker_calc_triangle(struct pluecker_test_* pt,
						   float p1x, float p1y, float p1z,
						   float p2x, float p2y, float p2z,
						   float p3x, float p3y, float p3z
						   ) {
	// A
	float Ax;
	float Ay;
	float Az;

	// B
	float Bx;
	float By;
	float Bz;
	
	Ax = p1x;
	Ay = p1y;
	Az = p1z;

	Bx = p2x;
	By = p2y;
	Bz = p2z;
	
	pt->v11._[0] = Ax*Bz - By*Az;
	pt->v11._[1] = Ax*Bz - By*Az;
	pt->v11._[2] = Ax*Bz - By*Az;
	pt->v11._[3] = Ax*Bz - By*Az;
	
	pt->v21._[0] = Ax*Bz - Bx*Az;
	pt->v21._[1] = Ax*Bz - Bx*Az;
	pt->v21._[2] = Ax*Bz - Bx*Az;
	pt->v21._[3] = Ax*Bz - Bx*Az;

	pt->v31._[0] = Ax*By - Bx*Ay;
	pt->v31._[1] = Ax*By - Bx*Ay;
	pt->v31._[2] = Ax*By - Bx*Ay;
	pt->v31._[3] = Ax*By - Bx*Ay;
	
	pt->v41._[0] = By - Ay;
	pt->v41._[1] = By - Ay;
	pt->v41._[2] = By - Ay;
	pt->v41._[3] = By - Ay;
	
	pt->v51._[0] = Az - Bz;
	pt->v51._[1] = Az - Bz;
	pt->v51._[2] = Az - Bz;
	pt->v51._[3] = Az - Bz;

	pt->v61._[0] = Ax - Bx;
	pt->v61._[1] = Ax - Bx;
	pt->v61._[2] = Ax - Bx;
	pt->v61._[3] = Ax - Bx;
	

	Ax = p2x;
	Ay = p2y;
	Az = p2z;

	Bx = p3x;
	By = p3y;
	Bz = p3z;
	
	pt->v12._[0] = Ax*Bz - By*Az;
	pt->v12._[1] = Ax*Bz - By*Az;
	pt->v12._[2] = Ax*Bz - By*Az;
	pt->v12._[3] = Ax*Bz - By*Az;
	
	pt->v22._[0] = Ax*Bz - Bx*Az;
	pt->v22._[1] = Ax*Bz - Bx*Az;
	pt->v22._[2] = Ax*Bz - Bx*Az;
	pt->v22._[3] = Ax*Bz - Bx*Az;

	pt->v32._[0] = Ax*By - Bx*Ay;
	pt->v32._[1] = Ax*By - Bx*Ay;
	pt->v32._[2] = Ax*By - Bx*Ay;
	pt->v32._[3] = Ax*By - Bx*Ay;
	
	pt->v42._[0] = By - Ay;
	pt->v42._[1] = By - Ay;
	pt->v42._[2] = By - Ay;
	pt->v42._[3] = By - Ay;
	
	pt->v52._[0] = Az - Bz;
	pt->v52._[1] = Az - Bz;
	pt->v52._[2] = Az - Bz;
	pt->v52._[3] = Az - Bz;

	pt->v62._[0] = Ax - Bx;
	pt->v62._[1] = Ax - Bx;
	pt->v62._[2] = Ax - Bx;
	pt->v62._[3] = Ax - Bx;
	

	Ax = p3x;
	Ay = p3y;
	Az = p3z;

	Bx = p1x;
	By = p1y;
	Bz = p1z;
	
	pt->v13._[0] = Ax*Bz - By*Az;
	pt->v13._[1] = Ax*Bz - By*Az;
	pt->v13._[2] = Ax*Bz - By*Az;
	pt->v13._[3] = Ax*Bz - By*Az;
	
	pt->v23._[0] = Ax*Bz - Bx*Az;
	pt->v23._[1] = Ax*Bz - Bx*Az;
	pt->v23._[2] = Ax*Bz - Bx*Az;
	pt->v23._[3] = Ax*Bz - Bx*Az;

	pt->v33._[0] = Ax*By - Bx*Ay;
	pt->v33._[1] = Ax*By - Bx*Ay;
	pt->v33._[2] = Ax*By - Bx*Ay;
	pt->v33._[3] = Ax*By - Bx*Ay;
	
	pt->v43._[0] = By - Ay;
	pt->v43._[1] = By - Ay;
	pt->v43._[2] = By - Ay;
	pt->v43._[3] = By - Ay;
	
	pt->v53._[0] = Az - Bz;
	pt->v53._[1] = Az - Bz;
	pt->v53._[2] = Az - Bz;
	pt->v53._[3] = Az - Bz;

	pt->v63._[0] = Ax - Bx;
	pt->v63._[1] = Ax - Bx;
	pt->v63._[2] = Ax - Bx;
	pt->v63._[3] = Ax - Bx;	
}

// this updates the ray data for the pluecker-test
// pt : is a pointer to the structure that should be actualisized
// O?  : ray orgin     ( ? coordinate )
// D?  : ray direction ( ? coordinate )
void pluecker_calc_ray(struct pluecker_test_* pt,
					   struct v4 Ox, struct v4 Oy, struct v4 Oz,
					   struct v4 Dx, struct v4 Dy, struct v4 Dz
					   ) {
	pt->Di._[0] = Dx._[0];
	pt->Di._[1] = Dx._[1];
	pt->Di._[2] = Dx._[2];
	pt->Di._[3] = Dx._[3];

	pt->Dj._[0] = Dy._[0];
	pt->Dj._[1] = Dy._[1];
	pt->Dj._[2] = Dy._[2];
	pt->Dj._[3] = Dy._[3];
	
	pt->Dk._[0] = Dz._[0];
	pt->Dk._[1] = Dz._[1];
	pt->Dk._[2] = Dz._[2];
	pt->Dk._[3] = Dz._[3];
	

	pt->r1._[0] = (Ox._[0]*Dz._[0] - Dx._[0]*Oz._[0]);
	pt->r1._[1] = (Ox._[1]*Dz._[1] - Dx._[1]*Oz._[1]);
	pt->r1._[2] = (Ox._[2]*Dz._[2] - Dx._[2]*Oz._[2]);
	pt->r1._[3] = (Ox._[3]*Dz._[3] - Dx._[3]*Oz._[3]);
	
	pt->r2._[0] = (Ox._[0]*Dy._[0] - Dx._[0]*Oy._[0]);
	pt->r2._[1] = (Ox._[1]*Dy._[1] - Dx._[1]*Oy._[1]);
	pt->r2._[2] = (Ox._[2]*Dy._[2] - Dx._[2]*Oy._[2]);
	pt->r2._[3] = (Ox._[3]*Dy._[3] - Dx._[3]*Oy._[3]);
	
	pt->r3._[0] = (Oy._[0]*Dz._[0] - Dy._[0]*Oz._[0]);
	pt->r3._[1] = (Oy._[1]*Dz._[1] - Dy._[1]*Oz._[1]);
	pt->r3._[2] = (Oy._[2]*Dz._[2] - Dy._[2]*Oz._[2]);
	pt->r3._[3] = (Oy._[3]*Dz._[3] - Dy._[3]*Oz._[3]);
}
