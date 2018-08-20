#pragma once
/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/
//https://software.intel.com/en-us/articles/sharing-surfaces-between-opencl-and-opengl-43-on-intel-processor-graphics-using-implicit
#ifndef _CL_GL_H
#define _CL_GL_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include<gl/GL.h>
#include<CL\opencl.h>
#include<CL\cl_ext.h>
#include<vector>


#define CHECK_OCL_CALL(ocl_status){ if(ocl_status != CL_SUCCESS) {cleanUP();return false;}}
#define CHECK_MALLOC(mem){if(mem == NULL){cleanUP();return false;}}

namespace HPP
{
	class CLGL
	{

	private:
		char* load_ocl_program_source_from_file(const char *filename, const char *preamble, size_t *sizeFinalLength);
		bool is_extension_supported(const char *support_str,const char*ext_string,size_t ext_buffer_size);
		cl_int is_CL_GL_supported();
		void cleanUP();

	public:
		CLGL();

		bool initialize_CL();

		bool test(cl_float *pfVertices,cl_int vertices_cnt, GLint vertice_vbo);
		
		bool reduce_vertices(cl_float *pfVertices,
			cl_int vertices_cnt,
			cl_uint *_pnVerticesIndex,
			cl_int _nVerticesIndexCnt,
			cl_int *verticesDuplcation,
			cl_float *_pfUniqueVertices,
			cl_int lengthofvertices,
			cl_int lenghtoffaces
			);

		
		~CLGL();


		 

	private:

		cl_platform_id _oclPlatformID;
		
		cl_device_id _oclComputeDeviceID;

		
		cl_context _oclContext;

		cl_command_queue _oclCommandQueue;

		cl_program _oclProgram; 

		cl_kernel _oclKernal; 

	};
}
#endif