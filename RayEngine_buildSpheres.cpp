#include "RayEngine.h"
#include "mathfunctions.h"
#include "vector_cStyle.h"

// for debugging
#include <iostream>
using namespace std;

void RayEngine::buildSpheres() {
	// this is a class that is used for the set of the Triangles that was accepted
	class KSet {
	   public:
		KSet(RayEngine* Engine) {
			ref_.trianglesC = 0;
			
			engine = Engine;
			
		}
		
		// this tries to add a Triangle to the set
		bool addTriangle(unsigned long Index) {
			// if the set is allready full
			if( ref_.trianglesC == 100 ) {
				return false;
			}
			
			// add the triangle
			ref_.triangles[ref_.trianglesC++] = Index;
			
			return true;
		}

		// this removes the last Polygon
		void removeLastTriangle() {
			if( ref_.trianglesC > 0 ) {
				ref_.trianglesC--;
			}
			else {
				// TODO< throw error >
			}
		}
			
		// this "flushs" all actual triangles
		void flush() {
			ref_.trianglesC = 0;
		}

		// this do calculate the "rating" of the actual set
		float calc() {
			// this clas sis used for static function (as a kind of a hack for functions in function...) ;)
			class _ {
			   public:
				static float reg(RayEngine* Engine, struct ref& ref__) {
					return areaP(Engine, ref__) / areaC(Engine, ref__);
				}
				
				static float regBound(RayEngine* Engine, struct ref& ref__) {
					unsigned long countDistinctVerticles = 0; // is the count of the Verticles of that set

					// count the distinct verticles (with help flag of each verticle)
					for( unsigned long i = 0; i < ref__.trianglesC; i++ ) {
						if( !(Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v1 ].counted) ) {
							Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v1 ].counted = true;

							countDistinctVerticles++;
						}

						if( !(Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v2 ].counted) ) {
							Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v2 ].counted = true;

							countDistinctVerticles++;
						}

						if( !(Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v3 ].counted) ) {
							Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v3 ].counted = true;

							countDistinctVerticles++;
						}
					}

					// disable again the flags
					for( unsigned long i = 0; i < ref__.trianglesC; i++ ) {
						Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v1 ].counted = false;
						Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v2 ].counted = false;
						Engine->verticles[ Engine->triangles[ ref__.triangles[i] ].v3 ].counted = false;
					}

					// return the calculated value
					return static_cast<float>( countDistinctVerticles ) / static_cast<float> ( ref__.static_.MaxCount );
				}

				static float sizeBound(RayEngine* Engine, struct ref& ref__) {
					return math_max(1.0, ref__.static_.SA / (ref__.static_.MaxSize * Engine->avgSA));
				}

			   protected:
				static void nav(RayEngine* Engine, struct ref& ref__, float& Vx, float &Vy, float& Vz ) {
					Vx = 0.0f;
					Vy = 0.0f;
					Vz = 0.0f;
						
					for( unsigned long i = 0; i < ref__.trianglesC; i++ ) {
						Vx += Engine->triangles[ ref__.triangles[i] ].vx;
						Vy += Engine->triangles[ ref__.triangles[i] ].vy;
						Vz += Engine->triangles[ ref__.triangles[i] ].vz;
					}

					vector_normalisize(Vx, Vy, Vz);
				}
					
				static float areaP(RayEngine* Engine, struct ref& ref__) {
					float ret = 0.0f;

					float nAvgX, nAvgY, nAvgZ;

					// calculate the average normal
					nav(Engine, ref__, nAvgX, nAvgY, nAvgZ);

					for( unsigned long i = 0; i < ref__.trianglesC; i++ ) {
						ret += Engine->triangles[ ref__.triangles[i] ].area * math_abs(
						                                                               vector_dotproduct(nAvgX,
						                                                                                 nAvgY,
						                                                                                 nAvgZ,
						                                                                                 Engine->triangles[ ref__.triangles[i] ].vx,
						                                                                                 Engine->triangles[ ref__.triangles[i] ].vy,
						                                                                                 Engine->triangles[ ref__.triangles[i] ].vz
																										 )
						                                                              );
					}

					return ret;
				}
				
				inline static float areaC(RayEngine* Engine, struct ref& ref__) {
					return PI * ref__.static_.radius*ref__.static_.radius;
				}
					
			};

			// TODO
			// set the Parameters
			// TODO< parameterisize it deeper >
			ref_.static_.MaxCount = 50;
			ref_.static_.MaxSize = 50.0;

			refreshBoundingSphere();

			// actualisize the Surface-Area of the boudning-sphere around that set
			ref_.static_.SA = PI * ref_.static_.radius*ref_.static_.radius;

			return _::reg(engine, ref_) - _::sizeBound(engine, ref_) * _::regBound(engine, ref_);
		}
			
		// this adds a polygon, calculate the rating and removes it
		float calc2(unsigned long Index) {
			float rating;
				
			addTriangle(Index);
			rating = calc();
			removeLastTriangle();
				
			return rating;
		}
			
		// this returns the data of the Set(bounding sphere)
		void getBoundingSphere(float& Posx, float& Posy, float& Posz, float& Radius) {
			refreshBoundingSphere();
				
			Posx = ref_.static_.posx;
			Posy = ref_.static_.posy;
			Posz = ref_.static_.posz;
			Radius = ref_.static_.radius;
		}

		// this returns the pointer to the array with the triangle-ids of that set
		unsigned long* getPolygons() {
			return &(ref_.triangles[0]);
		}

		// this return the count of triangles of that set
		unsigned long getPolygonCount() {
			return ref_.trianglesC;
		}

	   protected:
		// this is a "hack" that i can reference the array
		struct ref {
			unsigned long triangles[100]; // is the array with the triangles in that set
			unsigned long trianglesC;     // is the counter for the Elements
			
			// this is a "hack" for pseudo-static variables for the static help functions
			struct static__ {
				float SA;

				// the parameters
				unsigned long MaxCount;
				float         MaxSize;
				
				// this is the position and the radius of the sphere that contains the polygons in the set
				float posx;
				float posy;
				float posz;
				float radius;
			};
				
			// NOTE< this is used as a replacement of the static thing cos its propably in VC++ BROKEN >
			struct static__ static_;
		};

		struct ref ref_;

		RayEngine* engine; // is a pointer at the ray-engine
			
		void refreshBoundingSphere() {
			// fill the position and the radius with the data of the first polygon
			ref_.static_.posx   = engine->triangles[ ref_.triangles[0] ].middleX;
			ref_.static_.posy   = engine->triangles[ ref_.triangles[0] ].middleY;
			ref_.static_.posz   = engine->triangles[ ref_.triangles[0] ].middleZ;
			ref_.static_.radius = engine->triangles[ ref_.triangles[0] ].radius;
				
			// calculate the new posx and radius with the new triangle
			for( unsigned long i = 1; i < ref_.trianglesC; i++ ) {
				// calculate the distance between the triangle that is added and the sphere of all doned triangles
				float dist = vector_length(ref_.static_.posx,
				                           ref_.static_.posy,
				                           ref_.static_.posz,
				                           engine->triangles[ ref_.triangles[i] ].middleX,
				                           engine->triangles[ ref_.triangles[i] ].middleY,
				                           engine->triangles[ ref_.triangles[i] ].middleZ
				);

				if( dist + engine->triangles[ ref_.triangles[i] ].radius > ref_.static_.radius ) {
					// the new triangles is blowing up the Bounding-sphere

					// calculate the new position and radius of the bounding sphere
								
					float vx, vy, vz; // is the normalisized vector between the bounding sphere of the triangle group and the middle of the new added triangle
					float o1x, o1y, o1z, o2x, o2y, o2z; // are the "outer points"

					// calculate the normalisized vector
					vx = engine->triangles[ ref_.triangles[i] ].middleX - ref_.static_.posx;
					vy = engine->triangles[ ref_.triangles[i] ].middleY - ref_.static_.posy;
					vz = engine->triangles[ ref_.triangles[i] ].middleZ - ref_.static_.posz;
								
					vector_normalisize(vx, vy, vz);

					// calculate the outer-points
					o1x = engine->triangles[ ref_.triangles[i] ].middleX + engine->triangles[ ref_.triangles[i] ].radius * vx;
					o1y = engine->triangles[ ref_.triangles[i] ].middleY + engine->triangles[ ref_.triangles[i] ].radius * vy;
					o1z = engine->triangles[ ref_.triangles[i] ].middleZ + engine->triangles[ ref_.triangles[i] ].radius * vz;
								
					o2x = ref_.static_.posx - ref_.static_.radius * vx;
					o2y = ref_.static_.posy - ref_.static_.radius * vy;
					o2z = ref_.static_.posz - ref_.static_.radius * vz;

					// calculate the middle of that two points (and set it as the new middle of the bounding sphere for the triangle set
					ref_.static_.posx = (o1x + o2x) / 2.0f;
					ref_.static_.posy = (o1y + o2y) / 2.0f;
					ref_.static_.posz = (o1z + o2z) / 2.0f;

					// calculate the distance from the middle to one point(it is the new radius of the boudning sphere of the triangle set)
					ref_.static_.radius = vector_length(ref_.static_.posx, ref_.static_.posy, ref_.static_.posz, o1x, o1y, o1z);
				}
			}
		}
	};
	
	// this is the array with potencial polygons and their rating (of the polygon with the polygon set that was in the last step accepted)
	struct potencialTriangle {
		unsigned long index; // is the index of the potencial polygon
		float         rating; // is the rating of the polygon set with that polygon, can be nagative
	};

	potencialTriangle potencial[200];
	unsigned long     potencialC = 0; // is the count


	unsigned long actualTriangle = 0; // is the index of the actual triangle (we set it to the first triangle)
		
	setupVerticles(); // setup all verticles

	// TODO< validate the data of the triangles >

	// calculate the data of the triangles(area, sphere, ...)
	calculateTriangles();

	// calculate the avaerage Sphere Area of the Polygons
	calculateAvgSA();

	triangles[actualTriangle].isUsed = true;
	// search for the connected triangles to that actualTriangle
		
	KSet kset(this);

	kset.addTriangle(0); // we add the triangle
	// TODO< test for null value >
		
	for(;;) {
		// "flush" the potencial polygon list
		potencialC = 0;

		// debug it
		// (the count of the Used polygons)
		cout << countUsedPolygons() << endl;

		// search for all polygons at the egdes of the used polygons
		for( unsigned long ti = 0; ti < trianglesC; ti++ ) {
			if( !triangles[ti].isUsed && ti != actualTriangle ) {
				unsigned long connectionPointC = 0; // is counter for points that both Triangles uses
	
				if( ( triangles[ti].v1 == triangles[actualTriangle].v1 ) || 
				    ( triangles[ti].v1 == triangles[actualTriangle].v2 ) || 
					( triangles[ti].v1 == triangles[actualTriangle].v3 )
				) {
					connectionPointC++;
				}
	
				if( ( triangles[ti].v2 == triangles[actualTriangle].v1 ) || 
				    ( triangles[ti].v2 == triangles[actualTriangle].v2 ) || 
					( triangles[ti].v2 == triangles[actualTriangle].v3 )
				) {
					connectionPointC++;
				}
	
				if( ( triangles[ti].v3 == triangles[actualTriangle].v1 ) || 
				    ( triangles[ti].v3 == triangles[actualTriangle].v2 ) || 
					( triangles[ti].v3 == triangles[actualTriangle].v3 )
				) {
					connectionPointC++;
				}
					
				// MAYBE < if it is 3, we have a fault proof ( in the data _or_ in the algorithm ) >
				if( connectionPointC >= 2 ) {
					// set it as a potencial polygon
						
					potencial[potencialC].index = ti;
					potencial[potencialC].rating = kset.calc2(ti);
						
					potencialC++;
					if( potencialC > 200 ) {
						// TODO< throw error >
						unsigned long a = 0;
					}
				}
			}
		}
	
		// choose best polygon of the potencials, if all are negative we have to start a new set
		unsigned long besti = -1; // is the index of the potencial best polygon
		float         bestRating = 0.0f;       // is the rating of the best polygon
			
		for( unsigned long i = 0; i < potencialC; i++ ) {
			if( potencial[i].rating > bestRating ) {
				bestRating = potencial[i].rating;
				besti = i;
			}
		}
	
		if( besti == -1 ) {
			BoundingSphereContainer* bsc = new BoundingSphereContainer();

			// if it has not found a "best group"
				
			// add the group to the Array and repeat this process if not all polygons are used
				
			// get the bounding-sphere data of that set and save it into the Bounding Sphere container
			kset.getBoundingSphere(bsc->sphere_posx, bsc->sphere_posy, bsc->sphere_posz, bsc->sphere_radius);
				
			unsigned long  polygonCount = kset.getPolygonCount();
			unsigned long* polygonPtr   = kset.getPolygons();

			// copy the polygon-ids of the set into the bsc
			for( unsigned long i = 0; i < polygonCount; i++ ) {
				bsc->polygons.append( *(polygonPtr) );
				polygonPtr++;
			}

				
			// calculate the boundingbox of the polygons
			//  set fake-values for the bounding box
			bsc->bb_minx = 1e6f;
			bsc->bb_miny = 1e6f;
			bsc->bb_minz = 1e6f;

			bsc->bb_maxx = -1e6f;
			bsc->bb_maxy = -1e6f;
			bsc->bb_maxz = -1e6f;

			//  go througth every polygon in the bsc and expand the bounding-box
			for( unsigned long i = 0; i < bsc->polygons.getCount(); i++) {
				float posx, posy, posz; // are the positions of the actual verticle

				posx = verticles[ triangles[ bsc->polygons.elements[i] ].v1 ].posx;
				posy = verticles[ triangles[ bsc->polygons.elements[i] ].v1 ].posy;
				posz = verticles[ triangles[ bsc->polygons.elements[i] ].v1 ].posz;

				if( posx > bsc->bb_maxx ) {
					bsc->bb_maxx = posx;
				}
				if( posy > bsc->bb_maxy ) {
					bsc->bb_maxy = posy;
				}
				if( posz > bsc->bb_maxz ) {
					bsc->bb_maxz = posz;
				}

				if( posx < bsc->bb_minx ) {
					bsc->bb_minx = posx;
				}
				if( posy < bsc->bb_miny ) {
					bsc->bb_miny = posy;
				}
				if( posz < bsc->bb_minz ) {
					bsc->bb_minz = posz;
				}


				posx = verticles[ triangles[ bsc->polygons.elements[i] ].v2 ].posx;
				posy = verticles[ triangles[ bsc->polygons.elements[i] ].v2 ].posy;
				posz = verticles[ triangles[ bsc->polygons.elements[i] ].v2 ].posz;

				if( posx > bsc->bb_maxx ) {
					bsc->bb_maxx = posx;
				}
				if( posy > bsc->bb_maxy ) {
					bsc->bb_maxy = posy;
				}
				if( posz > bsc->bb_maxz ) {
					bsc->bb_maxz = posz;
				}

				if( posx < bsc->bb_minx ) {
					bsc->bb_minx = posx;
				}
				if( posy < bsc->bb_miny ) {
					bsc->bb_miny = posy;
				}
				if( posz < bsc->bb_minz ) {
					bsc->bb_minz = posz;
				}


				posx = verticles[ triangles[ bsc->polygons.elements[i] ].v3 ].posx;
				posy = verticles[ triangles[ bsc->polygons.elements[i] ].v3 ].posy;
				posz = verticles[ triangles[ bsc->polygons.elements[i] ].v3 ].posz;

				if( posx > bsc->bb_maxx ) {
					bsc->bb_maxx = posx;
				}
				if( posy > bsc->bb_maxy ) {
					bsc->bb_maxy = posy;
				}
				if( posz > bsc->bb_maxz ) {
					bsc->bb_maxz = posz;
				}

				if( posx < bsc->bb_minx ) {
					bsc->bb_minx = posx;
				}
				if( posy < bsc->bb_miny ) {
					bsc->bb_miny = posy;
				}
				if( posz < bsc->bb_minz ) {
					bsc->bb_minz = posz;
				}
			}

			// add the group to the array
			BScontainer.append(bsc);

			if( allPolygonsUsed() ) {
				break;
			}

			// flush the kset
			kset.flush();
				
			// we search for a unused triangle and set it as the actual triangle
			actualTriangle = searchUnusedTriangle();

			triangles[actualTriangle].isUsed = true;
				
			kset.addTriangle(actualTriangle); // we add the triangle
			// TODO< test for null value >
		}
		else {
			// if it has found a "best group" we add the polygon to the set and do this iteration again
	
			 triangles[ potencial[besti].index ].isUsed = true;
			 actualTriangle = potencial[besti].index;

			 kset.addTriangle(potencial[besti].index);
		}
	}
}

