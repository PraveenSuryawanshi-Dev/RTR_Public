/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/

#pragma once
#pragma once
#ifndef  _MODEL_PARSER_H_
#define _MODEL_PARSER_H_

#include<Windows.h>
#include"GLObject.h"
#include "CLGL.h"
#include"OBJLoader.h"
#include<gl/GL.h>




#define CHECK_ON_NULL_THROW_EXCEPTION(obj){if(obj==NULL){ delete obj;obj=NULL;throw "EXCEPTION: OBJECT NULL EXCEPTION";}} 

#define SAFE_DELETE(x){if(x){delete x;x=NULL;}}

#define SAFE_FREE(x){if(x){free(x);x=NULL;}}

using namespace HPP;

namespace ThreeDModelLoader
{

	typedef enum _process_type_
	{
		CPU,
		GPU

	}PROCESS_TYPE;


	typedef	enum _draw_type_
	{
		DRAW_USING_ARRAYS,
		DRAW_USING_ELEMENTS
	}DRAW_TYPE;

	class ModelParser
	{
	public:
		ModelParser(char *filenfullpathname);

		void parse(PROCESS_TYPE processtype, DRAW_TYPE drawtype);

		void draw();
		
		/*GLfloat * getVerticesArrayForDrawUsingArrays();
		int getSizeOfVerticesArrayForDrawUsingArrays();
		GLfloat * getNormalArray();
		int getSizeOfNormalArray();
		GLfloat * getTexturesCoords();
		int getSizeOfTexturesCoords();
		int getTexCordArrayCount();*/


		/*API FOR DRAW USING ELEMENTS*/
		/*GLfloat * getVerticesArrayForDrawUsingElements();
		int getSizeOfVerticesArrayForDrawUsingElements();
	
		unsigned int *getIndicesArray();
		int getSizeOfIndicesArray();
		
		int getIndicesArrayCount();


		GLfloat * getTexturesArrayForDrawUsingElements();
		int getSizeOfTexturesArrayForDrawUsingElements();

		unsigned int *getTexturesIndicesArray();
		int getSizeOfTexturesIndicesArray();



		GLfloat * getNormalsArrayForDrawUsingElements();
		int getSizeOfNormalsArrayForDrawUsingElements();

		unsigned int *getNormalsIndicesArray();
		int getSizeOfNormalsIndicesArray();*/
		/**/


		~ModelParser();


	private :
		void getTriangle(int position, OGLTRIANGLE * triangle);
		
		void reduceMesh();

	private:

		GLfloat *_pfvertices;
		GLfloat *_pfnormals;
		GLfloat *_pftexturesCoords;
		int texcord_cnt;

		GLfloat *_pfUniqueVertices;
		int _nUniqueVerticesIndexCnt;

		unsigned int *_pnVerticesIndex;
		int _nVerticesIndexCnt;


		GLfloat *_pfUniqueNormals;
		int _nUniqueNormalsIndexCnt;

		unsigned int *_pnNormalsIndex;
		int _nNormalsIndexCnt = 0;


		unsigned int *_pnTexturesIndex;
		int _nTexturesIndexCnt;

		GLfloat *_pfUniqueTextures;
		int _nUniqueTexturesIndexCnt;


		OBJLoader *_objLoader;
		
		PROCESS_TYPE _process_type;
		DRAW_TYPE _draw_type;

		CLGL *_ocl;

		GLObject *_gl_object;

		
	};

}

#endif