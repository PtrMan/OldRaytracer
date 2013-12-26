
#include <string>

#include <windows.h>
#include <gl/gl.h>
#include <GL/glu.h>

#include "RayEngine.h"
#include "vector_cStyle.h"

using namespace std;

// myObjImport

RayEngine::myObjImport::myObjImport(RayEngine* Engine) {
	engine = Engine;
}

void RayEngine::myObjImport::addVerticle(float PosX, float PosY, float PosZ) {
	// add the verticle to the verticle list of the Engine
	engine->verticles[engine->verticlesC].posx = PosX;
	engine->verticles[engine->verticlesC].posy = PosY;
	engine->verticles[engine->verticlesC].posz = PosZ;
	
	engine->verticlesC++;
}

void RayEngine::myObjImport::addFace(unsigned long Verticle1, unsigned long Verticle2, unsigned long Verticle3) {
	// add the face to the polygon list of the Engine
	engine->triangles[engine->trianglesC].v1 = Verticle1;
	engine->triangles[engine->trianglesC].v2 = Verticle2;
	engine->triangles[engine->trianglesC].v3 = Verticle3;

	engine->trianglesC++;
}



// Camera

void RayEngine::Camera::recalculate() {
	vector_normalisize(look.x, look.y, look.z);

	vector_crossproduct(look.x, look.y, look.z, up.x, up.y, up.z, cameraX.x, cameraX.y, cameraX.z);
	vector_crossproduct(cameraX.x, cameraX.y, cameraX.z, look.x, look.y, look.z, cameraY.x, cameraY.y, cameraY.z);

	vector_normalisize(cameraX.x, cameraX.y, cameraX.z);
	vector_normalisize(cameraY.x, cameraY.y, cameraY.z);
}

// ...

void RayEngine::CheckGLError() {
	long error = glGetError();
	if(error != 0) {
		string desc;	// the description of the error
		switch(error) {
			case GL_INVALID_ENUM:
				desc = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				desc = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				desc = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				desc = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				desc = "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				desc = "GL_OUT_OF_MEMORY";
				break;
		}
		throw class RayEngine::GLError("OpenGL error: " + desc);
	}
}

RayEngine::RayEngine() {
	cl_int ret; // return code of the openCL api
	
	// inititalisize the openGL stuff
	glShadeModel(GL_SMOOTH);
	CheckGLError();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	CheckGLError();
	glClearDepth(1.0);
	CheckGLError();
	glEnable(GL_DEPTH_TEST);
	CheckGLError();
	glDepthFunc(GL_LEQUAL);
	CheckGLError();
	
	// we enable the textures for ever
	glEnable(GL_TEXTURE_2D);
	CheckGLError();

		
	// set the root-node to NULL
	root = NULL;
	
	opencl_shading_outBinned = NULL;
	opencl_shading_outBinned_buffer = NULL;
	
	// TODO< for other memory areas >

	verticlesC = 0;
	trianglesC = 0;
	
	// create a new instance of the Obj-import class
	objImporter = NULL;
	objImporter = new myObjImport(this);
	
	resSetedUp = false;

	// we dont have any camera
	camera = NULL;
	
	rays2_raw = NULL;
	rays2_raw = reinterpret_cast<struct rays2*>( malloc(sizeof(struct rays2) + 256) );
	
	if( rays2_raw == NULL ) {
		throw class RayEngine::NoMemory();
	}

	rays2     = reinterpret_cast<struct rays2*>(reinterpret_cast<long>(rays2_raw) & 0xFFFFFF00);
	
	// setup the rays2 structure
	rays2->sse_mask_sign._[0] = 0x80000000;
	rays2->sse_mask_sign._[1] = 0x80000000;
	rays2->sse_mask_sign._[2] = 0x80000000;
	rays2->sse_mask_sign._[3] = 0x80000000;
	
	// load our OpenCL program
	opencl_shade_program = opencl.openAndCompile("C:\\Users\\Rob\\Documents\\Visual Studio 2008\\Projects\\raytracer0\\raytracer0\\cl\\shade.cl");
		
	const unsigned long p[512] = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		23,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
		129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
		49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,
		160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		23,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
		129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
		49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};
			

	opencl_perlinNoiseArray = opencl.CreateBuffer(CL_MEM_READ_ONLY | /* its only readable in the kernel */
	                                              CL_MEM_COPY_HOST_PTR, /* we want to copy the data from the host to the device */
	                                              sizeof(unsigned long)*512,
	                                              const_cast<void*>(reinterpret_cast<const void*>(&p))
												 );


	// TODO< do make this resolution depend
		
	// we do allocate a binned buffer for the "shading in buffer"
	opencl_shading_buffer = opencl.CreateBuffer(CL_MEM_READ_ONLY     | /* its only readable in the kernel */
	                                            CL_MEM_ALLOC_HOST_PTR, /* we want to use pinned memory */
	                                            sizeof(opencl_shading_in)*(TILESIZE_X*4*TILESIZE_Y*3), /* todo< make this resolution dependent */
												NULL
	                                           );
	
	// we map it
	opencl_shading = reinterpret_cast<struct opencl_shading_in*>(opencl_shading_buffer->map(CL_TRUE, CL_MAP_WRITE, sizeof(struct opencl_shading_in)*(TILESIZE_X*4*TILESIZE_Y*3), 0) );
			
	
#if 1
	// we do allocate a binned buffer for the "shading out buffer"
	opencl_shading_outBinned_buffer = opencl.CreateBuffer(CL_MEM_WRITE_ONLY     | /* its only writable in the kernel */
	                                            CL_MEM_ALLOC_HOST_PTR, /* we want to use pinned memory */
	                                            sizeof(opencl_shading_out)*(TILESIZE_X*4*TILESIZE_Y*3),
												NULL
	                                           );
	// we map it
	opencl_shading_outBinned = reinterpret_cast<struct opencl_shading_out*>(opencl_shading_outBinned_buffer->map(CL_TRUE, CL_MAP_READ, sizeof(struct opencl_shading_out)*(TILESIZE_X*4*TILESIZE_Y*3), 0) );
#endif
#if 0
	void* ptr = malloc(sizeof(opencl_shading_out)*(TILESIZE_X*4*TILESIZE_Y*3));

	opencl_shading_outBinned_buffer = opencl.CreateBuffer(CL_MEM_WRITE_ONLY |     /* its only writable in the kernel */
	                                            CL_MEM_USE_HOST_PTR , /* we dont want to use pinned memory */
	                                            sizeof(opencl_shading_out)*(TILESIZE_X*4*TILESIZE_Y*3),
												ptr
	                                           );

	opencl_shading_outBinned = reinterpret_cast<struct opencl_shading_out*>( ptr );
#endif

	opencl_shade_kernel = opencl_shade_program->CreateKernel("shade");
	
	// set the kernel arguments up
	ret = clSetKernelArg(opencl_shade_kernel,                    /* is the shading kernel */
	               0,                                      /* is the argument index */
	               sizeof(cl_mem),                         /* size of the argument */
	               reinterpret_cast<void*>(opencl_shading_buffer->getPtr()) /* argument: is our buffer */
				  );
		
		
	if( ret != CL_SUCCESS ) {
		throw new class RayEngine::GeneralError("Kernel Arg 1");
	}


	ret = clSetKernelArg(opencl_shade_kernel,                    /* is the shading kernel */
	               1,                                      /* is the argument index */
	               sizeof(cl_mem),                         /* size of the argument */
	               reinterpret_cast<void*>(opencl_shading_outBinned_buffer->getPtr()) /* argument: is our buffer */
				  );

		
	if( ret != CL_SUCCESS ) {
		throw new class RayEngine::GeneralError("Kernel Arg 2");
	}


	ret = clSetKernelArg(opencl_shade_kernel,
	               2,
	               sizeof(cl_mem),
				   reinterpret_cast<void*>(opencl_perlinNoiseArray->getPtr())
	);

		
	if( ret != CL_SUCCESS ) {
		throw new class RayEngine::GeneralError("Kernel Arg 3");
	}



	// create the additional buffer
	opencl_additional_buffer = opencl.CreateBuffer(CL_MEM_READ_ONLY     | /* its only readable in the kernel */
	                                            CL_MEM_ALLOC_HOST_PTR, /* we want to use pinned memory */
	                                            sizeof(struct opencl_struct_additional),
												NULL
	                                           );
	// we map it
	opencl_additional = reinterpret_cast<struct opencl_struct_additional*>(opencl_additional_buffer->map(CL_TRUE, CL_MAP_WRITE, sizeof(struct opencl_struct_additional), 0) );

	// set the kernel argument
	ret = clSetKernelArg(opencl_shade_kernel,
	               4,
	               sizeof(cl_mem),
				   reinterpret_cast<void*>(opencl_additional_buffer->getPtr())
	);
		
	if( ret != CL_SUCCESS ) {
		throw new class RayEngine::GeneralError("Kernel Arg 5");
	}
	
	// set the additional data
	opencl_additional->light0_posx = -0.62f;
	opencl_additional->light0_posy = 0.16f;
	opencl_additional->light0_posz = 0.15f;

	opencl_additional->light0_vecx = 1.0f;
	opencl_additional->light0_vecy = 1.0f;
	opencl_additional->light0_vecz = 1.0f;
	
	opencl_additional->light0_spot = 0.5f; // TODO< change this value to something that do have sense
	
	// write it
	opencl_additional_buffer->write(
		CL_TRUE, /* Blocking */
		opencl_additional,
		sizeof(struct opencl_struct_additional),
		0 /* Offset */
	);



	// we allocate enougth space for the Allocators and Lock them
	nodeAllocator.reallocateElements(5000);
	nodeAllocator.setLock(true);

	contentAllocator.reallocateElements(1000);
	contentAllocator.setLock(true);
}

RayEngine::~RayEngine() {
	// delete the openGL Texture
	glDeleteTextures(1, &oglDisplayTexture);


	/*if( perThread_asyn_binnedMemory ) {
		free(perThread_asyn_binnedMemory);
	}*/
	

	// TODO< other things >

	if( rays2_raw != NULL ) {
		free(rays2_raw);
	}
		
	if( objImporter != NULL ) {
		delete objImporter;
	}
}

RayEngine::Mesh* RayEngine::createMesh() {
	RayEngine::Mesh* nmesh = new Mesh();
	
	if( !nmesh ) {
		throw class RayEngine::NoMemory();
	}
	
	// copy all verticles
	for( unsigned long i = 0; i < verticlesC; i++ ) {
		RayEngine::Mesh::Verticle_primary* nverticle = new Mesh::Verticle_primary();
		
		if( !nverticle ) {
			throw class NoMemory();
		}

		nverticle->position.x = verticles[i].posx;
		nverticle->position.y = verticles[i].posy;
		nverticle->position.z = verticles[i].posz;

		nmesh->verticles.append(nverticle);
	}

	// copy all Polygons
	for( unsigned long i = 0; i < trianglesC; i++ ) {
		Mesh::Triangle_primary* ntriangle = new Mesh::Triangle_primary();

		if( !ntriangle ) {
			throw class NoMemory();
		}

		ntriangle->v1 = triangles[i].v1;
		ntriangle->v2 = triangles[i].v2;
		ntriangle->v3 = triangles[i].v3;

		nmesh->triangles.append(ntriangle);
	}

	// copy all BS's
	for( unsigned long i = 0; i < BScontainer.getCount(); i++ ) {
		Mesh::BoundingSphereContainer_primary* nbsc = new Mesh::BoundingSphereContainer_primary();

		if( !nbsc ) {
			throw class NoMemory();
		}
			
		// copy all polygon-indices
		for( unsigned long polyi = 0; polyi < BScontainer.elements[i]->polygons.getCount(); polyi++ ) {
			nbsc->polygons.append(BScontainer.elements[i]->polygons.elements[polyi]);
		}

		nbsc->sphere_position.x = BScontainer.elements[i]->sphere_posx;
		nbsc->sphere_position.y = BScontainer.elements[i]->sphere_posy;
		nbsc->sphere_position.z = BScontainer.elements[i]->sphere_posz;
			
		nbsc->sphere_radius     = BScontainer.elements[i]->sphere_radius;
			
		nmesh->BScontainer.append(nbsc);
	}
		
	// TODO< use a std linked list to hold the references to the meshs ? >

	return nmesh;
}


void RayEngine::setResolution(unsigned long X, unsigned long Y) {
	cl_int ret; // return code of the openCL api
	
	// TODO< check for dividablility of the ray-packet size >
	if( X < 0 || Y < 0 || ((X % TILESIZE_X) != 0) || ((Y % TILESIZE_Y) != 0)  ) {
		throw class InvalidResolution();
	}

	tiles.x = X/TILESIZE_X;
	tiles.y = Y/TILESIZE_Y;

	resolution.x = X;
	resolution.y = Y;

	resSetedUp = true;

	// setup openGL
	glViewport(0, 0, X, Y); CheckGLError();
	
	glMatrixMode(GL_PROJECTION); CheckGLError();
	glLoadIdentity(); CheckGLError();
	gluPerspective(45, X/Y, 0.1, 100); CheckGLError();
	glMatrixMode(GL_MODELVIEW); CheckGLError();


	// here we create the texture for the output
	glGenTextures(1, &oglDisplayTexture);
	CheckGLError();

	glBindTexture(GL_TEXTURE_2D, oglDisplayTexture);
	CheckGLError();
		
	// set the filterring
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// allocate the texture
	// TODO< make this resolution dependent >
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	CheckGLError();

	// create the OpenCL image
	oclDisplayTexture = opencl.CreateImageFromOpenGLTexture(CL_MEM_WRITE_ONLY, oglDisplayTexture);

	// set the attribut for the kernel (because we have now a valid oclDisplayTexture(OpenCL Texture) id )
	ret = clSetKernelArg(opencl_shade_kernel,                    /* is the shading kernel */
	               3,                                      /* is the argument index */
	               sizeof(cl_mem),                         /* size of the argument */
	               reinterpret_cast<void*>(oclDisplayTexture->getPtr()) /* argument: is our image */
	);

	if( ret != CL_SUCCESS ) {
		throw new class GeneralError("Kernel Arg 4");
	}
}

RayEngine::Object* RayEngine::createObject(Mesh* Mesh) {
	RayEngine::Object* nobject = new Object(Mesh);
	
	if( !nobject ) {
		throw class NoMemory();
	}

	// add the new object to the object ll of the engine
	objects.push_back(nobject);

	return nobject;
}

void RayEngine::importObj(string name) {
	objImporter->import(name);
}

float RayEngine::misc::calc_polygon_area(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z, float p3x, float p3y, float p3z) {
	float vresx, vresy, vresz;

	vector_crossproduct(p1x - p2x, p1y - p2y, p1z - p2z, p1x - p3x, p1y - p3y, p1z - p3z, vresx, vresy, vresz);
	
	return 0.5f * vector_length(vresx, vresy, vresz);
}

void RayEngine::calculateTriangles() {
	for( unsigned i = 0; i < trianglesC; i++ ) {
		// calculate the area of the triangle:
		triangles[i].area = RayEngine::misc::calc_polygon_area(verticles[ triangles[i].v1 ].posx,
		                                      verticles[ triangles[i].v1 ].posy,
		                                      verticles[ triangles[i].v1 ].posz,
			
		                                      verticles[ triangles[i].v2 ].posx,
		                                      verticles[ triangles[i].v2 ].posy,
		                                      verticles[ triangles[i].v2 ].posz,
			
		                                      verticles[ triangles[i].v3 ].posx,
		                                      verticles[ triangles[i].v3 ].posy,
		                                      verticles[ triangles[i].v3 ].posz
		);

		// calculate the middlepoint and the radius of the triangle:
		triangles[i].middleX = ( verticles[ triangles[i].v1 ].posx + verticles[ triangles[i].v2 ].posx + verticles[ triangles[i].v3 ].posx ) / 3.0f;
		triangles[i].middleY = ( verticles[ triangles[i].v1 ].posy + verticles[ triangles[i].v2 ].posy + verticles[ triangles[i].v3 ].posy ) / 3.0f;
		triangles[i].middleZ = ( verticles[ triangles[i].v1 ].posz + verticles[ triangles[i].v2 ].posz + verticles[ triangles[i].v3 ].posz ) / 3.0f;

		triangles[i].radius = vector_length(triangles[i].middleX,
		                                    triangles[i].middleY,
		                                    triangles[i].middleZ,

		                                    verticles[ triangles[i].v1 ].posx,
											verticles[ triangles[i].v1 ].posy,
											verticles[ triangles[i].v1 ].posz
		);
			
		// set the used flag to false
		triangles[i].isUsed = false;

		// calculate the vector of the triangle:
		vector_crossproduct( verticles[ triangles[i].v1 ].posx - verticles[ triangles[i].v2 ].posx,
		                     verticles[ triangles[i].v1 ].posy - verticles[ triangles[i].v2 ].posy,
		                     verticles[ triangles[i].v1 ].posz - verticles[ triangles[i].v2 ].posz,
			
		                     verticles[ triangles[i].v1 ].posx - verticles[ triangles[i].v3 ].posx,
		                     verticles[ triangles[i].v1 ].posy - verticles[ triangles[i].v3 ].posy,
		                     verticles[ triangles[i].v1 ].posz - verticles[ triangles[i].v3 ].posz,
			
							 triangles[i].vx,
		                     triangles[i].vy,
		                     triangles[i].vz
		);

		vector_normalisize(triangles[i].vx, triangles[i].vy, triangles[i].vz);
	}
}
