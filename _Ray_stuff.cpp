
#include "_Ray_stuff.h"

void setup_plane(struct plane* Plane, float px, float py, float pz, float nx, float ny, float nz) {
	Plane->nx._[0] = nx;
	Plane->nx._[1] = nx;
	Plane->nx._[2] = nx;
	Plane->nx._[3] = nx;

	Plane->ny._[0] = ny;
	Plane->ny._[1] = ny;
	Plane->ny._[2] = ny;
	Plane->ny._[3] = ny;

	Plane->nz._[0] = nz;
	Plane->nz._[1] = nz;
	Plane->nz._[2] = nz;
	Plane->nz._[3] = nz;
	
	Plane->d._[0] = px*nx + py*ny + pz*nz;
	Plane->d._[1] = px*nx + py*ny + pz*nz;
	Plane->d._[2] = px*nx + py*ny + pz*nz;
	Plane->d._[3] = px*nx + py*ny + pz*nz;
}

void pluecker_poly(struct ptp* Ptp,
				   float p1x, float p1y, float p1z,
				   float p2x, float p2y, float p2z,
				   float p3x, float p3y, float p3z
				   ) {
	float px, py, pz, qx, qy, qz;

	px = p1x;
	py = p1y;
	pz = p1z;

	qx = p2x;
	qy = p2y;
	qz = p2z;

	Ptp->a0._[0] = Ptp->a0._[1] = Ptp->a0._[2] = Ptp->a0._[3]   = px*qy - qx*py;
	Ptp->a1._[0] = Ptp->a1._[1] = Ptp->a1._[2] = Ptp->a1._[3]   = px*qz - qx*pz;
	Ptp->a2._[0] = Ptp->a2._[1] = Ptp->a2._[2] = Ptp->a2._[3]   = px    - qx;
	Ptp->a3._[0] = Ptp->a3._[1] = Ptp->a3._[2] = Ptp->a3._[3]   = py*qz - qy*pz;
	Ptp->a4._[0] = Ptp->a4._[1] = Ptp->a4._[2] = Ptp->a4._[3]   = pz    - qz;
	Ptp->a5._[0] = Ptp->a5._[1] = Ptp->a5._[2] = Ptp->a5._[3]   = qy    - py;
	

	px = p2x;
	py = p2y;
	pz = p2z;

	qx = p3x;
	qy = p3y;
	qz = p3z;

	Ptp->b0._[0] = Ptp->b0._[1] = Ptp->b0._[2] = Ptp->b0._[3]   = px*qy - qx*py;
	Ptp->b1._[0] = Ptp->b1._[1] = Ptp->b1._[2] = Ptp->b1._[3]   = px*qz - qx*pz;
	Ptp->b2._[0] = Ptp->b2._[1] = Ptp->b2._[2] = Ptp->b2._[3]   = px    - qx;
	Ptp->b3._[0] = Ptp->b3._[1] = Ptp->b3._[2] = Ptp->b3._[3]   = py*qz - qy*pz;
	Ptp->b4._[0] = Ptp->b4._[1] = Ptp->b4._[2] = Ptp->b4._[3]   = pz    - qz;
	Ptp->b5._[0] = Ptp->b5._[1] = Ptp->b5._[2] = Ptp->b5._[3]   = qy    - py;
	
	
	px = p3x;
	py = p3y;
	pz = p3z;

	qx = p1x;
	qy = p1y;
	qz = p1z;

	Ptp->c0._[0] = Ptp->c0._[1] = Ptp->c0._[2] = Ptp->c0._[3]   = px*qy - qx*py;
	Ptp->c1._[0] = Ptp->c1._[1] = Ptp->c1._[2] = Ptp->c1._[3]   = px*qz - qx*pz;
	Ptp->c2._[0] = Ptp->c2._[1] = Ptp->c2._[2] = Ptp->c2._[3]   = px    - qx;
	Ptp->c3._[0] = Ptp->c3._[1] = Ptp->c3._[2] = Ptp->c3._[3]   = py*qz - qy*pz;
	Ptp->c4._[0] = Ptp->c4._[1] = Ptp->c4._[2] = Ptp->c4._[3]   = pz    - qz;
	Ptp->c5._[0] = Ptp->c5._[1] = Ptp->c5._[2] = Ptp->c5._[3]   = qy    - py;
}