/******************************************************************************

  This source code accompanies the Journal of Graphics Tools paper:

  "Fast Ray / Axis-Aligned Bounding Box Overlap Tests using Ray Slopes" 
  by Martin Eisemann, Thorsten Grosch, Stefan Müller and Marcus Magnor
  Computer Graphics Lab, TU Braunschweig, Germany and
  University of Koblenz-Landau, Germany
  
  This source code is public domain, but please mention us if you use it.

******************************************************************************/
#ifndef H_AABB2_SLOPE_ORGINAL
#define H_AABB2_SLOPE_ORGINAL

#include "aabb2_ray_orginal.h"
#include "aabb2_aabbbox_orginal.h"

bool aabb2_slope_orginal(aabb2_ray_orginal *r, aabb2_aabox_orginal *b);

#endif
