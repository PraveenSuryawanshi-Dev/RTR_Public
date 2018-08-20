/*
* @author: Praveen Suryawanshi
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/

#include "ModelParser.h"
#include"OBJLoader.h"
#include<math.h>

ThreeDModelLoader::ModelParser::ModelParser(char * filenfullpathname)
{
	_pfvertices = NULL;
	_pfnormals = NULL;
	_pftexturesCoords = NULL;
	_objLoader = NULL;
	pfUniqueVertices = NULL;
	unique_vertices_index_cnt = 0;
	pnVerticesIndex = NULL;
	vertices_index_cnt = 0;

	_objLoader = new OBJLoader(filenfullpathname);
	
	if (_objLoader == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";

	_objLoader->parse();

	if (_objLoader->getVertices()->size() > 0)
	{
		_pfvertices = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3); // 9 because of  3 vertices with 3 points x,y,z i.e 3*3
		
		if (_pfvertices == NULL)
			throw "EXCEPTION: OBJECT NULL EXCEPTION";
	}

	if (_objLoader->getTextureCoordinates()->size()>0)
	{
		if (_objLoader->getTexCordArrayCount() == 2)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3/*vertices*/ * 2 /*cords*/);

		if (_objLoader->getTexCordArrayCount() == 3)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3);

		if (_objLoader->getTexCordArrayCount() == 4)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 4);

		if (_pftexturesCoords == NULL)
			throw "EXCEPTION: OBJECT NULL EXCEPTION";
	}

	
	if (_objLoader->getNormals()->size() > 0)
	{
		_pfnormals = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3);
		
		if (_pfnormals == NULL)
			throw "EXCEPTION: OBJECT NULL EXCEPTION";
	}
}

void ThreeDModelLoader::ModelParser::parse(bool useHpp)
{
	
	int vertices_cnt = 0;
	int texcord_cnt = 0;
	int normals_cnt = 0;

		for (size_t i = 0; i < (_objLoader->getFaces()->size()); i++)
		{
			OGLTRIANGLE triangle;

			getTriangle(i, &triangle);

			/*filling vertices*/
			if (_pfvertices != NULL && _objLoader->getVertices()->size() > 0)
			{
				_pfvertices[vertices_cnt++] = triangle.FirstPointVertices.X1;
				_pfvertices[vertices_cnt++] = triangle.FirstPointVertices.X2;
				_pfvertices[vertices_cnt++] = triangle.FirstPointVertices.X3;

				_pfvertices[vertices_cnt++] = triangle.SecondPointVertices.X1;
				_pfvertices[vertices_cnt++] = triangle.SecondPointVertices.X2;
				_pfvertices[vertices_cnt++] = triangle.SecondPointVertices.X3;

				_pfvertices[vertices_cnt++] = triangle.ThirdPointVertices.X1;
				_pfvertices[vertices_cnt++] = triangle.ThirdPointVertices.X2;
				_pfvertices[vertices_cnt++] = triangle.ThirdPointVertices.X3;
			}
			/**/



			/*filling color*/
			if (_pftexturesCoords != NULL && _objLoader->getTextureCoordinates()->size() > 0)
			{
				if (_objLoader->getTexCordArrayCount() == 2)
				{
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.T;

					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.T;

					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.T;
				}
				if (_objLoader->getTexCordArrayCount() == 3)
				{
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.R;

					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.R;

					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.R;
				}
				if (_objLoader->getTexCordArrayCount() == 4)
				{
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.R;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.Q;

					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.SecondPointTextureCoord.R;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.Q;

					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.S;
					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.T;
					_pftexturesCoords[texcord_cnt++] = triangle.ThirdPointTextureCoord.R;
					_pftexturesCoords[texcord_cnt++] = triangle.FirstPointTextureCoord.Q;
				}
			}
			/**/



			/*filling normals*/
			if (_pfnormals != NULL && _objLoader->getNormals()->size() > 0)
			{
				_pfnormals[normals_cnt++] = triangle.FirstPointNormals.N1;
				_pfnormals[normals_cnt++] = triangle.FirstPointNormals.N2;
				_pfnormals[normals_cnt++] = triangle.FirstPointNormals.N3;

				_pfnormals[normals_cnt++] = triangle.SecondPointNormals.N1;
				_pfnormals[normals_cnt++] = triangle.SecondPointNormals.N2;
				_pfnormals[normals_cnt++] = triangle.SecondPointNormals.N3;

				_pfnormals[normals_cnt++] = triangle.ThirdPointNormals.N1;
				_pfnormals[normals_cnt++] = triangle.ThirdPointNormals.N2;
				_pfnormals[normals_cnt++] = triangle.ThirdPointNormals.N3;
			}
			/**/

		}
	

		if (!useHpp)
			reduceMeshOnCPU();

}

void ThreeDModelLoader::ModelParser::getTriangle(int position, OGLTRIANGLE * triangle)
{

	//point 1
	if (_objLoader->getVertices()->size() > 0)
	{
		triangle->FirstPointVertices.X1 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1 ).X1;
		triangle->FirstPointVertices.X2 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1 ).X2;
		triangle->FirstPointVertices.X3 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1 ).X3;
	}
	if (_objLoader->getTextureCoordinates()->size() > 0)
	{
		triangle->FirstPointTextureCoord.S = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T1 ).S;
		triangle->FirstPointTextureCoord.T = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T1 ).T;
		triangle->FirstPointTextureCoord.R = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T1 ).R;
		triangle->FirstPointTextureCoord.Q = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T1 ).Q;
	}
	if (_objLoader->getNormals()->size() > 0)
	{
		triangle->FirstPointNormals.N1 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N1 ).N1;
		triangle->FirstPointNormals.N2 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N1 ).N2;
		triangle->FirstPointNormals.N3 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N1 ).N3;
	}
	//


	//point 2
	if (_objLoader->getVertices()->size()> 0)
	{
		triangle->SecondPointVertices.X1 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V2 ).X1;
		triangle->SecondPointVertices.X2 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V2 ).X2;
		triangle->SecondPointVertices.X3 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V2 ).X3;
	}
	if (_objLoader->getTextureCoordinates()->size() > 0)
	{
		triangle->SecondPointTextureCoord.S = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T2 ).S;
		triangle->SecondPointTextureCoord.T = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T2 ).T;
		triangle->SecondPointTextureCoord.R = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T2 ).R;
		triangle->SecondPointTextureCoord.Q = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T2 ).Q;
	}
	if (_objLoader->getNormals()->size() > 0)
	{
		triangle->SecondPointNormals.N1 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N2 ).N1;
		triangle->SecondPointNormals.N2 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N2 ).N2;
		triangle->SecondPointNormals.N3 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N2 ).N3;
	}
	//



	//point 3
	if (_objLoader->getVertices()->size() > 0)
	{
		triangle->ThirdPointVertices.X1 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V3 ).X1;
		triangle->ThirdPointVertices.X2 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V3 ).X2;
		triangle->ThirdPointVertices.X3 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V3 ).X3;
	}
	if (_objLoader->getTextureCoordinates()->size() > 0)
	{
		triangle->ThirdPointTextureCoord.S = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T3 ).S;
		triangle->ThirdPointTextureCoord.T = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T3 ).T;
		triangle->ThirdPointTextureCoord.R = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T3 ).R;
		triangle->ThirdPointTextureCoord.Q = _objLoader->getTextureCoordinates()->at(_objLoader->getFaces()->at(position).T3 ).Q;
	}
	if (_objLoader->getNormals()->size() > 0)
	{
		triangle->ThirdPointNormals.N1 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N3 ).N1;
		triangle->ThirdPointNormals.N2 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N3 ).N2;
		triangle->ThirdPointNormals.N3 = _objLoader->getNormals()->at(_objLoader->getFaces()->at(position).N3 ).N3;
	}
	//
}

void ThreeDModelLoader::ModelParser::reduceMeshOnCPU()
{
	const float TOKEN = 123;

	FILE * fp;
	fopen_s(&fp,"logtext.txt","w");

	int lenghtoffaces = _objLoader->getFaces()->size();
	//_verticesUnique and fill with IDENTIFIER , position ,IDENTIFIER,
	int lengthofvertices = _objLoader->getVertices()->size();
	int lengthofnormals = _objLoader->getNormals()->size();
	int lengthoftextures = _objLoader->getTextureCoordinates()->size();

	int step1 = (lengthofvertices > lengthofnormals) ? lengthofvertices : lengthofnormals;
	int max = (step1 > lengthoftextures) ? step1 : lengthoftextures;

	GLfloat *pfVertices = (GLfloat*)malloc(sizeof(GLfloat)*lengthofvertices * 3);
	int vertices_cnt = 0;

	GLfloat *pfNormals = (GLfloat*)malloc(sizeof(GLfloat)*lengthofnormals * 3);
	int  normals_cnt = 0;

	GLfloat *pfTextures = (GLfloat*)malloc(sizeof(GLfloat)*lengthoftextures * 3);
	int textures_cnt = 0;


	pnVerticesIndex = (unsigned int*)malloc(sizeof(unsigned int)*lenghtoffaces * 3);
	vertices_index_cnt=0;

	GLint *pntexturesIndex = (GLint*)malloc(sizeof(GLint)*lenghtoffaces * 3);
	int textures_index_cnt=0;

	GLint *pnNormalsIndex = (GLint*)malloc(sizeof(GLint)*lenghtoffaces * 3);
	int normals_index_cnt=0;



	//start of copying.
	for (int i = 0; i < (int)max; i++)
	{
			//copy vertices
			if (i < lengthofvertices)
			{
				pfVertices[vertices_cnt++] = _objLoader->getVertices()->at(i).X1;
				pfVertices[vertices_cnt++] = _objLoader->getVertices()->at(i).X2;
				pfVertices[vertices_cnt++] = _objLoader->getVertices()->at(i).X3;
			}
			//

			//copy textures cordinates
			if (i < lengthoftextures)
			{
				pfTextures[textures_cnt++] = _objLoader->getTextureCoordinates()->at(i).S;
				pfTextures[textures_cnt++] = _objLoader->getTextureCoordinates()->at(i).T;
				pfTextures[textures_cnt++] = _objLoader->getTextureCoordinates()->at(i).R;
			}
			//


			//copy normals
			if (i < lengthofnormals)
			{
				pfNormals[normals_cnt++] = _objLoader->getNormals()->at(i).N1;
				pfNormals[normals_cnt++] = _objLoader->getNormals()->at(i).N2;
				pfNormals[normals_cnt++] = _objLoader->getNormals()->at(i).N3;
			}
			//

			//copy faces
			if (i < lenghtoffaces)
			{
				pnVerticesIndex[vertices_index_cnt++] = _objLoader->getFaces()->at(i).V1;
				pnVerticesIndex[vertices_index_cnt++] = _objLoader->getFaces()->at(i).V2;
				pnVerticesIndex[vertices_index_cnt++] = _objLoader->getFaces()->at(i).V3;

				pntexturesIndex[textures_index_cnt++] = _objLoader->getFaces()->at(i).T1;
				pntexturesIndex[textures_index_cnt++] = _objLoader->getFaces()->at(i).T2;
				pntexturesIndex[textures_index_cnt++] = _objLoader->getFaces()->at(i).T3;
				
				pnNormalsIndex[normals_index_cnt++] = _objLoader->getFaces()->at(i).N1;
				pnNormalsIndex[normals_index_cnt++] = _objLoader->getFaces()->at(i).N2;
				pnNormalsIndex[normals_index_cnt++] = _objLoader->getFaces()->at(i).N3;
			}
			//

	}
	//end of copying

	
	

	step1 = (vertices_cnt > textures_cnt) ? vertices_cnt : textures_cnt;
	max = (step1 > normals_cnt) ? step1 : normals_cnt;

	int *verticesDuplcation = (int*)malloc(sizeof(int)*lengthofvertices*2);  //[i,j] // swap j with i logic
	int vertices_duplcation_cnt = 0;

	int *texturesDuplcation = (int*)malloc(sizeof(int)*lengthoftextures * 2);
	int textures_duplcation_cnt = 0;

	int *normalsDuplcation = (int*)malloc(sizeof(int)*lengthofnormals * 2);
	int normals_duplcation_cnt = 0;
	


	pfUniqueVertices = (GLfloat*)malloc(sizeof(GLfloat)*lenghtoffaces * 3);
	unique_vertices_index_cnt = 0;

	GLfloat *pfUniquetextures = (GLfloat*)malloc(sizeof(GLfloat)*lenghtoffaces * 3);
	int unique_textures_index_cnt = 0;

	GLfloat *pfUniqueNormals= (GLfloat*)malloc(sizeof(GLfloat)*lenghtoffaces * 3);
	int unique_normals_index_cnt = 0;

	int isUniqueVerticesElement = 1;
	int isUniqueTexturesElement = 1;
	int isUniqueNormalsElement = 1;


	int vertices_duplcation_table_index = 0;
	//reduce mesh vertices , textures, normals.
	for (int stride_i=0; stride_i < (int)max/3;  stride_i++)
	{
		

		int i_1 = (stride_i * 3) + 0;
		int i_2 = (stride_i * 3) + 1;
		int i_3 = (stride_i * 3) + 2;

		for (int stride_j= stride_i+1; stride_j < max/3; stride_j++)
		{
			int j_1 =  (stride_j * 3) + 0;
			int j_2 =  (stride_j * 3) + 1;
			int j_3 = (stride_j * 3) + 2;

			//compare vertices for duplication
			if (stride_j < vertices_cnt/3)
			{
				
				if (pfVertices[i_1] == TOKEN && pfVertices[i_2] == TOKEN && pfVertices[i_3] == TOKEN)
				{
					isUniqueVerticesElement = 0;
				}

				if (isUniqueVerticesElement != 0 && pfVertices[i_1] == pfVertices[j_1] && pfVertices[i_2] == pfVertices[j_2] && pfVertices[i_3] == pfVertices[j_3])
				{
					verticesDuplcation[vertices_duplcation_cnt++] = stride_i;
					verticesDuplcation[vertices_duplcation_cnt++] = stride_j;

					pfVertices[j_1] = TOKEN;
					pfVertices[j_2] = TOKEN;
					pfVertices[j_3] = TOKEN;

					isUniqueVerticesElement = 0;
				}

				
			}
			else
			{
				isUniqueVerticesElement = 0;
			}
			//end


			//compare textures cordinated for duplication
			if (stride_j <= textures_cnt / 3)
			{
				isUniqueTexturesElement = 1;

				if (pfTextures[i_1] == TOKEN && pfTextures[i_2] == TOKEN && pfTextures[i_3] == TOKEN)
				{
					isUniqueTexturesElement = 0;
				}

				if (isUniqueTexturesElement != 0 && pfTextures[i_1] == pfTextures[j_1] && pfTextures[i_2] == pfTextures[j_2] && pfTextures[i_3] == pfTextures[j_3])
				{
					texturesDuplcation[textures_duplcation_cnt++] = stride_i;
					texturesDuplcation[textures_duplcation_cnt++] = stride_j;

					pfTextures[j_1] = TOKEN;
					pfTextures[j_2] = TOKEN;
					pfTextures[j_3] = TOKEN;

					isUniqueTexturesElement = 0;
				}


			}
			else
			{
				isUniqueTexturesElement = 0;
			}
			//end


			//compare normals cordinated for duplication
			if (stride_j <= normals_cnt / 3)
			{
				isUniqueNormalsElement = 1;

				if (pfNormals[i_1] == TOKEN && pfNormals[i_2] == TOKEN && pfNormals[i_3] == TOKEN)
				{
					isUniqueNormalsElement = 0;
				}

				if (isUniqueNormalsElement != 0 && pfNormals[i_1] == pfNormals[j_1] && pfNormals[i_2] == pfNormals[j_2] && pfNormals[i_3] == pfNormals[j_3])
				{
					normalsDuplcation[normals_duplcation_cnt++] = stride_i;
					normalsDuplcation[normals_duplcation_cnt++] = stride_j;

					pfNormals[j_1] = TOKEN;
					pfNormals[j_2] = TOKEN;
					pfNormals[j_3] = TOKEN;

					isUniqueNormalsElement = 0;
				}


			}
			else
			{
				isUniqueNormalsElement = 0;
			}
			//end
		}
		

		//copy unique vertices in new array
		if (isUniqueVerticesElement == 1)
		{

			pfUniqueVertices[unique_vertices_index_cnt++] = pfVertices[i_1];
			pfUniqueVertices[unique_vertices_index_cnt++] = pfVertices[i_2];
			pfUniqueVertices[unique_vertices_index_cnt++] = pfVertices[i_3];

			isUniqueVerticesElement = 0;

		}
		//end


		//copy unique textures in new array
		if (isUniqueTexturesElement == 1)
		{
			pfUniquetextures[unique_textures_index_cnt++] = pfTextures[i_1];
			pfUniquetextures[unique_textures_index_cnt++] = pfTextures[i_2];
			pfUniquetextures[unique_textures_index_cnt++] = pfTextures[i_3];

			isUniqueTexturesElement = 0;

		}
		//end


		//copy unique normals in new array
		if (isUniqueNormalsElement == 1)
		{
			pfUniqueNormals[unique_normals_index_cnt++] = pfNormals[i_1];
			pfUniqueNormals[unique_normals_index_cnt++] = pfNormals[i_2];
			pfUniqueNormals[unique_normals_index_cnt++] = pfNormals[i_3];

			isUniqueNormalsElement = 0;

		}
		//end

	}
	//end



	for (int i = 0; i < vertices_index_cnt; i = i + 3)
	{
		fprintf(fp, "\nbefore:%d %d %d", pnVerticesIndex[i], pnVerticesIndex[i + 1], pnVerticesIndex[i + 2]);
	}
	//update the face table indices now
	step1 = (vertices_duplcation_cnt > textures_duplcation_cnt) ? vertices_duplcation_cnt : textures_duplcation_cnt;
	max = (step1 > normals_duplcation_cnt) ? step1 : normals_duplcation_cnt;

	int step2 = (int)(vertices_index_cnt > textures_index_cnt) ? vertices_index_cnt : textures_index_cnt;
	int max2 = (int)(step2 > normals_index_cnt) ? step2 : normals_index_cnt;

	int  max_vertices_stride = (int)vertices_duplcation_cnt / 3;
	int  max_textures_stride = (int)textures_duplcation_cnt / 3;
	int  max_normals_stride = (int)normals_duplcation_cnt / 3;
	
	
	for (int stride_i = 0; stride_i <= (int)max/2; stride_i++)
	{
		int i_1 = (stride_i * 2) + 0;
		int i_2 = (stride_i * 2) + 1;

		for (int i = 0; i < max2; i++)
		{
			//update vertices indices
			if (stride_i <= max_vertices_stride && i <= vertices_index_cnt)
			{
				if (pnVerticesIndex[i] == verticesDuplcation[i_2])
				{
					pnVerticesIndex[i] = verticesDuplcation[i_1];
				}
			}
			//end

			//update textures indices
			if (stride_i< max_textures_stride && i < textures_index_cnt)
			{
				if (pntexturesIndex[i] == texturesDuplcation[i_2])
				{
					pntexturesIndex[i] = texturesDuplcation[i_1];
				}
			}
			//end

			//update normals indices
			if (stride_i< max_normals_stride && i < normals_index_cnt)
			{
				if (pnNormalsIndex[i] == normalsDuplcation[i_2])
				{
					pnNormalsIndex[i] = normalsDuplcation[i_1];
				}
			}
			//end

		}
	}
	//end

	
	for (size_t i = 0; i < vertices_duplcation_cnt; i = i + 2)
	{
		fprintf(fp, "\ndup:%d %d", verticesDuplcation[i], verticesDuplcation[i + 1]);
	}
	for (size_t i = 0; i < vertices_index_cnt; i = i + 3)
	{
		fprintf(fp, "\nafter:%d %d %d", pnVerticesIndex[i], pnVerticesIndex[i + 1], pnVerticesIndex[i+2]);
	}
	int a = 0;
	//testing code
	pnVerticesIndex[0] = 0;
	pnVerticesIndex[1] = 1;
	pnVerticesIndex[2] = 2;
	pnVerticesIndex[3] = 0;
	pnVerticesIndex[4] = 3;
	pnVerticesIndex[5] = 4;
	pnVerticesIndex[6] = 0;
	pnVerticesIndex[7] = 2;
	pnVerticesIndex[8] = 3;
	pnVerticesIndex[9] = 0;
	pnVerticesIndex[10] = 4;
	pnVerticesIndex[11] = 1;
	//
	fclose(fp);
}


GLfloat * ThreeDModelLoader::ModelParser::getVerticesArrayForDrawUsingArrays()
{
	return (_pfvertices!=NULL)? _pfvertices:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfVerticesArrayForDrawUsingArrays()
{
	return _objLoader != NULL ? (_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat):0;
}

GLfloat * ThreeDModelLoader::ModelParser::getTexturesCoords()
{
	return (_pftexturesCoords!=NULL)? _pftexturesCoords:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfTexturesCoords()
{
	int size = 0;

	if (_objLoader->getTexCordArrayCount() == 2)
		size =(_objLoader->getFaces()->size()) * 3 * 2 * sizeof(GLfloat);

	if (_objLoader->getTexCordArrayCount() == 3)
		size = (_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat);

	if (_objLoader->getTexCordArrayCount() == 4)
		size = (_objLoader->getFaces()->size()) * 3 * 4 * sizeof(GLfloat);

	return size;
}

int ThreeDModelLoader::ModelParser::getTexCordArrayCount()
{
	return _objLoader!=NULL?_objLoader->getTexCordArrayCount():0;
}


GLfloat * ThreeDModelLoader::ModelParser::getVerticesArrayForDrawUsingElements()
{
	return  (pfUniqueVertices!=NULL)? pfUniqueVertices:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfVerticesArrayForDrawUsingElements()
{
	return unique_vertices_index_cnt * sizeof(GLfloat);
}


unsigned int * ThreeDModelLoader::ModelParser::getIndicesArray()
{
	return  (pnVerticesIndex != NULL) ? pnVerticesIndex : NULL;
}


int ThreeDModelLoader::ModelParser::getSizeOfIndicesArray()
{
	return  vertices_index_cnt * sizeof(unsigned int);
}


int ThreeDModelLoader::ModelParser::getIndicesArrayCount()
{
	return vertices_index_cnt;
}

GLfloat * ThreeDModelLoader::ModelParser::getNormalArray()
{
	return (_pfnormals!=NULL)? _pfnormals:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfNormalArray()
{
	return  _objLoader!=NULL?(_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat):0;
}


ThreeDModelLoader::ModelParser::~ModelParser()
{

	if (_objLoader != NULL)
	{
		delete _objLoader;
		_objLoader = NULL;
	}

	if (_pfvertices != NULL)
	{
		free(_pfvertices);
		_objLoader = NULL;
	}

	if (_pftexturesCoords != NULL)
	{
		free(_pftexturesCoords);
		_pftexturesCoords = NULL;
	}

	if (_pfnormals != NULL)
	{
		free(_pfnormals);
		_pfnormals = NULL;
	}
	
	if (pfUniqueVertices != NULL)
	{
		free(pfUniqueVertices);
		pfUniqueVertices = NULL;
	}

	if (pnVerticesIndex != NULL)
	{
		free(pnVerticesIndex);
		pnVerticesIndex = NULL;
	}
}

