#include "OpenCLW.h"

#include <fstream>

OpenCLw::OpenCLw() {
	cl_int ret;
	cl_uint numberOfPlatforms; // hold the number of available platforms
	cl_platform_id* platformIds; // this is a pointer to an array with (numberOfPlatforms) platforms that are available
	
	// query the count of the available platforms
	ret = clGetPlatformIDs(0, NULL, &numberOfPlatforms);
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Couldn't query the number of available Platforms!");
	}

	if( numberOfPlatforms == 0 ) {
		throw ErrorMessage("There are no Platforms!");
	}

	// allocate an memory region where the Platform id's can be saved
	platformIds = new cl_platform_id[numberOfPlatforms];
	
	if( !platformIds ) {
		// the array couln't be allocated
		throw ErrorMessage("Couldn't allocate array!");
	}

	
	// get all available platform ids
	ret = clGetPlatformIDs(numberOfPlatforms, platformIds, NULL);
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Couldn't query the available Platforms!");
	}

	/*
		this can be anytime helpful

	size_t size;
	char* stringInfo;

	// enumerate the informations about the device
	// TODO< enumerate the informations of multiple platforms if we have many and choose the right one >
	cl_platform_id id = platformIds[0];

	ret = clGetPlatformInfo(platformIds[0], CL_PLATFORM_EXTENSIONS , 0, NULL, &size); 
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Couldn't get informations about the Platform!"); 
	}
	
	stringInfo = new char[size];

	if( !stringInfo ) {
		throw ErrorMessage("Couldn't allocate array!");
	}

	ret = clGetPlatformInfo(platformIds[0], CL_PLATFORM_EXTENSIONS , size, stringInfo, NULL);
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Couldn't get informations about the Platform!"); 
	}
	
	delete stringInfo;
	*/

	// fill the prperties structure
	cl_context_properties akProperties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)platformIds[0],
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		0
	};


	// Create a context to run OpenCL
	GPUContext = clCreateContextFromType(akProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &ret); 
	if( !GPUContext ) {
		// figure out the error-code
		string errorText = getErrorText(ret);

		throw ErrorMessage("Could not create the GPU-Context(" + errorText + "!");
	}

	// Get the list of GPU devices associated with this context 
	size_t ParmDataBytes; 

	ret = clGetContextInfo(GPUContext, CL_CONTEXT_DEVICES, 0, NULL, &ParmDataBytes); 
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't get the context informations!");
	}
	
	GPUDevices = reinterpret_cast<cl_device_id*>(malloc(ParmDataBytes)); 
	if( !GPUDevices ) {
		throw NoMemory();
	}
	
	ret = clGetContextInfo(GPUContext, CL_CONTEXT_DEVICES, ParmDataBytes, GPUDevices, NULL); 
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't get the context informations!");
	}
	
	// Create a command-queue on the first GPU device 
	GPUCommandQueue = clCreateCommandQueue(GPUContext,
	                                       GPUDevices[0],
	                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | /*|*/ /* we want it for more speed */
	                                       CL_QUEUE_PROFILING_ENABLE ,              /* we want to profile the timings */
	                                       NULL);

	if( !GPUCommandQueue ) {
		throw ErrorMessage("Couldn't create the Command Queue!");
	}

	
	// free the array with the available platform ids
	delete platformIds;
}

OpenCLw::~OpenCLw() {
	for( list<Program*>::iterator programI = programs.begin(); programI != programs.end(); programI++ ) {
		delete (*programI);
	}

	// TODO< the same for images and buffers >

	free(GPUDevices);

	clReleaseContext(GPUContext);
	clReleaseCommandQueue(GPUCommandQueue);
}

OpenCLw::Program* OpenCLw::openAndCompile(string Filename) {
	char* buffer;
	cl_int ret;


	Program* program = new Program();
	if( !program ) {
		throw NoMemory();
	}

	ifstream f(Filename.c_str());
	
	if ( !f ) {
		throw CantOpenFile(Filename);
	}
	
	buffer = reinterpret_cast<char*>(malloc(1000000));
	if( !buffer ) {
		throw NoMemory();
	}

	f.get(buffer, 1000000-1, 0);
	buffer[f.gcount()] = 0; // delimit last byte

	f.close();

	program->program = clCreateProgramWithSource(GPUContext, 1, const_cast<const char**>(&buffer), NULL, NULL); 
	
	
	if( !program->program ) {
		throw ErrorMessage("Can't compile the Source!");
	}

	free(buffer);

	// Build the program (OpenCL JIT compilation) 
	ret = clBuildProgram(program->program, 0, NULL, NULL, NULL, NULL); 
	if( ret != CL_SUCCESS ) {
		char errBuff[4096]; // is a buffer for the error message

		// we get the compilation error
		clGetProgramBuildInfo(program->program, GPUDevices[0], CL_PROGRAM_BUILD_LOG, 4096, &errBuff, NULL);
		

		throw ErrorMessage("Can't build the program!");
	}
	
	// add the program object to the list
	programs.push_front(program);

	return program;
}

OpenCLw::Program::~Program() {
	clReleaseProgram(program);
}

cl_kernel OpenCLw::Program::CreateKernel(string Name) {
	cl_kernel kernel;
	
	kernel = clCreateKernel(program, Name.c_str(), NULL); 
	if( !kernel ) {
		throw ErrorMessage("Can't create the Kernel!");
	}

	return kernel;
}


OpenCLw::Buffer* OpenCLw::CreateBuffer(cl_mem_flags Flags, size_t Size, void *Ptr) {
	Buffer* buffer;
	
	buffer = new Buffer(this);
	if( !buffer ) {
		throw NoMemory();
	}

	buffer->buffer = clCreateBuffer(GPUContext, Flags, Size, Ptr, NULL);
	if( !buffer->buffer ) {
		throw ErrorMessage("Can't create the Buffer!");
	}
	
	// add the buffer object to the list
	buffers.push_front(buffer);
	
	return buffer;
}

OpenCLw::Buffer::Buffer(OpenCLw* OpenCLwPtr) {
	openclw = OpenCLwPtr;
}

OpenCLw::Buffer::~Buffer() {
	clReleaseMemObject(buffer);
}

cl_mem* OpenCLw::Buffer::getPtr() {
	return &buffer;
}

void OpenCLw::Buffer::write(cl_bool Blocking, const void *Ptr, size_t Size, size_t Offset, cl_event* Event, OpenCLw::EventList* Waitlist) {
	cl_int ret;

	if( Waitlist == NULL ) {
		ret = clEnqueueWriteBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, 0, 0, Event);
	}
	else {
		ret = clEnqueueWriteBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, Waitlist->events.getCount(), Waitlist->events.elements, Event);
	}

	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't write to OpenCL Buffer");
	}
}

void OpenCLw::Buffer::write(cl_bool Blocking, const void *Ptr, size_t Size, size_t Offset, cl_event* Event, cl_uint WaitlistCount, cl_event* Waitlist) {
	cl_int ret;

	ret = clEnqueueWriteBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, WaitlistCount, Waitlist, Event);
	
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't write to OpenCL Buffer");
	}
}

void OpenCLw::Buffer::read(cl_bool Blocking, void *Ptr, size_t Size, size_t Offset, cl_event* Event, OpenCLw::EventList* Eventlist) {
	cl_int ret;

	if( Eventlist == NULL ) {
		ret = clEnqueueReadBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, 0, 0, Event);
	}
	else {
		ret = clEnqueueReadBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, Eventlist->events.getCount(), Eventlist->events.elements, Event);
	}

	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't read from OpenCL Buffer");
	}
}

void OpenCLw::Buffer::read(cl_bool Blocking, void *Ptr, size_t Size, size_t Offset, cl_event* Event, cl_uint WaitlistCount, cl_event* Waitlist) {
	cl_int ret;

	ret = clEnqueueReadBuffer(openclw->GPUCommandQueue, buffer, Blocking, Offset, Size, Ptr, WaitlistCount, Waitlist, Event);
	
	if( ret != CL_SUCCESS ) {
		throw ErrorMessage("Can't read from OpenCL Buffer");
	}
}

void* OpenCLw::Buffer::map(cl_bool Blocking, cl_map_flags Flags, size_t Size, size_t Offset) {
	void* ret = clEnqueueMapBuffer(openclw->GPUCommandQueue, buffer, Blocking, Flags, Offset, Size, 0, 0, 0, 0);
	if( !ret ) {
		throw MappingFailed();
	}

	return ret;
}

OpenCLw::Image* OpenCLw::CreateImageFromOpenGLTexture(cl_mem_flags Flags, GLuint Texture) {
	cl_mem ret;
	cl_int errorcode;
	
	ret = clCreateFromGLTexture2D(GPUContext, Flags, GL_TEXTURE_2D, 0, Texture, &errorcode);

	if( errorcode != CL_SUCCESS ) {
		string code = getErrorText(errorcode);
		throw ErrorMessage("Can't create the OpenCL Texture from the OpenGL Texture!");
	}

	// create a new Image Object
	OpenCLw::Image* newImage = new OpenCLw::Image();
	
	if( !newImage ) {
		throw NoMemory();
	}

	newImage->image = ret;

	// append the new image to the LL
	images.push_front(newImage);

	return newImage;
}

OpenCLw::Image::Image() {

}

OpenCLw::Image::~Image() {

}

string OpenCLw::getErrorText(cl_int Errorcode) {
	string ret = "?";

	switch(Errorcode) {
		case CL_INVALID_COMMAND_QUEUE:
		ret = "CL_INVALID_COMMAND_QUEUE";
		break;

		case CL_INVALID_CONTEXT:
		ret = "CL_INVALID_CONTEXT";
		break;

		case CL_INVALID_DEVICE_TYPE:
		ret = "CL_INVALID_DEVICE_TYPE";
		break;

		case CL_INVALID_EVENT_WAIT_LIST:
		ret = "CL_INVALID_EVENT_WAIT_LIST";
		break;

		case CL_INVALID_GL_OBJECT:
		ret = "CL_INVALID_GL_OBJECT";
		break;

		case CL_INVALID_GLOBAL_OFFSET:
		ret = "CL_INVALID_GLOBAL_OFFSET";
		break;

		case CL_INVALID_KERNEL:
		ret = "CL_INVALID_KERNEL";
		break;

		case CL_INVALID_KERNEL_ARGS:
		ret = "CL_INVALID_KERNEL_ARGS";
		break;

		case CL_INVALID_PROGRAM_EXECUTABLE:
		ret = "CL_INVALID_PROGRAM_EXECUTABLE";
		break;

		case CL_INVALID_VALUE:
		ret = "CL_INVALID_VALUE";
		break;

		case CL_INVALID_WORK_DIMENSION:
		ret = "CL_INVALID_WORK_DIMENSION";
		break;

		case CL_INVALID_WORK_GROUP_SIZE:
		ret = "CL_INVALID_WORK_GROUP_SIZE";
		break;

		case CL_INVALID_WORK_ITEM_SIZE:
		ret = "CL_INVALID_WORK_ITEM_SIZE";
		break;
		
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		ret = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		break;
		
		case CL_OUT_OF_HOST_MEMORY:
		ret = "CL_OUT_OF_HOST_MEMORY";
		break;

		case CL_OUT_OF_RESOURCES:
		ret = "CL_OUT_OF_RESOURCES";
		break;

		/// --- until here sorted

		case CL_DEVICE_NOT_AVAILABLE:
		ret = "CL_DEVICE_NOT_AVAILABLE";
		break;

		case CL_DEVICE_NOT_FOUND:
		ret = "CL_DEVICE_NOT_FOUND";
		break;


		//case CL_INVALID_MIPLEVEL:
		//ret = "CL_INVALID_MIPLEVEL";
		//break;

		
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		ret = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		break;

		case CL_INVALID_PLATFORM:
		ret = "CL_INVALID_PLATFORM";
		break;
	}

	return ret;
}

cl_mem* OpenCLw::Image::getPtr() {
	return &image;
}
