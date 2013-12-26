/******************************************************************************

  This source code accompanies the Journal of Graphics Tools paper:

  "Fast Ray-Axis-Aligned Bounding Box Overlap Tests using Ray Slopes" 
  by Martin Eisemann, Thorsten Grosch, Stefan M�ller and Marcus Magnor
  Computer Graphics Lab, TU Braunschweig, Germany and
  University of Koblenz-Landau, Germany

  Parts of this code are taken from
  "Fast Ray-Axis Aligned Bounding Box Overlap Tests With Pluecker Coordinates" 
  by Jeffrey Mahovsky and Brian Wyvill
  Department of Computer Science, University of Calgary

  This source code is public domain, but please mention us if you use it.

******************************************************************************/
#ifndef H_AABB2_AABBBOX_ORGINAL
#define H_AABB2_AABBBOX_ORGINAL

// axis-aligned bounding box structure

struct aabb2_aabox_orginal
{
	float x0, y0, z0, x1, y1, z1;
};

void aabb2_make_aabox_orginal(float x0, float y0, float z0, float x1, float y1, float z1, aabb2_aabox_orginal *a);

#endif
