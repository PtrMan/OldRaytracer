// this are tree-building related methods

#include "RayEngine.h"
#include "bvh.h"
#include "boundingBox.h"

void RayEngine::rebuildTree2(BVH_Node* IntoNode, BVH_Content** Content, unsigned long ContentC) {
	// TODO

	// first we search the "biggest" dimension (the dimension in that the elements to have the most "width")
	BoundingBox bb;

	for( unsigned long contentI = 0; contentI < ContentC; contentI++ ) {
		const BVH_Content* bvhcontent = Content[contentI];
		
		bb.expand(bvhcontent->aabb.x0, bvhcontent->aabb.y0, bvhcontent->aabb.z0, bvhcontent->aabb.x1, bvhcontent->aabb.y1, bvhcontent->aabb.z1);
	}
		
	float bb_sizex = bb.maxx - bb.minx;
	float bb_sizey = bb.maxy - bb.miny;
	float bb_sizez = bb.maxz - bb.minz;
		
	enum dimension {
		X,
		Y,
		Z
	};

	dimension dim = X;
	float dim_value = bb_sizex;

	if( bb_sizey > dim_value ) {
		dim = Y;
		dim_value = bb_sizey;
	}

	if( bb_sizez > dim_value ) {
		dim = Z;
		dim_value = bb_sizez;
	}

	// calculate the split-position ("plane")
	float splitposition;
	if( dim == X ) {
		splitposition = bb.minx + bb_sizex * 0.5f;
	}
	else if( dim == Y ) {
		splitposition = bb.miny + bb_sizey * 0.5f;
	}
	else if( dim == Z ) {
		splitposition = bb.minz + bb_sizez * 0.5f;
	}






	if( ContentC > BVH_NODE_CONTENTSIZE ) {
		// split all BVH_Content things to the 2 sides (low side and high side)
		// TODO< rebuild this array thing more clever >
		BVH_Content* content_low[5000];
		unsigned long content_lowC = 0; // is the counter

		BVH_Content* content_high[5000];
		unsigned long content_highC = 0; // is the couter

		for( unsigned long contentI = 0; contentI < ContentC; contentI++ ) {
			BVH_Content* bvhcontent = const_cast<BVH_Content*>( Content[contentI] );
				
			if( dim == X ) {
				// all is splited along the X dimension
				if( bvhcontent->sposx > splitposition ) {
					// put it in the high array
						
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
			else if( dim == Y ) {
				// all is splited along the Y dimension
				if( bvhcontent->sposy > splitposition ) {
					// put it in the high array
						
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
			else {
				// all is splited along the Z dimension
				if( bvhcontent->sposz > splitposition ) {
					// put it in the high array
					
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
		}

		// now all is splited into their containers (low and high), now we need to do this recursivly
		// and put the result into the tree
			
		BVH_Node_init(IntoNode, bb.minx, bb.miny, bb.minz, bb.maxx, bb.maxy, bb.maxz);
		IntoNode->isLeaf = false;
			
		// we allocate the two children-nodes
		IntoNode->childrens[0] = nodeAllocator.allocElement();
		IntoNode->childrens[1] = nodeAllocator.allocElement();
			
		rebuildTree2(IntoNode->childrens[0], reinterpret_cast<BVH_Content**>( &content_low ), content_lowC);
		rebuildTree2(IntoNode->childrens[1], reinterpret_cast<BVH_Content**>( &content_high ), content_highC);
	}
	else {
		// we do hang it all in one node with a content node

		BVH_Node_init(IntoNode, bb.minx, bb.miny, bb.minz, bb.maxx, bb.maxy, bb.maxz);
			
		for( unsigned long contentI = 0; contentI < ContentC; contentI++ ) {
			const BVH_Content* bvhcontent = Content[contentI];

			BVH_Node_addContent(IntoNode, bvhcontent);
		}
	}
}

void RayEngine::rebuildTree() {
	// free all allocated elements
	nodeAllocator.reset();
		
	// first we search the "biggest" dimension (the dimension in that the elements to have the most "width")
	BoundingBox bb;

	for( unsigned long contentI = 0; contentI < contentAllocator.getSize(); contentI++ ) {
		const BVH_Content* bvhcontent = contentAllocator[contentI];
		
		bb.expand(bvhcontent->aabb.x0, bvhcontent->aabb.y0, bvhcontent->aabb.z0, bvhcontent->aabb.x1, bvhcontent->aabb.y1, bvhcontent->aabb.z1);
	}
		
	float bb_sizex = bb.maxx - bb.minx;
	float bb_sizey = bb.maxy - bb.miny;
	float bb_sizez = bb.maxz - bb.minz;
		
	enum dimension {
		X,
		Y,
		Z
	};

	dimension dim = X;
	float dim_value = bb_sizex;

	if( bb_sizey > dim_value ) {
		dim = Y;
		dim_value = bb_sizey;
	}

	if( bb_sizez > dim_value ) {
		dim = Z;
		dim_value = bb_sizez;
	}

	// calculate the split-position ("plane")
	float splitposition;
	if( dim == X ) {
		splitposition = bb.minx + bb_sizex * 0.5f;
	}
	else if( dim == Y ) {
		splitposition = bb.miny + bb_sizey * 0.5f;
	}
	else if( dim == Z ) {
		splitposition = bb.minz + bb_sizez * 0.5f;
	}






	if( contentAllocator.getSize() > BVH_NODE_CONTENTSIZE ) {
		// split all BVH_Content things to the 2 sides (low side and high side)
		// TODO< rebuild this array thing more clever >
		BVH_Content* content_low[5000];
		unsigned long content_lowC = 0; // is the counter

		BVH_Content* content_high[5000];
		unsigned long content_highC = 0; // is the couter

		for( unsigned long contentI = 0; contentI < contentAllocator.getSize(); contentI++ ) {
			BVH_Content* bvhcontent = const_cast<BVH_Content*>( contentAllocator[contentI] );
				
			if( dim == X ) {
				// all is splited along the X dimension
				if( bvhcontent->sposx > splitposition ) {
					// put it in the high array
						
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
			else if( dim == Y ) {
				// all is splited along the Y dimension
				if( bvhcontent->sposy > splitposition ) {
					// put it in the high array
						
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
			else {
				// all is splited along the Z dimension
				if( bvhcontent->sposz > splitposition ) {
					// put it in the high array
						
					content_high[content_highC] = bvhcontent;
					content_highC++;
					if( content_highC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
				else {
					// put it in the low array

					content_low[content_lowC] = bvhcontent;
					content_lowC++;
					if( content_lowC > 5000 ) {
						throw class ArrayOverflow();
					}
				}
			}
		}

		// now all is splited into their containers (low and high), now we need to do this recursivly
		// and put the result into the tree
			
		// we allocate a new "master" node
		BVH_Node* nRootNode = nodeAllocator.allocElement();
		BVH_Node_init(nRootNode, bb.minx, bb.miny, bb.minz, bb.maxx, bb.maxy, bb.maxz);
		nRootNode->isLeaf = false;
		root = nRootNode;

		// we allocate the two children-nodes
		nRootNode->childrens[0] = nodeAllocator.allocElement();
		nRootNode->childrens[1] = nodeAllocator.allocElement();

		rebuildTree2(nRootNode->childrens[0], reinterpret_cast<BVH_Content**>( &content_low ), content_lowC);
		rebuildTree2(nRootNode->childrens[1], reinterpret_cast<BVH_Content**>( &content_high ), content_highC);
	}
	else {
		// we do hang it all in one "master" node with a content node

		BVH_Node* nRootNode = nodeAllocator.allocElement();
		BVH_Node_init(nRootNode, bb.minx, bb.miny, bb.minz, bb.maxx, bb.maxy, bb.maxz);
			
		for( unsigned long contentI = 0; contentI < contentAllocator.getSize(); contentI++ ) {
			const BVH_Content* bvhcontent = contentAllocator[contentI];

			BVH_Node_addContent(nRootNode, bvhcontent);
		}

		root = nRootNode;
	}
}
