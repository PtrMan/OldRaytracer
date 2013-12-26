
#include "bvh.h"
#include "vector_cStyle.h"

// this initialisize all fields of the BVH_Node structure
void BVH_Node_init(struct BVH_Node* Node, float aabbminx, float aabbminy, float aabbminz, float aabbmaxx, float aabbmaxy, float aabbmaxz) {
	// set first content to NULL
	//Node->content[0] = NULL;
	
	Node->contentc = 0; // we have nothing inside
		
	// don't have any childrens
	//Node->childrens[0] = NULL;
	//Node->childrens[1] = NULL;

	// set as a leaf
	Node->isLeaf = true;
	
	// set the aabb
	Node->aabb.x0 = aabbminx;
	Node->aabb.y0 = aabbminy;
	Node->aabb.z0 = aabbminz;

	Node->aabb.x1 = aabbmaxx;
	Node->aabb.y1 = aabbmaxy;
	Node->aabb.z1 = aabbmaxz;

	// set the frustrum-sphere for the aabb
	Node->aabb_frustrum.posx = (aabbminx + aabbmaxx) / 2.0f;
	Node->aabb_frustrum.posy = (aabbminy + aabbmaxy) / 2.0f;
	Node->aabb_frustrum.posz = (aabbminz + aabbmaxz) / 2.0f;
		
	// calculate the radius of the frustrum sphere for the aabb
	Node->aabb_frustrum.radius = vector_length(Node->aabb_frustrum.posx - aabbmaxx, Node->aabb_frustrum.posy - aabbmaxy, Node->aabb_frustrum.posz - aabbmaxz);
}

void BVH_Node_addContent(struct BVH_Node* Node, const struct BVH_Content* Content) {
	// check if the array do have a free place
	if( Node->contentc == BVH_NODE_CONTENTSIZE ) {
		throw class ArrayOverflow();
	}

	// append the content to the Array
	Node->content[Node->contentc] = const_cast<BVH_Content*>( Content );

	Node->contentc++;
}

