#include "CLGL.h"
/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/




bool HPP::CLGL::is_extension_supported(const char * support_str,const char * ext_string, size_t ext_buffer_size)
{
	char *extension_buffer = (char*)malloc(sizeof(char)*ext_buffer_size);
	CHECK_MALLOC(extension_buffer)

	strcpy(extension_buffer, ext_string);

	char *token = NULL;
	token = strtok(extension_buffer, " ");
	while (true)
	{
		if (strcmp(token, support_str) == 0)
			return true;

		token = strtok(NULL, " ");
		if (token == NULL)
			break;		
	}

	return false;
}

void HPP::CLGL::cleanUP()
{
	int a = 0;
}



HPP::CLGL::CLGL()
{
	
	
}

bool HPP::CLGL::initialize_CL()
{
	cl_int ocl_status;


	CHECK_OCL_CALL(clGetPlatformIDs(1, &_oclPlatformID, NULL))

	CHECK_OCL_CALL(clGetDeviceIDs(_oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &_oclComputeDeviceID, NULL))

	CHECK_OCL_CALL(is_CL_GL_supported())

	cl_context_properties ocl_properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)_oclPlatformID,
		0
	};

	// Find CL capable devices in the current GL context
	cl_device_id devices[32]; size_t size;
	clGetGLContextInfoKHR_fn pclGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddressForPlatform(_oclPlatformID, "clGetGLContextInfoKHR");
	pclGetGLContextInfoKHR(ocl_properties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,32 * sizeof(cl_device_id), devices, &size);

	// Create a context using the supported devices
	int count = size / sizeof(cl_device_id);


	_oclContext = clCreateContext(ocl_properties, count, &_oclComputeDeviceID, NULL, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status);


	
	size_t lengthOfFile = 0;
	const char* szOpenCLKernelCode = load_ocl_program_source_from_file("vertices_reduction.cl", "", &lengthOfFile);
	_oclProgram = clCreateProgramWithSource(_oclContext, 1, &szOpenCLKernelCode, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clBuildProgram(_oclProgram, 0, NULL, NULL, NULL, NULL);
	if (ocl_status != CL_SUCCESS)
	{
		size_t len;
		char buffer[20480];
		clGetProgramBuildInfo(_oclProgram, _oclComputeDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		cleanUP();
		return  false;

	}

	_oclKernal = clCreateKernel(_oclProgram, "vertices_reduction_function", &ocl_status);
	CHECK_OCL_CALL(ocl_status)

	return true;
}

bool HPP::CLGL::test(cl_float * pfVertices, cl_int vertices_cnt, GLint vertice_vbo)
{
	///////////////////////Allocate Buffers///////////////////////
	cl_mem  pfVertices_deviceInput = NULL;
	cl_int ocl_status;
	int size = vertices_cnt * sizeof(cl_float);
	// allocate device-memory
	pfVertices_deviceInput = clCreateBuffer(_oclContext, CL_MEM_READ_ONLY, size, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)

	//////////////////////////////////////////////////////////////
	cl_mem cl_gl_buffer = clCreateFromGLBuffer(_oclContext, CL_MEM_READ_WRITE, vertice_vbo, &ocl_status);
	CHECK_OCL_CALL(ocl_status)


	///////////////////////SET KERNEL ARGS///////////////////////

	ocl_status = clSetKernelArg(_oclKernal, 0, sizeof(cl_mem), (void *)&pfVertices_deviceInput);
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clSetKernelArg(_oclKernal, 1, sizeof(cl_int), (void *)&vertices_cnt);
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clSetKernelArg(_oclKernal, 2, sizeof(cl_mem), (void *)&cl_gl_buffer);
	CHECK_OCL_CALL(ocl_status)

	///////////////////////WRITE TO BUFFRES ///////////////////////
		size = vertices_cnt * sizeof(cl_float);
	ocl_status = clEnqueueWriteBuffer(_oclCommandQueue, pfVertices_deviceInput, CL_FALSE, 0, size, pfVertices, 0, NULL, NULL);
	CHECK_OCL_CALL(ocl_status)

		// run the kernel
		size_t global_size = vertices_cnt / 3;
	ocl_status = clEnqueueNDRangeKernel(_oclCommandQueue, _oclKernal, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	CHECK_OCL_CALL(ocl_status)

		
		
	CHECK_OCL_CALL(clEnqueueAcquireGLObjects(_oclCommandQueue, 1, &cl_gl_buffer, 0, NULL, NULL))
	ocl_status = clFinish(_oclCommandQueue);
	CHECK_OCL_CALL(clEnqueueReleaseGLObjects(_oclCommandQueue, 1, &cl_gl_buffer, 0, NULL, NULL))
	CHECK_OCL_CALL(ocl_status)

		return true;
}

bool HPP::CLGL::reduce_vertices(cl_float *pfVertices,cl_int vertices_cnt,cl_uint *_pnVerticesIndex,cl_int _nVerticesIndexCnt,
		cl_int *verticesDuplcation,
		cl_float *_pfUniqueVertices,
		cl_int lengthofvertices,
		cl_int lenghtoffaces
)
{
	

	cl_mem  pfVertices_deviceInput = NULL;
	cl_mem _pnVerticesIndex_deviceInput = NULL;
	cl_mem verticesDuplcation_deviceInput = NULL;
	cl_mem _pfUniqueVertices_deviceInput = NULL;

	cl_int ocl_status;
	

	_oclCommandQueue = clCreateCommandQueue(_oclContext, _oclComputeDeviceID, 0, &ocl_status);
	CHECK_OCL_CALL(ocl_status)


	

	///////////////////////Allocate Buffers///////////////////////

	int size = vertices_cnt * sizeof(cl_float);
	// allocate device-memory
	pfVertices_deviceInput = clCreateBuffer(_oclContext, CL_MEM_READ_ONLY, size, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)
	

	
	size = _nVerticesIndexCnt * sizeof(unsigned int);
	// allocate device-memory
	_pnVerticesIndex_deviceInput = clCreateBuffer(_oclContext, CL_MEM_READ_WRITE, size, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)

	
	size = lengthofvertices * sizeof(cl_uint) * 2;
	// allocate device-memory
	verticesDuplcation_deviceInput = clCreateBuffer(_oclContext, CL_MEM_WRITE_ONLY, size, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)

	size = lenghtoffaces * sizeof(cl_float) * 3;
	// allocate device-memory
	_pfUniqueVertices_deviceInput = clCreateBuffer(_oclContext, CL_MEM_WRITE_ONLY, size, NULL, &ocl_status);
	CHECK_OCL_CALL(ocl_status)


	///////////////////////SET KERNEL ARGS///////////////////////

	ocl_status = clSetKernelArg(_oclKernal, 0, sizeof(cl_mem), (void *)&pfVertices_deviceInput); 
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clSetKernelArg(_oclKernal, 1, sizeof(cl_int), (void *)&vertices_cnt);
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clSetKernelArg(_oclKernal, 2, sizeof(cl_mem), (void *)&_pnVerticesIndex_deviceInput); 
	CHECK_OCL_CALL(ocl_status)

	ocl_status = clSetKernelArg(_oclKernal, 3, sizeof(cl_int), (void *)&_nVerticesIndexCnt);
	CHECK_OCL_CALL(ocl_status)
		
	ocl_status = clSetKernelArg(_oclKernal, 4, sizeof(cl_mem), (void *)&verticesDuplcation_deviceInput); 
	CHECK_OCL_CALL(ocl_status)
		
	ocl_status = clSetKernelArg(_oclKernal, 5, sizeof(cl_mem), (void *)&_pfUniqueVertices_deviceInput); 
	CHECK_OCL_CALL(ocl_status)
	
	///////////////////////WRITE TO BUFFRES ///////////////////////
	size = vertices_cnt * sizeof(cl_float);
	ocl_status = clEnqueueWriteBuffer(_oclCommandQueue, pfVertices_deviceInput, CL_FALSE, 0, size, pfVertices, 0, NULL, NULL);
	CHECK_OCL_CALL(ocl_status)


	size = _nVerticesIndexCnt * sizeof(unsigned int);
	ocl_status = clEnqueueWriteBuffer(_oclCommandQueue, _pnVerticesIndex_deviceInput, CL_FALSE, 0, size, _pnVerticesIndex, 0, NULL, NULL);
	CHECK_OCL_CALL(ocl_status)

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// run the kernel
	size_t global_size = vertices_cnt /3;
	ocl_status = clEnqueueNDRangeKernel(_oclCommandQueue,_oclKernal, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	CHECK_OCL_CALL(ocl_status)


	ocl_status= clFinish(_oclCommandQueue);
	CHECK_OCL_CALL(ocl_status)

	return true;
}

HPP::CLGL::~CLGL()
{
}

cl_int HPP::CLGL::is_CL_GL_supported()
{
	cl_int cl_status;
	static const char * CL_GL_SHARING_EXT = "cl_khr_gl_sharing";

	
	size_t ext_size = 1024;
	char* ext_string = (char*)malloc(ext_size);
	CHECK_MALLOC(ext_string)

	CHECK_OCL_CALL(clGetDeviceInfo(_oclComputeDeviceID, CL_DEVICE_EXTENSIONS, ext_size, ext_string, &ext_size));


	bool is_supported = is_extension_supported(CL_GL_SHARING_EXT, ext_string, ext_size);
	return (is_supported == true)? CL_SUCCESS: CL_INVALID_VALUE;
}


char * HPP::CLGL::load_ocl_program_source_from_file(const char * filename, const char * preamble, size_t * sizeFinalLength)
{
	// locals
	FILE *pFile = NULL;
	size_t sizeSourceLength;

	pFile = fopen(filename, "rb"); // binary read
	if (pFile == NULL)
		return(NULL);

	size_t sizePreambleLength = (size_t)strlen(preamble);

	// get the length of the source code
	fseek(pFile, 0, SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET); // reset to beginning

							   // allocate a buffer for the source code string and read it in
	char *sourceString = (char *)malloc(sizeSourceLength + sizePreambleLength + 1);
	memcpy(sourceString, preamble, sizePreambleLength);
	if (fread((sourceString)+sizePreambleLength, sizeSourceLength, 1, pFile) != 1)
	{
		fclose(pFile);
		free(sourceString);
		return(0);
	}

	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFile);
	if (sizeFinalLength != 0)
	{
		*sizeFinalLength = sizeSourceLength + sizePreambleLength;
	}
	sourceString[sizeSourceLength + sizePreambleLength] = '\0';

	return(sourceString);
}
