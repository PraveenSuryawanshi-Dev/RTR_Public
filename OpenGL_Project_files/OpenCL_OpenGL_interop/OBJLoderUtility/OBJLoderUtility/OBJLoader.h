/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/
#pragma once
#ifndef  _OBJ_LOADER_
#define _OBJ_LOADER_
#include<Windows.h>
#include<gl/GL.h>
#include <vector>
#include "MTLLoader.h"

#define CHECK_ON_NULL_THROW_EXCEPTION(obj){if(obj == NULL){ delete obj;obj=NULL;throw "EXCEPTION: OBJECT NULL EXCEPTION";}} 
#define SAFE_CLEAR_VECTOR_AND_FREE(x){if(x){x->clear();delete x;x=NULL;}}
#define SAFE_DELETE(x) { if (x) { delete x; x = NULL; } }

namespace ThreeDModelLoader
{
	typedef struct _vertex_
	{
		GLfloat X1;
		GLfloat X2;
		GLfloat X3;

	}VERTEX;


	typedef struct _texture_coord_
	{
		GLfloat S;
		GLfloat T;
		GLfloat R;
		GLfloat Q;

	}TEXTURE_CORD;


	typedef struct _normal_
	{
		GLfloat N1;
		GLfloat N2;
		GLfloat N3;

	}NORMAL;
	
	
	typedef struct _oglTriangle_
	{
		VERTEX FirstPointVertices;
		TEXTURE_CORD FirstPointTextureCoord;
		NORMAL FirstPointNormals;

		VERTEX SecondPointVertices;
		TEXTURE_CORD SecondPointTextureCoord;
		NORMAL SecondPointNormals;

		VERTEX ThirdPointVertices;
		TEXTURE_CORD ThirdPointTextureCoord;
		NORMAL ThirdPointNormals;

	}OGLTRIANGLE;



	typedef struct  _trianglefaceindices_
	{
		/*NOTE:
		* V:Vertex
		* T:TEXTURE
		* N:NORMALS
		*/

		int V1;
		int T1;
		int N1;

		int V2;
		int T2;
		int N2;

		int V3;
		int T3;
		int N3;
	}TRIANGLE_FACE_INDICES;


	class OBJLoader
	{
		const char *separater_space = " ";
		const char *separater_slash = "/";

	private:

		int getTotalTokens(char *line);

		bool isVertex(char* token);
		void onVertex();
		
		bool isTextureCoords(char* token);
		void onTextureCoords(int token_count);
		

		bool isNormal(char* token);
		void onNormal();
		

		bool isFaces(char* token);
		void onFaces();

		bool isMtlLib(char* token);
		void onMtlLib();

		bool isStartUseMtl(char * token);
		void onStartUseMtl();

		bool isEndUseMtl(char * token);
		void onEndUseMtl();

		void mapOBJToMaterials();

		int equals(char *src, char *dest);

	public:
		OBJLoader(char *szfileFullPath);

		void parse();


		int getTexCordArrayCount();

		std::vector<VERTEX>  * getVertices();

		std::vector<TEXTURE_CORD> *getTextureCoordinates();

		std::vector<NORMAL>  *getNormals();

		std::vector<TRIANGLE_FACE_INDICES>  *getFaces();

		~OBJLoader();

	private:
		


		typedef struct _model_indices_material_map_table_
		{
			char materialName[256];
			int material_index;
			int start_index;
			int end_index;

		}MODEL_INDICES_MAP_TABLE;


		std::vector<VERTEX>  *_pvVertices;

		std::vector<TEXTURE_CORD> *_pvTexturesCoords;
		
		std::vector<NORMAL>  *_pvNormals;
		
		std::vector<TRIANGLE_FACE_INDICES>  *_pvFaces;

		std::vector<MODEL_INDICES_MAP_TABLE*>  *_pvModelIndicesMapTable;
	

		int _nTexCordCount;


		char *_szfileFullPath;

		MTLLoader *_pMtlLoader;

		MODEL_INDICES_MAP_TABLE *_model;


	};

#endif // ! 

}
