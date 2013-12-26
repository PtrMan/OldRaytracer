#ifndef _H_BVH
#define _H_BVH

#include "_Ray_Stuff.h"
#include "_misc_stuff.h"

//#include "aabb.h"
#include "aabb2_aabbbox_orginal.h"

#include "vector3.h"


// this is the size of the polygon array of the BVH_Content structure
const unsigned long BVH_CONTENT_POLYGONS_SIZE = 64;
const unsigned long BVH_NODE_CONTENTSIZE = 32;

// this is the data of one polygon
struct poly { // ASM
	ptp ptp;
	plane plane;
};

// this are informations needed to calculate the Barycentric Coordinates of the Polygon
struct polyBaryCoord {
	//                                       ->         ->
	// on xy projected position of A and c = AB and b = AC for a fast calculation of the barycentric coordinates >
	float Ax;
	float Ay;

	float cX;
	float cY;

	float bX;
	float bY;

	float mul; // this is the precalculated 1.0f/(bX*cY - bY*cX) term

	// this are help values for this polygon (the normals of the three points) because i think that it speeds this stuff up a bit
	Vector3 n1;
	Vector3 n2;
	Vector3 n3;

	// here we can also put in other stuff for example the texture coordinates
};

struct BVH_Content {
	// this is a array with all polygons inside this Content-Node
	// IMPORTANT< ASM< this need to be the first structure content for simplyfing the raypacket vs BVH_Content test > >
	struct poly polys[BVH_CONTENT_POLYGONS_SIZE];

	// ASM< this is needed for asm, so don't change the offset of this >
	unsigned long polysc; // is the count of the polygons inside the array
	
	// this are aditionaly informations for the polygons [barycentric Coordinates)(its not collision-test essential, because of this i have move this here)
	struct polyBaryCoord polysBaryCoords[BVH_CONTENT_POLYGONS_SIZE];

	// the data for the sphere
	float sposx;
	float sposy;
	float sposz;
	float sr;    // radius of the sphere

	// this is the boudning box (seted up in the tranformToWorld member and used in the tree-building algorithm)
	/*float bb_minx;
	float bb_miny;
	float bb_minz;
	float bb_maxx;
	float bb_maxy;
	float bb_maxz;*/
	aabb2_aabox_orginal aabb;
};


struct BVH_Node {
	//aabbBox aabb;                 // this is the AABB of the BVH node
	aabb2_aabox_orginal aabb;
	
	frustrumSphere aabb_frustrum; // this is the frustrum sphere of the AABB of the BVH node
	
	BVH_Node* childrens[2]; // this are the childrens of the bvh
	                        // can be 0 if children is not "supported"
	
	BVH_Content* content[BVH_NODE_CONTENTSIZE]; // is the content
	                                   // first null pointer represents the end of the list

	unsigned long contentc; // is the count of the content

	bool isLeaf; // is it a leaf
	             // true  : childrens are invalid, content is valid
	             // false : childrens are valid  , content is invalid
};

void BVH_Node_init(struct BVH_Node* Node, float aabbminx, float aabbminy, float aabbminz, float aabbmaxx, float aabbmaxy, float aabbmaxz);

// this tries to add a BVH_Content Content into that Node
// it throws ArrayOverflow() if the array is too full
void BVH_Node_addContent(struct BVH_Node* Node, const struct BVH_Content* Content);

#endif