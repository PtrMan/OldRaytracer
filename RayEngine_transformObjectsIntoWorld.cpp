#include <math.h>

#include "RayEngine.h"
#include "matrix.h"
#include "bvh.h"
#include "vector3.h"
#include "_Ray_stuff.h"
#include "vector_cStyle.h"

void RayEngine::transformObjectsIntoWorld() {
	contentAllocator.reset();
		
	// transform all objects into worldspace
	// just go througth the objects
	for(list<Object*>::iterator actualObject = objects.begin(); actualObject != objects.end(); actualObject++ ) {
		matrix_44 mat; // this is our transformation matrix
		matrix_44 mat2; // is our 2nd transformation matrix
		matrix_44 matM; // is our "manipulation" matrix
		// TODO< build in the translate and rotation and later on the scale matrices >
		
		/*
		// fill the matrix with the "identity matrix"
		mat._[0][0] = 1.0f;
		mat._[1][0] = 0.0f;
		mat._[2][0] = 0.0f;
		mat._[3][0] = 0.0f;
		
		mat._[0][1] = 0.0f;
		mat._[1][1] = 1.0f;
		mat._[2][1] = 0.0f;
		mat._[3][1] = 0.0f;
		
		mat._[0][2] = 0.0f;
		mat._[1][2] = 0.0f;
		mat._[2][2] = 1.0f;
		mat._[3][2] = 0.0f;
		
		mat._[0][3] = 0.0f;
		mat._[1][3] = 0.0f;
		mat._[2][3] = 0.0f;
		mat._[3][3] = 1.0f;
		*/
		// fill in the rotation(x) matrix
		mat._[0][0] = 1.0f;
		mat._[1][0] = 0.0f;
		mat._[2][0] = 0.0f;
		mat._[3][0] = 0.0f;

		mat._[0][1] = 0.0f;
		mat._[1][1] =  cosf((*actualObject)->rotation.x); // TODO< translate this from degree into rad >
		mat._[2][1] = -sinf((*actualObject)->rotation.x); // DITTO
		mat._[3][1] = 0.0f;

		mat._[0][2] = 0.0f;
		mat._[1][2] =  sinf((*actualObject)->rotation.x); // DITTO
		mat._[2][2] =  cosf((*actualObject)->rotation.x); // DITTO
		mat._[3][2] = 0.0f;

		mat._[0][3] = 0.0f;
		mat._[1][3] = 0.0f;
		mat._[2][3] = 0.0f;
		mat._[3][3] = 1.0f;
			
		// fill in the Ration(y) matrix
		matM._[0][0] = cosf((*actualObject)->rotation.y); // DITTO
		matM._[1][0] = 0.0f;
		matM._[2][0] = sinf((*actualObject)->rotation.y); // DITTO
		matM._[3][0] = 0.0f;

		matM._[0][1] = 0.0f;
		matM._[1][1] = 1.0f;
		matM._[2][1] = 0.0f;
		matM._[3][1] = 0.0f;

		matM._[0][2] = -sinf((*actualObject)->rotation.y); // DITTO
		matM._[1][2] = 0.0f;
		matM._[2][2] =  cos((*actualObject)->rotation.y); // DITTO
		matM._[3][2] = 0.0f;

		matM._[0][3] = 0.0f;
		matM._[1][3] = 0.0f;
		matM._[2][3] = 0.0f;
		matM._[3][3] = 1.0f;

		// multiplicate it
		matrix_mul(mat, matM, mat2);
			
		// fill in the rotation(z) matrix
		matM._[0][0] =  cosf((*actualObject)->rotation.z); // DITTO
		matM._[1][0] = -sinf((*actualObject)->rotation.z); // DITTO
		matM._[2][0] = 0.0f;
		matM._[3][0] = 0.0f;

		matM._[0][1] =  sin((*actualObject)->rotation.z); // DITTO
		matM._[1][1] =  cos((*actualObject)->rotation.z); // DITTO
		matM._[2][1] = 0.0f;
		matM._[3][1] = 0.0f;

		matM._[0][2] = 0.0f;
		matM._[1][2] = 0.0f;
		matM._[2][2] = 1.0f;
		matM._[3][2] = 0.0f;

		matM._[0][3] = 0.0f;
		matM._[1][3] = 0.0f;
		matM._[2][3] = 0.0f;
		matM._[3][3] = 1.0f;

		// multiplicate it
		matrix_mul(mat2, matM, mat);

		// fill the move matrix
		matrix_fill(matM,
		          1.f, 0.f, 0.f, (*actualObject)->position.x,
		          0.f, 1.f, 0.f, (*actualObject)->position.y,
		          0.f, 0.f, 1.f, (*actualObject)->position.z,
		          0.f, 0.f, 0.f, 1.f
		);
			
		matrix_mul(mat, matM, mat2);

		matrix_move(mat2, mat);
			
			
		// reset all normals of the verticles
		for( unsigned long verticleI = 0; verticleI < (*actualObject)->mesh->verticles.getCount(); verticleI++ ) {
			(*actualObject)->mesh->verticles.elements[verticleI]->normal.x = 0.0f;
			(*actualObject)->mesh->verticles.elements[verticleI]->normal.y = 0.0f;
			(*actualObject)->mesh->verticles.elements[verticleI]->normal.z = 0.0f;
		}

		// this is an array with the pointers to the single bvhcontent structures of each iteration
		// we use it later to fill in the three normals of the polygons
		// i think 1000 is enougth
		BVH_Content* bvhcPtrs[1000];

		// just go througth the bounding-container in the meshes of the object and transform and calculate all things
		for( unsigned long bsci = 0; bsci < (*actualObject)->mesh->BScontainer.getCount(); bsci++ ) {

			// allocate a new BVH_Content structure
			BVH_Content* bvhcontent = contentAllocator.allocElement();
				
			// save the pointer to the BVH_Content structure
			if( bsci > 1000 ) {
				throw class ArrayOverflow();
			}
			bvhcPtrs[bsci] = bvhcontent;

			// this is the bounding box of the bvh_Content object
			Vector3 bb_min( 9e6f,  9e6f,  9e6f);
			Vector3 bb_max(-9e6f, -9e6f, -9e6f);
				
			// transform the position of the bounding-"sphere"
			matrix_mul(mat,
				           
			           (*actualObject)->mesh->BScontainer.elements[bsci]->sphere_position.x,
					   (*actualObject)->mesh->BScontainer.elements[bsci]->sphere_position.y,
					   (*actualObject)->mesh->BScontainer.elements[bsci]->sphere_position.z,
				           
					   bvhcontent->sposx,
					   bvhcontent->sposy,
					   bvhcontent->sposz
			);

			// TODO< scale and calculate the radius of the "bounding sphere" >

			// we go througth the polygons and translate them and calculate all pluecker and plane things and we also calculate the aabb of that thing
				
			//  but first we test if we have enougth space for it
			if( (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.getCount() > BVH_CONTENT_POLYGONS_SIZE ) {
				throw class ArrayOverflow();
			}
				
			for( unsigned long polyi = 0; polyi < (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.getCount(); polyi++ ) {
				unsigned long polygon = (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.elements[polyi];

				unsigned long v1 = (*actualObject)->mesh->triangles.elements[polygon]->v1;
				unsigned long v2 = (*actualObject)->mesh->triangles.elements[polygon]->v2;
				unsigned long v3 = (*actualObject)->mesh->triangles.elements[polygon]->v3;

				// get the position of the verticles and transform them
					
				// are the tranformed positions of the verticles
				Vector3 v1p;
				Vector3 v2p;
				Vector3 v3p;

				matrix_mul(mat,
					           
				           (*actualObject)->mesh->verticles.elements[v1]->position.x,
				           (*actualObject)->mesh->verticles.elements[v1]->position.y,
						   (*actualObject)->mesh->verticles.elements[v1]->position.z,
					
				           v1p.x,
						   v1p.y,
						   v1p.z
				);

				matrix_mul(mat,
					           
				           (*actualObject)->mesh->verticles.elements[v2]->position.x,
				           (*actualObject)->mesh->verticles.elements[v2]->position.y,
				           (*actualObject)->mesh->verticles.elements[v2]->position.z,
					
				           v2p.x,
						   v2p.y,
						   v2p.z
				);

				matrix_mul(mat,
					           
				           (*actualObject)->mesh->verticles.elements[v3]->position.x,
				           (*actualObject)->mesh->verticles.elements[v3]->position.y,
				           (*actualObject)->mesh->verticles.elements[v3]->position.z,
					
				           v3p.x,
						   v3p.y,
						   v3p.z
				);

				// expand the bounding box with that values
				if( bb_min.x > v1p.x ) {
					bb_min.x = v1p.x;
				}
				if( bb_min.y > v1p.y ) {
					bb_min.y = v1p.y;
				}
				if( bb_min.z > v1p.z ) {
					bb_min.z = v1p.z;
				}
					
				if( bb_max.x < v1p.x ) {
					bb_max.x = v1p.x;
				}
				if( bb_max.y < v1p.y ) {
					bb_max.y = v1p.y;
				}
				if( bb_max.z < v1p.z ) {
					bb_max.z = v1p.z;
				}


				if( bb_min.x > v2p.x ) {
					bb_min.x = v2p.x;
				}
				if( bb_min.y > v2p.y ) {
					bb_min.y = v2p.y;
				}
				if( bb_min.z > v2p.z ) {
					bb_min.z = v2p.z;
				}
					
				if( bb_max.x < v2p.x ) {
					bb_max.x = v2p.x;
				}
				if( bb_max.y < v2p.y ) {
					bb_max.y = v2p.y;
				}
				if( bb_max.z < v2p.z ) {
					bb_max.z = v2p.z;
				}


				if( bb_min.x > v3p.x ) {
					bb_min.x = v3p.x;
				}
				if( bb_min.y > v3p.y ) {
					bb_min.y = v3p.y;
				}
				if( bb_min.z > v3p.z ) {
					bb_min.z = v3p.z;
				}
					
				if( bb_max.x < v3p.x ) {
					bb_max.x = v3p.x;
				}
				if( bb_max.y < v3p.y ) {
					bb_max.y = v3p.y;
				}
				if( bb_max.z < v3p.z ) {
					bb_max.z = v3p.z;
				}

				// calculate the pluecker things
				pluecker_poly(&(bvhcontent->polys[polyi].ptp),
			                  v1p.x, v1p.y, v1p.z,
			                  v2p.x, v2p.y, v2p.z,
			                  v3p.x, v3p.y, v3p.z
				);


				// set up the plane things
				//  calculate the normal of the plane (and normalisize it)
				Vector3 planeVector; // is the vector of the plane

				vector_crossproduct(v1p.x - v2p.x, v1p.y - v2p.y, v1p.z - v2p.z,
				                    v1p.x - v3p.x, v1p.y - v3p.y, v1p.z - v3p.z,
				                    planeVector.x, planeVector.y, planeVector.z
				);

				vector_normalisize(planeVector.x, planeVector.y, planeVector.z);

				//  set it up
				setup_plane(&(bvhcontent->polys[polyi].plane),
				            v1p.x, v1p.y, v1p.z,
							planeVector.x, planeVector.y, planeVector.z
				);

				// add this normal to the average normal of the verticles
				(*actualObject)->mesh->verticles.elements[v1]->normal += planeVector;
				(*actualObject)->mesh->verticles.elements[v2]->normal += planeVector;
				(*actualObject)->mesh->verticles.elements[v3]->normal += planeVector;

				// calculate the stuff for the barycentric coordinates (used for the normal/texture coordinate interpolarisation)
				bvhcontent->polysBaryCoords[polyi].Ax = v1p.x;
				bvhcontent->polysBaryCoords[polyi].Ay = v1p.y;
				bvhcontent->polysBaryCoords[polyi].bX = v3p.x - v1p.x;
				bvhcontent->polysBaryCoords[polyi].bY = v3p.y - v1p.y;
				bvhcontent->polysBaryCoords[polyi].cX = v2p.x - v1p.x;
				bvhcontent->polysBaryCoords[polyi].cY = v2p.y - v1p.y;
				bvhcontent->polysBaryCoords[polyi].mul = 1.0f / (bvhcontent->polysBaryCoords[polyi].bX*bvhcontent->polysBaryCoords[polyi].cY - bvhcontent->polysBaryCoords[polyi].bY*bvhcontent->polysBaryCoords[polyi].cX);
			}
				
			// save the bounding box of the BVH_Content object
			bvhcontent->aabb.x0 = bb_min.x;
			bvhcontent->aabb.y0 = bb_min.y;
			bvhcontent->aabb.z0 = bb_min.z;
				
			bvhcontent->aabb.x1 = bb_max.x;
			bvhcontent->aabb.y1 = bb_max.y;
			bvhcontent->aabb.z1 = bb_max.z;
				
			// save the count of the polygons
			bvhcontent->polysc = (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.getCount();

			// set the sphere radius
			// WARNING< if we scale it we need to scale the shere radius too >
			bvhcontent->sr = (*actualObject)->mesh->BScontainer.elements[bsci]->sphere_radius;
		}

		// normalisize all normals of the verticles
		for( unsigned long verticleI = 0; verticleI < (*actualObject)->mesh->verticles.getCount(); verticleI++ ) {
			vector_normalisize(
				(*actualObject)->mesh->verticles.elements[verticleI]->normal.x,
				(*actualObject)->mesh->verticles.elements[verticleI]->normal.y,
				(*actualObject)->mesh->verticles.elements[verticleI]->normal.z
			);
		}

		// fill in all normals of the polygons in the bvh-Content
		// it is for a faster calculation in the mainloop for the normals
		for( unsigned long bsci = 0; bsci < (*actualObject)->mesh->BScontainer.getCount(); bsci++ ) {
			BVH_Content* bvhcontent = bvhcPtrs[bsci];

			for( unsigned long polyi = 0; polyi < (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.getCount(); polyi++ ) {
				unsigned long polygon = (*actualObject)->mesh->BScontainer.elements[bsci]->polygons.elements[polyi];

				unsigned long v1 = (*actualObject)->mesh->triangles.elements[polygon]->v1;
				unsigned long v2 = (*actualObject)->mesh->triangles.elements[polygon]->v2;
				unsigned long v3 = (*actualObject)->mesh->triangles.elements[polygon]->v3;

				// get the normals of the verticles
				Vector3 n1 = (*actualObject)->mesh->verticles.elements[v1]->normal;
				Vector3 n2 = (*actualObject)->mesh->verticles.elements[v2]->normal;
				Vector3 n3 = (*actualObject)->mesh->verticles.elements[v3]->normal;

				// set the normals of the three verticles of the polygon for the barycentric coordinates related calculations
				bvhcontent->polysBaryCoords[polyi].n1 = n1;
				bvhcontent->polysBaryCoords[polyi].n2 = n2;
				bvhcontent->polysBaryCoords[polyi].n3 = n3;
			}
		}
	}
}
