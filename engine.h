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
		void recalculate() {
			vector_normalisize(look.x, look.y, look.z);

			vector_crossproduct(look.x, look.y, look.z, up.x, up.y, up.z, cameraX.x, cameraX.y, cameraX.z);
			vector_crossproduct(cameraX.x, cameraX.y, cameraX.z, look.x, look.y, look.z, cameraY.x, cameraY.y, cameraY.z);

			vector_normalisize(cameraX.x, cameraX.y, cameraX.z);
			vector_normalisize(cameraY.x, cameraY.y, cameraY.z);
		}
		
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
	void CheckGLError() {
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
			throw class GLError("OpenGL error: " + desc);
		}
	}

	// throws GLError, NoMemory
	RayEngine() {
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

		
		////// init the Perlin Noise
		////PerlinNoise::init();

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
			throw class NoMemory();
		}

		rays2     = reinterpret_cast<struct rays2*>(reinterpret_cast<long>(rays2_raw) & 0xFFFFFF00);
		
		// create the memory area for the opencl_shading array
		/*opencl_shading = reinterpret_cast<struct opencl_shading_in*>( malloc(sizeof(opencl_shading_in)*(TILESIZE_X*TILESIZE_Y)) );
		if( !opencl_shading ) {
			throw class NoMemory();
		}*/

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
			throw new class GeneralError("Kernel Arg 1");
		}


		ret = clSetKernelArg(opencl_shade_kernel,                    /* is the shading kernel */
		               1,                                      /* is the argument index */
		               sizeof(cl_mem),                         /* size of the argument */
		               reinterpret_cast<void*>(opencl_shading_outBinned_buffer->getPtr()) /* argument: is our buffer */
					  );

		
		if( ret != CL_SUCCESS ) {
			throw new class GeneralError("Kernel Arg 2");
		}


		ret = clSetKernelArg(opencl_shade_kernel,
		               2,
		               sizeof(cl_mem),
					   reinterpret_cast<void*>(opencl_perlinNoiseArray->getPtr())
		);

		
		if( ret != CL_SUCCESS ) {
			throw new class GeneralError("Kernel Arg 3");
		}


		// we allocate enougth space for the Allocators and Lock them
		nodeAllocator.reallocateElements(5000);
		nodeAllocator.setLock(true);

		contentAllocator.reallocateElements(1000);
		contentAllocator.setLock(true);
	}

	~RayEngine() {
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
	
	class InvalidResolution {
	};

	// this sets the resolution of the renderer
	// throws InvalidResolution if the resolution is not valid
	void setResolution(unsigned long X, unsigned long Y) {
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

		// allocate the space
		// TODO< make this resolution dependent >

		// this is just a test
		
		char* arr = new char[4*1024*1024];
		for( unsigned int i = 0; i < 4*1024*1024; i++ ) {
			arr[i] = 0;
		}

		for( unsigned int x = 0; x < 512; x++ ) {
			for( unsigned int y = 0; y < 512; y++ ) {
				arr[y*(4*1024) + x*4 + 0] = 255; // r
				arr[y*(4*1024) + x*4 + 1] = 255; // g
				arr[y*(4*1024) + x*4 + 2] = 255; // b
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, arr);
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
	
	// this creates a new Object with the Mesh as the "hull"
	// throws NoMemory
	Object* createObject(Mesh* Mesh) {
		Object* nobject = new Object(Mesh);

		if( !nobject ) {
			throw class NoMemory();
		}

		// add the new object to the object ll of the engine
		objects.push_back(nobject);

		return nobject;
	}

	// this tries to build up the best spherical fit of the Triangles in the Array
	void buildSpheres() {
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


	// this builds up out of the engine verticle/polygon/BSC data a Mesh and return a pointer to it
	// it throws noMemory
	Mesh* createMesh() {
		Mesh* nmesh = new Mesh();
		
		if( !nmesh ) {
			throw class NoMemory();
		}
		
		// copy all verticles
		for( unsigned long i = 0; i < verticlesC; i++ ) {
			Mesh::Verticle_primary* nverticle = new Mesh::Verticle_primary();

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

	// this transform all objects(with the meshs) into the global data 
	void transformObjectsIntoWorld() {
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
				bvhcontent->bb_minx = bb_min.x;
				bvhcontent->bb_miny = bb_min.y;
				bvhcontent->bb_minz = bb_min.z;
				
				bvhcontent->bb_maxx = bb_max.x;
				bvhcontent->bb_maxy = bb_max.y;
				bvhcontent->bb_maxz = bb_max.z;
				
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

	// this is called by rebuildTree and rebuildTree2 calls it self for each level of deep
	// TODO< maybe rebuild this as a non-recursive algorithm? >

	// IntoNode : into which Node should the result go?
	// Content  : what content should go into that Node?
	// ContentC : is the count of the Content
	void rebuildTree2(BVH_Node* IntoNode, BVH_Content** Content, unsigned long ContentC) {
		// TODO

		// first we search the "biggest" dimension (the dimension in that the elements to have the most "width")
		BoundingBox bb;

		for( unsigned long contentI = 0; contentI < ContentC; contentI++ ) {
			const BVH_Content* bvhcontent = Content[contentI];
			
			bb.expand(bvhcontent->bb_minx, bvhcontent->bb_miny, bvhcontent->bb_minz, bvhcontent->bb_maxx, bvhcontent->bb_maxy, bvhcontent->bb_maxz);
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

	// this delete the whoole tree and rebuilds it from the BScontainer-Array that was calculated in the method buildSpheres
	// NOTE< it calls recursivly rebuildTree2 >
	void rebuildTree() {
		// free all allocated elements
		nodeAllocator.reset();
		
		// first we search the "biggest" dimension (the dimension in that the elements to have the most "width")
		BoundingBox bb;

		for( unsigned long contentI = 0; contentI < contentAllocator.getSize(); contentI++ ) {
			const BVH_Content* bvhcontent = contentAllocator[contentI];
			
			bb.expand(bvhcontent->bb_minx, bvhcontent->bb_miny, bvhcontent->bb_minz, bvhcontent->bb_maxx, bvhcontent->bb_maxy, bvhcontent->bb_maxz);
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

	// this imports a obj Object
	void importObj(string name) {
		objImporter->import(name);
	}


	class NoCamera {
	};

	// this renders the image (to the screen, the member do lock it)
	// throws:
	//    InvalidResolution
	//       If the resolution is not seted
	//    NoCamera
	//       If the camera was not seted
	void render(SDL_Surface* Screen) {
		global Global; // is the "local" "global" data (cos on the stack its faster and more practical than class-object wide u know...)
		perThread PerThread; // per thread structure // TODO< do align it for sse things >

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

		// rebuild everything
		//  transform
		transformObjectsIntoWorld();
		//  rebuild the tree
		rebuildTree();
		
		///if( SDL_MUSTLOCK(Screen) ) {
		///	SDL_LockSurface(Screen);
		///}

		///// overdraw all with black
		///SDL_Rect rect;
		///rect.x = 0;
		///rect.y = 0;
		///rect.w = static_cast<Uint16>(resolution.x);
		///rect.h = static_cast<Uint16>(resolution.y);
		///SDL_FillRect(Screen, &rect, SDL_MapRGB(Screen->format, 0, 0, 0));

		UniTimer.sum = 0;

		// this is the loop compatible to openMP
		
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

					// TODO< more general algorithm > !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					
					// this are the structures for the ray-AABB test
					aabbRay test_aabbray;

					// set up the aabb ray-structure
					setup_aabb_ray(Global.cameraXPos, Global.cameraYPos, Global.cameraZPos,
					               PerThread.cameraDirX, PerThread.cameraDirY, PerThread.cameraDirZ,
								   &test_aabbray);
					

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
							for( unsigned long contenti = 0; contenti < actualNode->contentc; contenti++ ) {
								test_raypacket_BVHContent_(rays2, actualNode->content[contenti]);
							}
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
								// we push the children
								nodeStack.push(actualNode->childrens[0]);
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
								// we push the children
								nodeStack.push(actualNode->childrens[1]);
							}
						}
					}
					
					// we copy the ray data to binned Memory of the OpenCL-device
					for( fasti rayI = 0; rayI < (32/4)*32; rayI++ ) {
						// 8x8 cos we use 8x8 ray-packets
						//unsigned long addr = 8*8 * (xsection + ysection * (TILESIZE_X/8)) + rayI * 4;
						
						
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
						
						addr1->posX = Global.cameraXPos + rp->deep._[1] * rp->vx.m128_f32[1];
						addr1->posY = Global.cameraYPos + rp->deep._[1] * rp->vy.m128_f32[1];
						addr1->posZ = Global.cameraZPos + rp->deep._[1] * rp->vz.m128_f32[1];
						
						addr2->posX = Global.cameraXPos + rp->deep._[2] * rp->vx.m128_f32[2];
						addr2->posY = Global.cameraYPos + rp->deep._[2] * rp->vy.m128_f32[2];
						addr2->posZ = Global.cameraZPos + rp->deep._[2] * rp->vz.m128_f32[2];

						addr3->posX = Global.cameraXPos + rp->deep._[3] * rp->vx.m128_f32[3];
						addr3->posY = Global.cameraYPos + rp->deep._[3] * rp->vy.m128_f32[3];
						addr3->posZ = Global.cameraZPos + rp->deep._[3] * rp->vz.m128_f32[3];
						
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

			// we let the openCL device shade it and we can calculate the shadows (and maybe reflections/refraction?)			
			opencl_shading_buffer->write(CL_FALSE /* not blocking */,
			                                          opencl_shading,
													  (TILESIZE_X*tiles.x * TILESIZE_Y*tiles.y) * sizeof(struct opencl_shading_in),
													  0,
													  &opencl_shading_events, /* we want that this emits a event */
													  0,
													  0);
			
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
			
			if( ret_ !=  CL_SUCCESS ) {
				string desc = "";

				switch( ret_ ) {
					case CL_INVALID_PROGRAM_EXECUTABLE:
					desc = "CL_INVALID_PROGRAM_EXECUTABLE";
					break;
					
					case CL_INVALID_COMMAND_QUEUE:
					desc = "CL_INVALID_COMMAND_QUEUE";
					break;

					case CL_INVALID_KERNEL:
					desc = "CL_INVALID_KERNEL";
					break;

					case CL_INVALID_CONTEXT:
					desc = "CL_INVALID_CONTEXT";
					break;

					case CL_INVALID_KERNEL_ARGS:
					desc = "CL_INVALID_KERNEL_ARGS";
					break;

					case CL_INVALID_WORK_DIMENSION:
					desc = "CL_INVALID_WORK_DIMENSION";
					break;

					case CL_INVALID_WORK_GROUP_SIZE:
					desc = "CL_INVALID_WORK_GROUP_SIZE";
					break;

					case CL_INVALID_WORK_ITEM_SIZE:
					desc = "CL_INVALID_WORK_ITEM_SIZE";
					break;

					case CL_INVALID_GLOBAL_OFFSET:
					desc = "CL_INVALID_GLOBAL_OFFSET";
					break;

					case CL_OUT_OF_RESOURCES:
					desc = "CL_OUT_OF_RESOURCES";
					break;
					
					case CL_MEM_OBJECT_ALLOCATION_FAILURE:
					desc = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
					break;

					case CL_INVALID_EVENT_WAIT_LIST:
					desc = "CL_INVALID_EVENT_WAIT_LIST";
					break;

					case CL_OUT_OF_HOST_MEMORY:
					desc = "CL_OUT_OF_HOST_MEMORY";
					break;

					default:
					desc = "UNKNOWN";
					break;
				}

				unsigned long a = 0;

				throw new class GeneralError("OpenCL");
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
			
			// after this it is safe to use OpenGL
			ret_ = clFinish(opencl.testcode_getCQ());

			if( ret_ != CL_SUCCESS ) {
				// TODO< analyse the error >

				throw new class GeneralError("OpenCL");
			}
		

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





		// debug the time
		cout << UniTimer.sum/2 << " ns" << endl;
		cout << UniTimer.sum/2000 << " us" << endl;
		cout << UniTimer.sum/2000000 << " ms" << endl;
		cout << static_cast<double>(UniTimer.sum)/2000000000.0 << " sec" << endl;
		
		cout << endl;
	}
   
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
	
	OpenCLw::Buffer*    opencl_perlinNoiseArray;
	
	// openGL
	GLuint oglDisplayTexture; // is the texture for the Display
	OpenCLw::Image* oclDisplayTexture; // is the openCL image for the Display texture

	// calculate the area and the bounding-sphere of the triangles
	// NOTE< needs valid data >
	void calculateTriangles() {
		for( unsigned i = 0; i < trianglesC; i++ ) {
			// calculate the area of the triangle:
			triangles[i].area = calc_polygon_area(verticles[ triangles[i].v1 ].posx,
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
