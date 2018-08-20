/*
* @author: Praveen Suryawanshi
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/

#pragma once
#pragma once
#ifndef  _MODEL_PARSER_


#define _MODEL_PARSER_
#include<Windows.h>
#include<Windows.h>
#include<gl/GL.h>
#include"OBJLoader.h"

namespace ThreeDModelLoader
{
	class ModelParser
	{
	public:
		ModelParser(char *filenfullpathname);

		void parse(bool useHpp);
		
		GLfloat * getVerticesArrayForDrawUsingArrays();
		int getSizeOfVerticesArrayForDrawUsingArrays();


		GLfloat * getTexturesCoords();
		int getSizeOfTexturesCoords();
		int getTexCordArrayCount();


		GLfloat * getVerticesArrayForDrawUsingElements();
		int getSizeOfVerticesArrayForDrawUsingElements();
		unsigned int *getIndicesArray();
		int getSizeOfIndicesArray();
		int getIndicesArrayCount();
		

		GLfloat * getNormalArray();
		int getSizeOfNormalArray();

		~ModelParser();


	private :
		void getTriangle(int position, OGLTRIANGLE * triangle);
		
		void reduceMeshOnCPU();

	private:

		GLfloat *_pfvertices;
		GLfloat *_pfnormals;
		GLfloat *_pftexturesCoords;

		GLfloat *pfUniqueVertices;
		int unique_vertices_index_cnt;

		unsigned int *pnVerticesIndex;
		int vertices_index_cnt;

		OBJLoader *_objLoader;

		
	};

}

#endif