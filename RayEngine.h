
#include <string>
#include <list>

#include "SDL\SDL.h"

#include "OpenCLW.h"
#include "varray.h"
#include "vector3.h"
#include "_misc_stuff.h"
#include "objImport.h"
#include "bvh.h"
#include "Stack.h"
#include "alignAllocator.h"
#include "NSTimer.h"

using namespace std;

extern "C" void test_raypacket_BVHContent_(struct rays2* Rays, struct BVH_Content* Content);

// we have both function for testing
// now with a function for debugging
extern "C" void test_raypacket_BVHContent_2(struct rays2* Rays, struct BVH_Content* Content, void (*ptr)(unsigned int, void*));


// this is the structure that is used for pushing the shading data to the openCL-device
// (per pixel)
struct opencl_shading_in { // OPENCL
	// the "view vector"
	// this is not normalisized
	// (we dont need vz cos we can calculate it cos the vector have to be normalisized)
	// TODO< we need a flag for the sign of the z component >
	float Vx;
	float Vy;
	float Vz;
	
	// is the normal of the surface
	// the same again like V?
	float Nx;
	float Ny;
	float Nz;

	// is the raw contentPtr
	// ASK< should we use and fetch the material id directly? >
	unsigned long contentPtr;
	
	// is the position of the hit
	float posX;
	float posY;
	float posZ;
	
	// padding for full 32 byte (cos for the x86 its a bit fast =) )
	//unsigned long pad0;
	//unsigned long pad1;
};

// this is a "temporary" structure that is used for the output from the OpenCL-device
// (per Pixel)
struct opencl_shading_out { // OPENCL
	float r;
	float g;
	float b;
};

// this is a structure for aditional data that needs to be passed to the kernel
// it is used for
// - global camera settings (where it is, begin of first ray, vectors of the rayplane)
// - lighting settings
struct opencl_struct_additional {
	float light0_posx;
	float light0_posy;
	float light0_posz;
	
	// this is the normalisized "view vector" of the light
	// we need it only for spotlights
	float light0_vecx;
	float light0_vecy;
	float light0_vecz;

	// this is the maximal "field of view" of the spotlight in radiants
	float light0_spot;
};

// this is the global data-structure
// can only be readed in each rendering-cycle by all threads but not written to
struct global {
	// is the position of the Camera
	float cameraXPos;
	float cameraYPos;
	float cameraZPos;

	// this is the unormalisized beginning at (0,0) that the tile(0,0) have to render
	// it is the unormalisized vector
	float cameraBeginX;
	float cameraBeginY;
	float cameraBeginZ;
	
	// shift of the ray beginning to get a SIMD 4x1 ray-packet
	/* ***
	struct v4 ShiftX;
	struct v4 ShiftY;
	struct v4 ShiftZ;
	*/

	// this is the absolute vector that spans the whoole X distance from cameraBegin to the end of the X direction
	float cameraXX;
	float cameraXY;
	float cameraXZ;

	// this is the absolute vector that spans the whoole Y distance form cameraBegin to the end of the Y direction
	float cameraYX;
	float cameraYY;
	float cameraYZ;

	// this are the datastructures for the redering "in the tile"

	// is the x offset that is added after each "big ray-packet", it pushes the Camera-Direction in the x Direction further
	float inTileCameraRelXX;
	float inTileCameraRelXY;
	float inTileCameraRelXZ;

	// is the y offset that is added after each "big ray-packet", it pushes the Camera-Direction in the y Direction further
	float inTileCameraRelYX;
	float inTileCameraRelYY;
	float inTileCameraRelYZ;
	

	// this are the datastructure for the calculation of the "out tile" level
	
	// is the x offset that is added for each tile in the x direction
	float perTileCameraRelXX;
	float perTileCameraRelXY;
	float perTileCameraRelXZ;

	// is the y offset that is added for each tile in the y direction
	float perTileCameraRelYX;
	float perTileCameraRelYY;
	float perTileCameraRelYZ;
};

// this is a per thread structure
// is duplicated for each thread that do the rendering
// do also contain all types of important constants/precalculated data and so on
struct perThread {
	// is the unnormalisized Direction (Vector) from the camera to the top-left corner of a "big ray-packet"
	float cameraDirX;
	float cameraDirY;
	float cameraDirZ;
	
	// a unormaisized Direction (Vector) from the camera to the topest position of a tile where all rays start
	float cameraDirBeginX;
	float cameraDirBeginY;
	float cameraDirBeginZ;
};



class RayEngine {
   protected:
	class myObjImport : public ObjImport {
	   public:
		myObjImport(RayEngine* Engine);
	   protected:
		RayEngine* engine;

		virtual void addVerticle(float PosX, float PosY, float PosZ);

		virtual void addFace(unsigned long Verticle1, unsigned long Verticle2, unsigned long Verticle3);
	};

   public:
	class Camera {
		friend class RayEngine;
	   public:
		Vector3 position;
		Vector3 look;  // look vector
		Vector3 up;    // up vector

		inline void getCameraX(Vector3& CameraX) const {
			CameraX = cameraX;
		}

		inline void getCameraY(Vector3& CameraY) const {
			CameraY = cameraY;
		}

	   protected:
		// this recalculate the Vectors in the left direction(cameraX) or in the right direction (cameraY)
		void recalculate();
		
		Vector3 cameraX;
		Vector3 cameraY;
	};

	Camera* camera; // is the global camera
	
	// this class is to throw errors at "TODO" places
	class NotImplemented {
	};

	// this is used to throw Errors that should not Occur and are proofs for DesignFlaws
	class DesignFlaw {
	};

	// this is used if no memory is free
	class NoMemory {
	};
	
	// this is used for Error handling if something is with openGL not right
	class GLError {
		public:
		GLError(string Desc = ""):desc(Desc){}
	
		string	desc;
	};

	// this is used for Error handling
	class GeneralError {
		public:
		GeneralError(string Desc = ""):desc(Desc){}
	
		string	desc;
	};

	// TODO< use the primary BSC thing for the importing and bounding of something, maybe aslo remove this class completly? >
	// this is a bounding-sphere that contains the Triangles
	// a array with this class is given to the BVH-builder
	class BoundingSphereContainer {
	   public:
		float sphere_posx;   // is the x position of the bounding sphere
		float sphere_posy;   // is the y position of the bounding sphere
		float sphere_posz;   // is the z position of the boudning sphere
		float sphere_radius; // is the radius of the bounding sphere
		
		VArray<unsigned long> polygons; // is a VArray with the ids of the polygons that it does contain

		// this is the bounding box ONLY OF THE TRIANGLES, NOT THE SPHERES
		float bb_minx;
		float bb_miny;
		float bb_minz;

		float bb_maxx;
		float bb_maxy;
		float bb_maxz;
	};

	// this is a "public" Mesh
	// it Contains only the untransformed data
	class Mesh {
		friend class RayEngine;
	   public:
		~Mesh() {
			// delete all BoundingSphereContainer_primary objects
			for( unsigned long i = 0; i < BScontainer.getCount(); i++ ) {
				delete BScontainer.elements[i];
			}

			// delete all verticles elements
			for( unsigned long i = 0; i < verticles.getCount(); i++ ) {
				delete verticles.elements[i];
			}

			// delete all triangles
			for( unsigned long i = 0; i < triangles.getCount(); i++ ) {
				delete triangles.elements[i];
			}
		}
		
	   protected:
		// this is a structure for the Triangle
		struct Triangle_primary {
	
			// this are the indices for the Verticle indexes of the Triangle-Verticles
			unsigned long v1;
			unsigned long v2;
			unsigned long v3;
		};

		// this structure is used for the Verticles Array
		struct Verticle_primary {
			Vector3 position;
			
			// this is used to calculate the normal of this Verticle
			Vector3 normal;
		};

		// this is a bounding-sphere that contains the Triangles
		// a array with this class is given to the BVH-builder
		// _primary means that it is in object space and don't have a bounding box (cos its tranformed)
		class BoundingSphereContainer_primary {
			//friend class RayEngine;
		   public:
			Vector3 sphere_position; // is the position of the sphere
			/* REMOVE
			float sphere_posx;   // is the x position of the bounding sphere
			float sphere_posy;   // is the y position of the bounding sphere
			float sphere_posz;   // is the z position of the boudning sphere
			*/
			float sphere_radius; // is the radius of the bounding sphere
			
			VArray<unsigned long> polygons; // is a VArray with the ids of the polygons that it does contain
		};
		
		VArray<BoundingSphereContainer_primary*> BScontainer; // this is a container with the Bounding Spheres (that contain the triangles)
		VArray<Verticle_primary*>                verticles; // is a container with the verticles of the mesh
		VArray<Triangle_primary*>                triangles; // is a container with the traingles of the mesh
	};

	// this is a "public" Object
	// it can be moved and rotated and scaled and all changes do affect the transaltion and so on of the mesh inside if it is translated into world space
	class Object {
		friend class RayEngine;
	   public:
		Object(Mesh* Mesh) : mesh(Mesh) {
		}

		Vector3 position;
		Vector3 rotation;
	   protected:
		Mesh* mesh; // is the mesh it does use
		
		// Vector3 rotation; TODO<>
		// float scale; TODO<>
	};
	
	// this structure is used for the Verticles Array
	   // it is the position of the verticle
	struct Verticle {
		float posx;
		float posy;
		float posz;

		bool counted; // is used by the regBound function (see the documentation there)
	};
	
	// this is the structure for the Triangle Array
	struct Triangle {

		// this are the indices for the Verticle indexes of the Triangle-Verticles
		unsigned long v1;
		unsigned long v2;
		unsigned long v3;

		// this is the normalisized vector (can be calculated after loading model? >
		float vx;
		float vy;
		float vz;

		// this is used to indicate that the triangle was allready used in a sphere(can't used again)
		// have to set to false in the entry to th buildSpheres method
		bool isUsed;

		// TODO< do add a flag that shows if the triangle is in the actual set >

		float radius; // is the radius of the triangle

		// this is the middle point of the triangle
		float middleX;
		float middleY;
		float middleZ;

		// this is the pre-calculates polygon-area, have to be recalculated if the positions were changed
		float area;
	};

	VArray<BoundingSphereContainer*> BScontainer;
	
	list<Object*> objects; // is a ll with the objects that the engine have to use
	
	// this checks for any error of the openGL stuff
	// throws GLError
	void CheckGLError();

	// throws GLError, NoMemory
	RayEngine();

	~RayEngine();
	
	class InvalidResolution {
	};

	// this sets the resolution of the renderer
	// throws InvalidResolution if the resolution is not valid
	void setResolution(unsigned long X, unsigned long Y);
	
	// this creates a new Object with the Mesh as the "hull"
	// throws NoMemory
	Object* createObject(Mesh* Mesh);

	// this tries to build up the best spherical fit of the Triangles in the Array
	void buildSpheres();

	// this builds up out of the engine verticle/polygon/BSC data a Mesh and return a pointer to it
	// it throws noMemory
	Mesh* createMesh();

	// this transform all objects(with the meshs) into the global data 
	void transformObjectsIntoWorld();

	// this is called by rebuildTree and rebuildTree2 calls it self for each level of deep
	// TODO< maybe rebuild this as a non-recursive algorithm? >

	// IntoNode : into which Node should the result go?
	// Content  : what content should go into that Node?
	// ContentC : is the count of the Content
	void rebuildTree2(BVH_Node* IntoNode, BVH_Content** Content, unsigned long ContentC);

	// this delete the whoole tree and rebuilds it from the BScontainer-Array that was calculated in the method buildSpheres
	// NOTE< it calls recursivly rebuildTree2 >
	void rebuildTree();

	// this imports a obj Object
	void importObj(string name);

	class NoCamera {
	};

	// this renders the image (to the screen, the member do lock it)
	// throws:
	//    InvalidResolution
	//       If the resolution is not seted
	//    NoCamera
	//       If the camera was not seted
	void render(SDL_Surface* Screen);
   
	BVH_Node* root; // is the root-BVH Cell
	
	// this is used for the creation of the Triangle Clusters
	//  array with the verticles

	// TODO< do implement this as a memory location >
	Verticle      verticles[100000];         // are the verticles
	unsigned long verticlesC;        // this is the count of the verticles in that array
	
	// array with the triangles
	Triangle      triangles[100000];
	unsigned long trianglesC;
	
   protected:
	xyInt resolution; // is the resolution
	xyInt tiles;      // is the number of tiles
	bool resSetedUp;  // was the resolution seted up?
	
	// this is the raw and the aligned pointer to the rays2 structure for the engine
	struct rays2* rays2_raw;
	struct rays2* rays2;

	Stack<256, struct BVH_Node*> nodeStack; // is the stack for the tree-transversal
	                                        // TODO< make a array out of this and cpu-count dependent because every cpu-core needs its own stack >
	
	OpenCLw opencl; // our openCL object
	
	cl_kernel         opencl_shade_kernel; // is our shader kernel
	cl_event          shading_kernel_events; // this is a array with the events that the execution of our kernel is done
	OpenCLw::Program* opencl_shade_program; // is the Program object of the shade Kernel
	

	// this is the (per Thread) buffer that is given to the "shading openCL Kernel"
	opencl_shading_in* opencl_shading;        // is the Binned memory for our shading in data for our device
	OpenCLw::Buffer*   opencl_shading_buffer; // is the openCL-buffer for the memory
	cl_event            opencl_shading_events; // this is a array with the events that the writing to the opencl_shading structure is done/in process

	opencl_shading_out* opencl_shading_outBinned; // is the Binned memory for our output for the shading openCL kernel
	OpenCLw::Buffer*    opencl_shading_outBinned_buffer;
	
	// this is for additional data for shading and stuff
	opencl_struct_additional* opencl_additional;
	OpenCLw::Buffer*          opencl_additional_buffer;

	OpenCLw::Buffer*    opencl_perlinNoiseArray;
	
	// openGL
	GLuint oglDisplayTexture; // is the texture for the Display
	OpenCLw::Image* oclDisplayTexture; // is the openCL image for the Display texture
	
	// this is a helper class for all misc funtions
	class misc {
	   public:
		// this calculates the polygon-surface area
		static float calc_polygon_area(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z, float p3x, float p3y, float p3z);
	};

	// calculate the area and the bounding-sphere of the triangles
	// NOTE< needs valid data >
	void calculateTriangles();

	// set up all data of all verticles
	// calculate the area and the bounding-sphere of the triangles
	// NOTE< needs validated data >
	void setupVerticles() {
		for( unsigned i = 0; i < verticlesC; i++ ) {
			verticles[i].counted = false;
		}
	}

	// this calculates the average Sureface area of the bounding Spheres
	void calculateAvgSA() {
		avgSA = 0.0f; // is the average Surface Area
		
		for( unsigned i = 0; i < trianglesC; i++ ) {
			avgSA = avgSA + PI * triangles[i].radius*triangles[i].radius;
		}

		avgSA /= float(trianglesC);
	}

	// this checks if all polygons are used
	bool allPolygonsUsed() {
		for( unsigned long i = 0; i < trianglesC; i++ ) {
			if( !triangles[i].isUsed ) {
				return false;
			}
		}

		return true;
	}

	// this searches for the first next unused triangle, it don't check if it can't find it
	unsigned long searchUnusedTriangle() {
		for( unsigned long i = 0; i < trianglesC; i++ ) {
			if( !triangles[i].isUsed ) {
				return i;
			}
		}
		
		// this is not possible?
		throw class DesignFlaw();
	}

	// this is used only for debugging?
	// count the number of the Used polygons
	unsigned long countUsedPolygons() {
		unsigned long count = 0;

		for( unsigned long i = 0; i < trianglesC; i++ ) {
			if( triangles[i].isUsed ) {
				count++;
			}
		}

		return count;
	}

	float avgSA; // is the average surface Area of the boudning spheres in the scene
	
	myObjImport* objImporter; // is a pointer at the myObjImport object for the importing of the obj stuff
	
	AlignAllocator<0xFFFFFF00, 256, struct BVH_Node> nodeAllocator;
	AlignAllocator<0xFFFFFF00, 256, struct BVH_Content> contentAllocator;

	// universaltimer
	NSTimer<true> UniTimer;
};
