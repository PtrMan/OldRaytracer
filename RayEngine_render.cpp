#include "RayEngine.h"
#include "intrinsic.h"
#include "_Ray_stuff.h"
#include "_misc_stuff.h"
#include "inline.h"

#include "aabb2_ray_orginal.h"
#include "aabb2_slope_orginal.h"

// for debugging
#include <iostream>
using namespace std;

// this is used for debugging the Assembly code
void debug(unsigned int id, void* dataPtr) {
	cout << id << " ";
		
	std::hex(cout);
	cout << (unsigned int)dataPtr << endl;
	std::dec(cout);

	// id
	//    0 : info : 2nd parameter
	//    1 : new polytest : pointer to polygon
	//    2 : next poly : -.-
	//    3 : next ray-packet : -.-
	//    4 : end
	//    5 : ray-setup
	if( id == 4 ) {
		unsigned int a = 0;
	}
}

void RayEngine::render(SDL_Surface* Screen) {
	global Global; // is the "local" "global" data (cos on the stack its faster and more practical than class-object wide u know...)
	perThread PerThread; // per thread structure // TODO< do align it for sse things >
	
	NSTimer<true> timer_renderAll; // timer for the whoole time of the rendering
	NSTimer<true> timer_call_clFinish; // timer for the clFinish syscall

	// check all conditions
	if( !resSetedUp ) {
		throw class InvalidResolution();
	}

	if( camera == NULL ) {
		throw class NoCamera();
	}

	// set up the global structure
	float xpixeld = 1.0f/float(resolution.x);
	float ypixeld = 1.0f/float(resolution.y); // is the "distance" of a pixel to pixel (full wide is 1.0)

	camera->recalculate();

	Global.cameraXPos = camera->position.x;
	Global.cameraYPos = camera->position.y;
	Global.cameraZPos = camera->position.z;

	// the x span vector
	Global.cameraXX = -2.0f * camera->cameraX.x;
	Global.cameraXY = -2.0f * camera->cameraX.y;
	Global.cameraXZ = -2.0f * camera->cameraX.z;
		
	// the y span vector
	Global.cameraYX = -2.0f * camera->cameraY.x;
	Global.cameraYY = -2.0f * camera->cameraY.y;
	Global.cameraYZ = -2.0f * camera->cameraY.z;
		
	Global.cameraBeginX = camera->look.x + camera->cameraX.x + camera->cameraY.x;
	Global.cameraBeginY = camera->look.y + camera->cameraX.y + camera->cameraY.y;
	Global.cameraBeginZ = camera->look.z + camera->cameraX.z + camera->cameraY.z;
		
	// calculate the intile data
	// its for 8x8 packets
	Global.inTileCameraRelXX = Global.cameraXX * xpixeld * /*(TILESIZE_X/*/32;
	Global.inTileCameraRelXY = Global.cameraXY * xpixeld * /*(TILESIZE_X/*/32;
	Global.inTileCameraRelXZ = Global.cameraXZ * xpixeld * /*(TILESIZE_X/*/32;
	
	Global.inTileCameraRelYX = Global.cameraYX * ypixeld * /*(TILESIZE_X/*/32;
	Global.inTileCameraRelYY = Global.cameraYY * ypixeld * /*(TILESIZE_X/*/32;
	Global.inTileCameraRelYZ = Global.cameraYZ * ypixeld * /*(TILESIZE_X/*/32;
		
	// calculate the per tile data
	// x offset
	Global.perTileCameraRelXX = Global.cameraXX * TILESIZE_X * xpixeld;
	Global.perTileCameraRelXY = Global.cameraXY * TILESIZE_X * xpixeld;
	Global.perTileCameraRelXZ = Global.cameraXZ * TILESIZE_X * xpixeld;
		
	// y offset
	Global.perTileCameraRelYX = Global.cameraYX * TILESIZE_Y * ypixeld;
	Global.perTileCameraRelYY = Global.cameraYY * TILESIZE_Y * ypixeld;
	Global.perTileCameraRelYZ = Global.cameraYZ * TILESIZE_Y * ypixeld;
		
	// do some simd spreads
	Intrinsic::f4 globalCameraXPos = Intrinsic::set(Global.cameraXPos,
	                                                Global.cameraXPos,
	                                                Global.cameraXPos,
	                                                Global.cameraXPos);
					
	Intrinsic::f4 globalCameraYPos = Intrinsic::set(Global.cameraYPos,
	                                                Global.cameraYPos,
	                                                Global.cameraYPos,
	                                                Global.cameraYPos);
					
	Intrinsic::f4 globalCameraZPos = Intrinsic::set(Global.cameraZPos,
	                                                Global.cameraZPos,
	                                                Global.cameraZPos,
	                                                Global.cameraZPos);
		
		
	Intrinsic::f4 xpixeldV = Intrinsic::set(xpixeld,
	                                        xpixeld,
	                                        xpixeld,
	                                        xpixeld);

	Intrinsic::f4 ypixeldV = Intrinsic::set(ypixeld,
	                                        ypixeld,
	                                        ypixeld,
	                                        ypixeld);

		
	Intrinsic::f4 cameraYXV = Intrinsic::set(Global.cameraYX,
	                                         Global.cameraYX,
	                                         Global.cameraYX,
	                                         Global.cameraYX);
	
	Intrinsic::f4 cameraYYV = Intrinsic::set(Global.cameraYY,
	                                         Global.cameraYY,
	                                         Global.cameraYY,
	                                         Global.cameraYY);

	
	Intrinsic::f4 cameraYZV = Intrinsic::set(Global.cameraYZ,
	                                         Global.cameraYZ,
	                                         Global.cameraYZ,
	                                         Global.cameraYZ);
		
		

	Intrinsic::f4 cameraXXV = Intrinsic::set(Global.cameraXX,
	                                         Global.cameraXX,
	                                         Global.cameraXX,
	                                         Global.cameraXX);
		
	Intrinsic::f4 cameraXYV = Intrinsic::set(Global.cameraXY,
	                                         Global.cameraXY,
	                                         Global.cameraXY,
	                                         Global.cameraXY);

		
	Intrinsic::f4 cameraXZV = Intrinsic::set(Global.cameraXZ,
	                                         Global.cameraXZ,
	                                         Global.cameraXZ,
	                                         Global.cameraXZ);



	// precalculate some values
	//pixelWidth? = Global.cameraX? * xpixeld
	Intrinsic::f4 pixelWidthX = Intrinsic::mul(cameraXXV, xpixeldV);
	Intrinsic::f4 pixelWidthY = Intrinsic::mul(cameraXYV, xpixeldV);
	Intrinsic::f4 pixelWidthZ = Intrinsic::mul(cameraXZV, xpixeldV);
		
	// do setup some SIMD stuff
	Intrinsic::f4 raster0 = Intrinsic::set( 3.0f,  2.0f,  1.0f,  0.0f);
	Intrinsic::f4 raster1 = Intrinsic::set( 7.0f,  6.0f,  5.0f,  4.0f);
	Intrinsic::f4 raster2 = Intrinsic::set(11.0f, 10.0f,  9.0f,  8.0f);
	Intrinsic::f4 raster3 = Intrinsic::set(15.0f, 14.0f, 13.0f, 12.0f);
	Intrinsic::f4 raster4 = Intrinsic::set(19.0f, 18.0f, 17.0f, 16.0f);
	Intrinsic::f4 raster5 = Intrinsic::set(23.0f, 22.0f, 21.0f, 20.0f);
	Intrinsic::f4 raster6 = Intrinsic::set(27.0f, 26.0f, 25.0f, 24.0f);
	Intrinsic::f4 raster7 = Intrinsic::set(31.0f, 30.0f, 29.0f, 28.0f);
		
	Intrinsic::f4 f4Minus = Intrinsic::set(-1.0f, -1.0f, -1.0f, -1.0f);

	// start the timer
	timer_renderAll.startTime();

	// rebuild everything
	//  transform
	transformObjectsIntoWorld();
	//  rebuild the tree
	rebuildTree();
	
	UniTimer.sum = 0;

	// this is the mainloop

	// this is a timer for the whool main rendering thing (without tree-build, transformation and stuff)
	NSTimer<true> timer_main;
	
	// this is a timer for the kernel function "syscall"
	NSTimer<true> timer_call_kernel;
	
	// this is a timer for the upload "syscall"
	NSTimer<true> timer_call_upload;

	// this is a timer for the overal time of the "ray traversal"
	NSTimer<true> timer_rayTraversal;
	timer_rayTraversal.sum = 0;

	NSTimer<true> timer_rayTraversal_PolyInteresction;
	timer_rayTraversal_PolyInteresction.sum = 0;
	unsigned long timer_rayTraversal_PolyInteresction_time = 0;
	unsigned long timer_rayTraversal_PolyInteresction_count = 0;
	
	timer_main.startTime();

	// calculate the number of tiles
	unsigned long tilesCount = tiles.x * tiles.y;
	for( fasti i = 0; i < tilesCount; i++ ) {
		fasti xi = i % tiles.x; // is the x index of that tile
		fasti yi = i / (tiles.y+1); // is the y index of that tile
		
		// TODO< recall the i value to something different >
		fasti actualTile_ = i;

		// calculate the (0,0) position of the (single) ray of that tile
		// TODO< nuke this? >
		PerThread.cameraDirX = Global.cameraBeginX + xi * Global.perTileCameraRelXX  + yi * Global.perTileCameraRelYX;
		PerThread.cameraDirY = Global.cameraBeginY + xi * Global.perTileCameraRelXY + yi * Global.perTileCameraRelYY;
		PerThread.cameraDirZ = Global.cameraBeginZ + xi * Global.perTileCameraRelXZ + yi * Global.perTileCameraRelYZ;
		/*
		Intrinsic::f4 cameraDirXV = Intrinsic::set(PerThread.cameraDirX,
		                                           PerThread.cameraDirX,
		                                           PerThread.cameraDirX,
		                                           PerThread.cameraDirX);

		Intrinsic::f4 cameraDirYV = Intrinsic::set(PerThread.cameraDirY,
		                                           PerThread.cameraDirY,
		                                           PerThread.cameraDirY,
		                                           PerThread.cameraDirY);

		Intrinsic::f4 cameraDirZV = Intrinsic::set(PerThread.cameraDirZ,
		                                           PerThread.cameraDirZ,
		                                           PerThread.cameraDirZ,
		                                           PerThread.cameraDirZ);
		*/



		PerThread.cameraDirBeginX = PerThread.cameraDirX;
		PerThread.cameraDirBeginY = PerThread.cameraDirY;
		PerThread.cameraDirBeginZ = PerThread.cameraDirZ;
			
		// loop thougth the x sections
		for( fasti ysection = 0; ysection < (TILESIZE_Y/32); ysection++ ) {
			// /8 cos we use 8x8 "big ray packets"
				
			PerThread.cameraDirX = PerThread.cameraDirBeginX;
			PerThread.cameraDirY = PerThread.cameraDirBeginY;
			PerThread.cameraDirZ = PerThread.cameraDirBeginZ;





			// loop thougth the x sections
			for( fasti xsection = 0; xsection < (TILESIZE_X/32); xsection++ ) {
					
				Intrinsic::f4 cameraDirXV = Intrinsic::set(PerThread.cameraDirX,
		                                                   PerThread.cameraDirX,
		                                                   PerThread.cameraDirX,
		                                                   PerThread.cameraDirX);

				Intrinsic::f4 cameraDirYV = Intrinsic::set(PerThread.cameraDirY,
		                                                   PerThread.cameraDirY,
		                                                   PerThread.cameraDirY,
		                                                   PerThread.cameraDirY);

				Intrinsic::f4 cameraDirZV = Intrinsic::set(PerThread.cameraDirZ,
		                                                   PerThread.cameraDirZ,
		                                                   PerThread.cameraDirZ,
		                                                   PerThread.cameraDirZ);



				UniTimer.startTime();
					
				// calculate all directions and stuff (precalculated things for aabb tests and so on)

				// fill the ray structure
				for( unsigned long yray = 0; yray < 32; yray++ ) {
					Intrinsic::f4 yrayV = Intrinsic::set(static_cast<float>(yray), static_cast<float>(yray), static_cast<float>(yray), static_cast<float>(yray));

					// downRay? = Global.cameraY? * ypixeld * yray
					Intrinsic::f4 downRayX = Intrinsic::mul(Intrinsic::mul(cameraYXV, ypixeldV), yrayV);
					Intrinsic::f4 downRayY = Intrinsic::mul(Intrinsic::mul(cameraYYV, ypixeldV), yrayV);
					Intrinsic::f4 downRayZ = Intrinsic::mul(Intrinsic::mul(cameraYZV, ypixeldV), yrayV);
						
					pluecker_setup_ray2(rays2->rays[0 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
					                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster0)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster0)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster0)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[1 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster1)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster1)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster1)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[2 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
					                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster2)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster2)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster2)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[3 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster3)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster3)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster3)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[4 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster4)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster4)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster4)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[5 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster5)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster5)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster5)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[6 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster6)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster6)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster6)), downRayZ),
						                    
					                    f4Minus
					);

					pluecker_setup_ray2(rays2->rays[7 + yray*(32/4)].ptrays,
						                    
					                    globalCameraXPos,
					                    globalCameraYPos,
					                    globalCameraZPos,
						                    
					                    Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster7)), downRayX),
					                    Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster7)), downRayY),
					                    Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster7)), downRayZ),
						                    
					                    f4Minus
					);
						

					rays2->rays[0 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster0)), downRayX);
					rays2->rays[0 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster0)), downRayY);
					rays2->rays[0 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster0)), downRayZ);
						
					rays2->rays[1 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster1)), downRayX);
					rays2->rays[1 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster1)), downRayY);
					rays2->rays[1 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster1)), downRayZ);
						
					rays2->rays[2 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster2)), downRayX);
					rays2->rays[2 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster2)), downRayY);
					rays2->rays[2 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster2)), downRayZ);
						
					rays2->rays[3 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster3)), downRayX);
					rays2->rays[3 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster3)), downRayY);
					rays2->rays[3 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster3)), downRayZ);
						
					rays2->rays[4 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster4)), downRayX);
					rays2->rays[4 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster4)), downRayY);
					rays2->rays[4 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster4)), downRayZ);
						
					rays2->rays[5 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster5)), downRayX);
					rays2->rays[5 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster5)), downRayY);
					rays2->rays[5 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster5)), downRayZ);
						
					rays2->rays[6 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster6)), downRayX);
					rays2->rays[6 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster6)), downRayY);
					rays2->rays[6 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster6)), downRayZ);
						
					rays2->rays[7 + yray*(32/4)].vx = Intrinsic::add(Intrinsic::add(cameraDirXV, Intrinsic::mul(pixelWidthX, raster7)), downRayX);
					rays2->rays[7 + yray*(32/4)].vy = Intrinsic::add(Intrinsic::add(cameraDirYV, Intrinsic::mul(pixelWidthY, raster7)), downRayY);
					rays2->rays[7 + yray*(32/4)].vz = Intrinsic::add(Intrinsic::add(cameraDirZV, Intrinsic::mul(pixelWidthZ, raster7)), downRayZ);
						
					// reset the id and the deep
					// TODO< maybe some SSE things for a little speed up? >
					rays2->rays[0 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[0 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[0 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[0 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[0 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[0 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[0 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[0 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[1 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[1 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[1 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[1 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[1 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[1 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[1 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[1 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[2 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[2 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[2 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[2 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[2 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[2 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[2 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[2 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[3 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[3 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[3 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[3 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[3 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[3 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[3 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[3 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[4 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[4 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[4 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[4 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[4 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[4 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[4 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[4 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[5 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[5 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[5 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[5 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[5 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[5 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[5 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[5 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[6 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[6 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[6 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[6 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[6 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[6 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[6 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[6 + yray*(32/4)].deep._[3] = 1e6;

					rays2->rays[7 + yray*(32/4)].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[7 + yray*(32/4)].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[7 + yray*(32/4)].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[7 + yray*(32/4)].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[7 + yray*(32/4)].deep._[0] = 1e6;
					rays2->rays[7 + yray*(32/4)].deep._[1] = 1e6;
					rays2->rays[7 + yray*(32/4)].deep._[2] = 1e6;
					rays2->rays[7 + yray*(32/4)].deep._[3] = 1e6;
				}

				// reset the id and the deep
				/*for( unsigned long i = 0; i < (32/4)*32; i++ ) {
						

					rays2->rays[i].contentPtr._[0] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[i].contentPtr._[1] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[i].contentPtr._[2] = reinterpret_cast<BVH_Content*>(1);
					rays2->rays[i].contentPtr._[3] = reinterpret_cast<BVH_Content*>(1);

					rays2->rays[i].deep._[0] = 1e6;
					rays2->rays[i].deep._[1] = 1e6;
					rays2->rays[i].deep._[2] = 1e6;
					rays2->rays[i].deep._[3] = 1e6;
				}*/

				// set up the other variables
				//  primary ray camera position
				// TODO< set this up earlyier >
				rays2->ppx._[0] = Global.cameraXPos;
				rays2->ppx._[1] = Global.cameraXPos;
				rays2->ppx._[2] = Global.cameraXPos;
				rays2->ppx._[3] = Global.cameraXPos;

				rays2->ppy._[0] = Global.cameraYPos;
				rays2->ppy._[1] = Global.cameraYPos;
				rays2->ppy._[2] = Global.cameraYPos;
				rays2->ppy._[3] = Global.cameraYPos;

				rays2->ppz._[0] = Global.cameraZPos;
				rays2->ppz._[1] = Global.cameraZPos;
				rays2->ppz._[2] = Global.cameraZPos;
				rays2->ppz._[3] = Global.cameraZPos;
					
				UniTimer.stopTime();
				UniTimer.addSum();
				
				timer_rayTraversal.startTime();
				
				// TODO< more general algorithm > !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					
				// this are the structures for the ray-AABB test
				////aabbRay test_aabbray;
				aabb2_ray_orginal testray;

				aabb2_make_ray_orginal(
					Global.cameraXPos,
					Global.cameraYPos,
					Global.cameraZPos,

					PerThread.cameraDirX + Global.cameraXX * xpixeld * 16.0f + Global.cameraYX * ypixeld * 16.0f,
					PerThread.cameraDirY + Global.cameraXY * xpixeld * 16.0f + Global.cameraYY * ypixeld * 16.0f,
					PerThread.cameraDirZ + Global.cameraXZ * xpixeld * 16.0f + Global.cameraYZ * ypixeld * 16.0f,

					&testray
				);

				

				// set up the aabb ray-structure
				/*setup_aabb_ray(Global.cameraXPos, Global.cameraYPos, Global.cameraZPos,
				               PerThread.cameraDirX, PerThread.cameraDirY, PerThread.cameraDirZ,
							   &test_aabbray);
				*/


				// calculate the frustrum for the "big ray-packet"
				struct frustrum packetFrustrum;

				packetFrustrum.v1_x = Intrinsic::set(Global.inTileCameraRelXX,
				                                    PerThread.cameraDirX + Global.inTileCameraRelXX,
				                                    PerThread.cameraDirX + Global.inTileCameraRelYX,
				                                    Global.inTileCameraRelYX);
				packetFrustrum.v1_y = Intrinsic::set(Global.inTileCameraRelXY,
				                                    PerThread.cameraDirY + Global.inTileCameraRelXY,
				                                    PerThread.cameraDirY + Global.inTileCameraRelYY,
				                                    Global.inTileCameraRelYY);
				packetFrustrum.v1_z = Intrinsic::set(Global.inTileCameraRelXZ,
				                                    PerThread.cameraDirZ + Global.inTileCameraRelXZ,
				                                    PerThread.cameraDirZ + Global.inTileCameraRelYZ,
				                                    Global.inTileCameraRelYZ);
					
					
				packetFrustrum.v2_x = Intrinsic::set(PerThread.cameraDirX,
				                                    Global.inTileCameraRelYX,
				                                    Global.inTileCameraRelXX,
				                                    PerThread.cameraDirX);
				packetFrustrum.v2_y = Intrinsic::set(PerThread.cameraDirY,
				                                    Global.inTileCameraRelYY,
				                                    Global.inTileCameraRelXY,
				                                    PerThread.cameraDirY);
				packetFrustrum.v2_z = Intrinsic::set(PerThread.cameraDirZ,
				                                    Global.inTileCameraRelYZ,
				                                    Global.inTileCameraRelXZ,
				                                    PerThread.cameraDirZ);

				Inline::crossproduct(packetFrustrum.v1_x, packetFrustrum.v1_y, packetFrustrum.v1_z,
				                     packetFrustrum.v2_x, packetFrustrum.v2_y, packetFrustrum.v2_z,
				                     packetFrustrum.x, packetFrustrum.y, packetFrustrum.z);
				Inline::normalisize(packetFrustrum.x, packetFrustrum.y, packetFrustrum.z);
					
				packetFrustrum.w = Inline::dotproduct(packetFrustrum.x, packetFrustrum.y, packetFrustrum.z, globalCameraXPos, globalCameraYPos, globalCameraZPos);

				// reset the stack
				nodeStack.flush();

				// TODO< test if the root Node is inside the frustrum or if the camera is inside the aabb of the root >
				// TODO< we need the "is in aabb check" anyhow >
				// TODO< we need just the aabb test for ealy rejection >
				nodeStack.push(root);

				// tranversal
				for(;;) {
					struct BVH_Node* actualNode;
					
					actualNode = nodeStack.pop();
					
					if( actualNode == NULL ) {
						break;
					}

					if( actualNode->isLeaf ) {
						// we test against the content(pluecker-test)
						// orginal code
						
						for( unsigned long contenti = 0; contenti < actualNode->contentc; contenti++ ) {
							// we check if we hit the aabb-box of the polygons
							
							// TODO< 0 more tests >
							if( aabb2_slope_orginal(&testray, &(actualNode->content[contenti]->aabb)) ) {
							
								timer_rayTraversal_PolyInteresction.startTime();
								//test_raypacket_BVHContent_(rays2, actualNode->content[contenti]);
								
								// TODO< some debug output >
								/*cout << std::hex;
								std::hex(cout);
								cout << actualNode->content[contenti] << endl;
								cout << actualNode->content[contenti]->polysc * sizeof(struct poly) + actualNode->content[contenti] << endl;
								std::dec(cout);*/
								
								test_raypacket_BVHContent_2(rays2, actualNode->content[contenti], &debug);
								//test_raypacket_BVHContent_(rays2, actualNode->content[contenti]);

								timer_rayTraversal_PolyInteresction_count++;
							
								timer_rayTraversal_PolyInteresction.stopTime();
								timer_rayTraversal_PolyInteresction.addSum();
							}
						}
						

						/* benchmark code */
						
						
						/*
						// measure the cache timing + normal timing
						unsigned long timings[10000];

						for( unsigned long i = 0; i < 10000; i++ ) {

						timer_rayTraversal_PolyInteresction.startTime();
						test_raypacket_BVHContent_(rays2, actualNode->content[0]);
						timer_rayTraversal_PolyInteresction.stopTime();

						timings[i] = timer_rayTraversal_PolyInteresction.delta;
						}

						// print the timings
						unsigned long min_ = 4200000000;
						unsigned long max_ = 0;
						unsigned long maxpos;
						unsigned long long avg_ = 0;

						for( unsigned long i = 0; i < 10000; i++ ) {
							if( timings[i] < min_ ) {
								min_ = timings[i];
							}

							if( timings[i] > max_ ) {
								max_ = timings[i];
								maxpos = i;
							}

							avg_ += timings[i];
						}

						cout << "min:" << min_ << endl;
						cout << "max:" << max_ << "[" << maxpos << "]" << endl;

						cout << "avg:" << avg_ / 10000 << endl;

						cout << "." << endl;*/
						
					}
					else {
						// we push the childens that are in the frustrum (or if the camera position is inside the aabb)
						// TODO< push the children that is next to the camera "as last"(that it is checked as first >

						Intrinsic::f4 frustrum_deep, frustrum_dist2;
							
						frustrum_deep = Inline::dotproduct(packetFrustrum.x, packetFrustrum.y, packetFrustrum.z,
						                                                 &(actualNode->childrens[0]->aabb_frustrum.posx), &(actualNode->childrens[0]->aabb_frustrum.posy), &(actualNode->childrens[0]->aabb_frustrum.posz)
						                                                );
						frustrum_dist2 = Intrinsic::add(Intrinsic::sub(frustrum_deep, packetFrustrum.w), Intrinsic::shuffle_spread(Intrinsic::loadS(&(actualNode->childrens[0]->aabb_frustrum.radius))));

						if( Intrinsic::get(frustrum_dist2, 0) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 1) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 2) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 3) > 0.0f
						) {
							// check if the aabb get hit by at least one ray
							if( aabb2_slope_orginal(&testray, &(actualNode->childrens[0]->aabb)) ) {
								// we push the children
								nodeStack.push(actualNode->childrens[0]);
							}
							else {
								cout << "herio" << endl;
							}
						}


						frustrum_deep = Inline::dotproduct(packetFrustrum.x, packetFrustrum.y, packetFrustrum.z,
						                                                 &(actualNode->childrens[1]->aabb_frustrum.posx), &(actualNode->childrens[1]->aabb_frustrum.posy), &(actualNode->childrens[1]->aabb_frustrum.posz)
						                                                );
						frustrum_dist2 = Intrinsic::add(Intrinsic::sub(frustrum_deep, packetFrustrum.w), Intrinsic::shuffle_spread(Intrinsic::loadS(&(actualNode->childrens[1]->aabb_frustrum.radius))));

						if( Intrinsic::get(frustrum_dist2, 0) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 1) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 2) > 0.0f &&
							Intrinsic::get(frustrum_dist2, 3) > 0.0f
						) {
							// check if the aabb get hit by at least one ray
							if( aabb2_slope_orginal(&testray, &(actualNode->childrens[1]->aabb)) ) {
								// we push the children
								nodeStack.push(actualNode->childrens[1]);
							}
							else {
								cout << "herio" << endl;
							}
						}
					}
				}
											
				timer_rayTraversal.stopTime();
				timer_rayTraversal.addSum();


				// we copy the ray data to binned Memory of the OpenCL-device
				for( fasti rayI = 0; rayI < (32/4)*32; rayI++ ) {
					
						
					// is the x adress
					unsigned long addr_x = (rayI % (32/4)) * 4 + xsection*32 + xi*TILESIZE_X; // 2 cos 4 rays match 2 times in 8 pixel 
					// is the y adress
					unsigned long addr_y = rayI / (32/4) + ysection *32 + yi*TILESIZE_Y;
					// put it together
					unsigned long addr = addr_x + addr_y * TILESIZE_X*tiles.x;
						
					opencl_shading_in* addr0 = &(opencl_shading[addr + 0]);
					opencl_shading_in* addr1 = &(opencl_shading[addr + 1]);
					opencl_shading_in* addr2 = &(opencl_shading[addr + 2]);
					opencl_shading_in* addr3 = &(opencl_shading[addr + 3]);
					
					

					rays2_* rp = &(rays2->rays[rayI]);
					
					
					

					// TODO< write deep to the structure >
					//float deep0 = rp->deep._[0];
					//float deep1 = rp->deep._[1];
					//float deep2 = rp->deep._[2];
					//float deep3 = rp->deep._[3];

					addr0->contentPtr = reinterpret_cast<unsigned long>( rp->contentPtr._[0] );
					addr1->contentPtr = reinterpret_cast<unsigned long>( rp->contentPtr._[1] );
					addr2->contentPtr = reinterpret_cast<unsigned long>( rp->contentPtr._[2] );
					addr3->contentPtr = reinterpret_cast<unsigned long>( rp->contentPtr._[3] );
						
					addr0->posX = Global.cameraXPos + rp->deep._[0] * rp->vx.m128_f32[0];
					addr0->posY = Global.cameraYPos + rp->deep._[0] * rp->vy.m128_f32[0];
					addr0->posZ = Global.cameraZPos + rp->deep._[0] * rp->vz.m128_f32[0];

					addr0->Vx = rp->vx.m128_f32[0];
					addr0->Vy = rp->vy.m128_f32[0];
					addr0->Vz = rp->vz.m128_f32[0];
					
					
					addr1->posX = Global.cameraXPos + rp->deep._[1] * rp->vx.m128_f32[1];
					addr1->posY = Global.cameraYPos + rp->deep._[1] * rp->vy.m128_f32[1];
					addr1->posZ = Global.cameraZPos + rp->deep._[1] * rp->vz.m128_f32[1];

					addr1->Vx = rp->vx.m128_f32[1];
					addr1->Vy = rp->vy.m128_f32[1];
					addr1->Vz = rp->vz.m128_f32[1];
						
					addr2->posX = Global.cameraXPos + rp->deep._[2] * rp->vx.m128_f32[2];
					addr2->posY = Global.cameraYPos + rp->deep._[2] * rp->vy.m128_f32[2];
					addr2->posZ = Global.cameraZPos + rp->deep._[2] * rp->vz.m128_f32[2];
					
					addr2->Vx = rp->vx.m128_f32[2];
					addr2->Vy = rp->vy.m128_f32[2];
					addr2->Vz = rp->vz.m128_f32[2];

					addr3->posX = Global.cameraXPos + rp->deep._[3] * rp->vx.m128_f32[3];
					addr3->posY = Global.cameraYPos + rp->deep._[3] * rp->vy.m128_f32[3];
					addr3->posZ = Global.cameraZPos + rp->deep._[3] * rp->vz.m128_f32[3];
					
					addr3->Vx = rp->vx.m128_f32[3];
					addr3->Vy = rp->vy.m128_f32[3];
					addr3->Vz = rp->vz.m128_f32[3];

					/*addr0->Vx = rp->vx.m128_f32[0];
					addr1->Vx = rp->vx.m128_f32[1];
					addr2->Vx = rp->vx.m128_f32[2];
					addr3->Vx = rp->vx.m128_f32[3];
					addr0->Vy = rp->vy.m128_f32[0];
					addr1->Vy = rp->vy.m128_f32[1];
					addr2->Vy = rp->vy.m128_f32[2];
					addr3->Vy = rp->vy.m128_f32[3];*/
					// TODO< the sign thingy for the z vector >
					// TODO< maybe we can throw out the if thingys IF its gurantueed that every ray hits something >
					if( rp->contentPtr._[0] != reinterpret_cast<struct BVH_Content*>(1) ) {
#if 0
													
						addr0->Nx = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.nx.SKALAR;
						addr0->Ny = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.ny.SKALAR;
						
						float Nz = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.nz.SKALAR;
						addr0->Nzs = Nz < 0.0f ? 1 : 0; // TODO< do work this as a bit operation out and so on (comression?)
#else
						// this is the barycentric coordinate stuff

						//                 ->
						// calculate (h) = AH
						float hx = addr0->posX - rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].Ax;
						float hy = addr0->posY - rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].Ay;
						
						// calculate the coordinates
						float beta  = ( rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].bX * hy   -   rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].bY * hx ) * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].mul;
						float gamma = ( hx * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].cY   -   hy * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].cX ) * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].mul;
						float alpha = 1.0f - beta - gamma;
						
						// interpolarate the 3 normals with the barycentric coordinates
						addr0->Nx = alpha * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n1.x  +  beta * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n2.x  +  gamma * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n3.x;
						addr0->Ny = alpha * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n1.y  +  beta * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n2.y  +  gamma * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n3.y;
							
						///float  Nz = alpha * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n1.z  +  beta * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n2.z  +  gamma * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n3.z;
						///addr0->Nzs = Nz < 0.0f ? 1 : 0; // TODO< do work this as a bit operation out and so on (comression?)
						addr0->Nz = alpha * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n1.z  +  beta * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n2.z  +  gamma * rp->contentPtr._[0]->polysBaryCoords[ rp->surface._[0] ].n3.z;
						/* this is a "draft" for the "compression" algorithm for packing the vector into one single 4 byte value */
						//float Nx = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.nx.SKALAR;
						//float Ny = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.ny.SKALAR;
						//float Nz = rp->contentPtr._[0]->polys[  rp->surface._[0]  ].plane.nz.SKALAR;
							
						//unsigned long compressed0 = 0;
						//unsigned long Nx0 = static_cast<unsigned long>(((Nx + 1.0f) / 2.0f) * 56536.0f/* 2^16 */);
						//unsigned long Ny0 = static_cast<unsigned long>(((Ny + 1.0f) / 2.0f) * 56536.0f/* 2^15 */);
						//long Nzf = static_cast<long>(Nz);

						//compressed0 = (Nx0 << 16) | (Ny << 1) | (Nzf >> 31);
#endif
					}
						
					if( rp->contentPtr._[1] != reinterpret_cast<struct BVH_Content*>(1) ) {
#if 0
						addr1->Nx = rp->contentPtr._[1]->polys[  rp->surface._[1]  ].plane.nx.SKALAR;
						addr1->Ny = rp->contentPtr._[1]->polys[  rp->surface._[1]  ].plane.ny.SKALAR;
					
						float Nz = rp->contentPtr._[1]->polys[  rp->surface._[1]  ].plane.nz.SKALAR;
						addr1->Nzs = Nz < 0.0f ? 1 : 0; // TODO< do work this as a bit operation out and so on (comression?)
#else
						float hx = addr1->posX - rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].Ax;
						float hy = addr1->posY - rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].Ay;
							
						float beta  = ( rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].bX * hy   -   rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].bY * hx ) * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].mul;
						float gamma = ( hx * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].cY   -   hy * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].cX ) * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].mul;
						float alpha = 1.0f - beta - gamma;

						addr1->Nx = alpha * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n1.x  +  beta * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n2.x  +  gamma * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n3.x;
						addr1->Ny = alpha * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n1.y  +  beta * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n2.y  +  gamma * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n3.y;
						///float  Nz = alpha * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n1.z  +  beta * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n2.z  +  gamma * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n3.z;
						///addr1->Nzs = Nz < 0.0f ? 1 : 0;
						addr1->Nz = alpha * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n1.z  +  beta * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n2.z  +  gamma * rp->contentPtr._[1]->polysBaryCoords[ rp->surface._[1] ].n3.z;

#endif
					}
						
					if( rp->contentPtr._[2] != reinterpret_cast<struct BVH_Content*>(1) ) {
#if 0
						addr2->Nx = rp->contentPtr._[2]->polys[  rp->surface._[2]  ].plane.nx.SKALAR;
						addr2->Ny = rp->contentPtr._[2]->polys[  rp->surface._[2]  ].plane.ny.SKALAR;
					
						float Nz = rp->contentPtr._[2]->polys[  rp->surface._[2]  ].plane.nz.SKALAR;
						addr2->Nzs = Nz < 0.0f ? 1 : 0; // TODO< do work this as a bit operation out and so on (comression?)
#else
						float hx = addr2->posX - rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].Ax;
						float hy = addr2->posY - rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].Ay;
							
						float beta  = ( rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].bX * hy   -   rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].bY * hx ) * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].mul;
						float gamma = ( hx * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].cY   -   hy * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].cX ) * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].mul;
						float alpha = 1.0f - beta - gamma;

						addr2->Nx = alpha * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n1.x  +  beta * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n2.x  +  gamma * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n3.x;
						addr2->Ny = alpha * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n1.y  +  beta * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n2.y  +  gamma * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n3.y;
						//float  Nz = alpha * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n1.z  +  beta * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n2.z  +  gamma * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n3.z;
						//addr2->Nzs = Nz < 0.0f ? 1 : 0;
						addr2->Nz = alpha * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n1.z  +  beta * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n2.z  +  gamma * rp->contentPtr._[2]->polysBaryCoords[ rp->surface._[2] ].n3.z;
#endif
					}

					if( rp->contentPtr._[3] != reinterpret_cast<struct BVH_Content*>(1) ) {
#if 0
						addr3->Nx = rp->contentPtr._[3]->polys[  rp->surface._[3]  ].plane.nx.SKALAR;
						addr3->Ny = rp->contentPtr._[3]->polys[  rp->surface._[3]  ].plane.ny.SKALAR;
							
						float Nz = rp->contentPtr._[3]->polys[  rp->surface._[3]  ].plane.nz.SKALAR;
						addr3->Nzs = Nz < 0.0f ? 1 : 0; // TODO< do work this as a bit operation out and so on (comression?)
#else
						float hx = addr3->posX - rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].Ax;
						float hy = addr3->posY - rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].Ay;
							
						float beta  = ( rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].bX * hy   -   rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].bY * hx ) * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].mul;
						float gamma = ( hx * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].cY   -   hy * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].cX ) * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].mul;
						float alpha = 1.0f - beta - gamma;

						addr3->Nx = alpha * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n1.x  +  beta * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n2.x  +  gamma * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n3.x;
						addr3->Ny = alpha * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n1.y  +  beta * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n2.y  +  gamma * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n3.y;
						//float  Nz = alpha * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n1.z  +  beta * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n2.z  +  gamma * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n3.z;
						//addr3->Nzs = Nz < 0.0f ? 1 : 0;
						addr3->Nz = alpha * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n1.z  +  beta * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n2.z  +  gamma * rp->contentPtr._[3]->polysBaryCoords[ rp->surface._[3] ].n3.z;
#endif
					}
				}

				// and we also copy the other later on needed data
				// TODO

				// shift it to the right
				PerThread.cameraDirX += Global.inTileCameraRelXX;
				PerThread.cameraDirY += Global.inTileCameraRelXY;
				PerThread.cameraDirZ += Global.inTileCameraRelXZ;
			}

			// shift it to the down
			PerThread.cameraDirBeginX += Global.inTileCameraRelYX;
			PerThread.cameraDirBeginY += Global.inTileCameraRelYY;
			PerThread.cameraDirBeginZ += Global.inTileCameraRelYZ;
		}
	}
		
		timer_call_upload.startTime();
		// we let the openCL device shade it and we can calculate the shadows (and maybe reflections/refraction?)			
		opencl_shading_buffer->write(CL_FALSE /* not blocking */,
		                                          opencl_shading,
												  (TILESIZE_X*tiles.x * TILESIZE_Y*tiles.y) * sizeof(struct opencl_shading_in),
												  0,
												  &opencl_shading_events, /* we want that this emits a event */
												  0,
												  0);
		timer_call_upload.stopTime();

		// sync openGL for openGL/openCL interop
		glFinish();

		cl_event enqueueDone;
		
		// we need this that we can access the openGL Texture in openCL
		cl_int ret_ = clEnqueueAcquireGLObjects(
			opencl.testcode_getCQ(), /* is the command queue */
			1, /* we have one object */
			oclDisplayTexture->getPtr(), /* our image object */
			1, /* number of elements in the wait list */
			&opencl_shading_events, /* waitlist */
			&enqueueDone /* events to emit */
		);

		if( ret_ != CL_SUCCESS ) {
			// TODO< analyse the error >

			throw new class GeneralError("OpenCL");
		}

		// shade it (run kernel)

		size_t globalWorkSize[2] = {TILESIZE_X*tiles.x, TILESIZE_Y*tiles.y};
		size_t localWorkSize[2] = {32, 16};
		
		cl_event shading_kernel; // is the openCL-event of the shading kernel
		
		timer_call_kernel.startTime();

		//cl_event shading_kernel; // is the openCL-event of the shading kernel
		ret_ = clEnqueueNDRangeKernel(opencl.testcode_getCQ(), /* is the command queue */
		                       opencl_shade_kernel,     /* is the shading kernel */
		                       2,                       /* work dimensions, we need only one dimension */
		                       NULL,                    /* global work offset, have to be NULL */
		                       globalWorkSize,          /* global work size : */
		                       localWorkSize,                    /* local work size : is NULL cos openCL should itself estimate it and it don't care */
		                       1,                       /* events in wait list : 1*/
		                       &(enqueueDone), /* pointer to the events in the wait list : we have to wait for the writing action */
		                       &(shading_kernel)  /* pointer at the event of that calculations : put it into this */
		                      );
		
		timer_call_kernel.stopTime();

		if( ret_ !=  CL_SUCCESS ) {
			string desc = OpenCLw::getErrorText(ret_);

			throw new class GeneralError("OpenCL(" + desc + ")");
		}
			
		// now we release again the OpenGL Texture
		ret_ = clEnqueueReleaseGLObjects(
			opencl.testcode_getCQ(), /* is the command queue */
			1, /* we have one object */
			oclDisplayTexture->getPtr(), /* our image object */
			1, /* number of elements in the wait list */
			&shading_kernel, /* waitlist */
			NULL /* events to emit */
		);

		if( ret_ != CL_SUCCESS ) {
			// TODO< analyse the error >

			throw new class GeneralError("OpenCL");
		}
		
		timer_call_clFinish.startTime();
		
		// after this it is safe to use OpenGL
		ret_ = clFinish(opencl.testcode_getCQ());
		
		timer_call_clFinish.stopTime();
		
		if( ret_ != CL_SUCCESS ) {
			// TODO< analyse the error >

			throw new class GeneralError("OpenCL");
		}
		
		// this are the timings of the profiling
		cl_ulong profiling_upload_start, profiling_upload_end;
		cl_ulong profiling_render_start, profiling_render_end;

		// after this we profile the timings of the upload and the rendering
		ret_ = clGetEventProfilingInfo(opencl_shading_events, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &profiling_upload_start, NULL);
		
		if( ret_ != CL_SUCCESS ) {
			throw new class GeneralError("OpenCL");
		}
		
		ret_ = clGetEventProfilingInfo(opencl_shading_events, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &profiling_upload_end, NULL);
		
		if( ret_ != CL_SUCCESS ) {
			throw new class GeneralError("OpenCL");
		}

		// calculate the time difference
		float time_upload = (float)((double)1.0e-9 * (profiling_upload_end - profiling_upload_start));

		cout << "Up:" << time_upload << endl;
	
		
		// and the timing of the shading kernel
		ret_ = clGetEventProfilingInfo(shading_kernel, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &profiling_render_start, NULL);
		
		if( ret_ != CL_SUCCESS ) {
			throw new class GeneralError("OpenCL");
		}
		
		ret_ = clGetEventProfilingInfo(shading_kernel, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &profiling_render_end, NULL);
		
		if( ret_ != CL_SUCCESS ) {
			throw new class GeneralError("OpenCL");
		}

		
		// calculate the time difference
		float time_render = (float)((double)1.0e-9 * (profiling_render_end - profiling_render_start));

		cout << "Render:" << time_render << endl;


	// read from openCL-device to host

	// we deactive it because it crashes here [crash A]
#if 0
	opencl_shading_outBinned_buffer->read(CL_TRUE,
	                                      opencl_shading_outBinned,
	                                      (TILESIZE_X*tiles.x * TILESIZE_Y*tiles.y) * sizeof(struct opencl_shading_out),
	                                      0, /* offset : 0 */
	                                      NULL, /* event: we need none */
	                                      1,    /* waitlist-Count : we wait for one event */
	                                      &(shading_kernel_events) /* events in waitlist : only this event */
											);
#endif		

#if 1
	///struct opencl_shading_out* p = &(opencl_shading_outBinned[0]);
		
	///for( fasti yp = 0; yp < TILESIZE_Y*tiles.y; yp++ ) {
	///	for( fasti xp = 0; xp < TILESIZE_X*tiles.x; xp++ ) {
	///		SDL_PutPixel(Screen, xp, yp, rgb(p->r, p->g, p->b));
	///		p++;
	///	}
	///}
#endif
		


	///if( SDL_MUSTLOCK(Screen) ) {
	///	SDL_UnlockSurface (Screen);
	///}

	// render the openGL stuff
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	CheckGLError();
	glLoadIdentity();
	CheckGLError();

	// i don't know why -2. ... is here?
	glTranslatef(0, 0, -2.416);
		
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, oglDisplayTexture);

	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0); /*glColor3f(0, 0, 0);*/ glVertex3f(-1,1, 0);  
		glTexCoord2f(0, 768.f/1024.f); /*glColor3f(0, 1, 0);*/ glVertex3f(-1,-1, 0);
		glTexCoord2f(1, 0); /*glColor3f(1, 0, 0);*/ glVertex3f( 1,1, 0);
		glTexCoord2f(1, 768.f/1024.f); /*glColor3f(1, 1, 0);*/ glVertex3f( 1,-1, 0);
	glEnd();
	
	// swap the buffers
	SDL_GL_SwapBuffers();

	timer_main.stopTime();
	timer_renderAll.stopTime();

	cout << "Timing:" << endl;
	cout << "[ALL                       ]:" << static_cast<double>(timer_renderAll.delta     )/2000000000.0 << "s. (" << 1.0 / (static_cast<double>(timer_renderAll.delta  )/2000000000.0) << ") FPS" << endl;
	cout << "[Main                      ]:" << static_cast<double>(timer_main.delta          )/2000000000.0 << "s." << endl;
	cout << "[Call:Kernel:Render        ]:" << static_cast<double>(timer_call_kernel.delta   )/2000000000.0 << "s." << endl;
	cout << "[Call:Upload               ]:" << static_cast<double>(timer_call_upload.delta   )/2000000000.0 << "s." << endl;
	cout << "[Call:clFinish             ]:" << static_cast<double>(timer_call_clFinish.delta )/2000000000.0 << "s." << endl;
	cout << "[RayTraversal              ]:" << static_cast<double>(timer_rayTraversal.sum    )/2000000000.0 << "s." << endl;
	cout << "[RayTraversal:Intersection ]:" << static_cast<double>(timer_rayTraversal_PolyInteresction.sum)/2000000000.0 << "s." << endl;
	cout << timer_rayTraversal_PolyInteresction_count << endl;

	// debug the time
	cout << UniTimer.sum/2 << " ns" << endl;
	cout << UniTimer.sum/2000 << " us" << endl;
	cout << UniTimer.sum/2000000 << " ms" << endl;
	cout << static_cast<double>(UniTimer.sum)/2000000000.0 << " sec" << endl;
		
	cout << endl;
}
