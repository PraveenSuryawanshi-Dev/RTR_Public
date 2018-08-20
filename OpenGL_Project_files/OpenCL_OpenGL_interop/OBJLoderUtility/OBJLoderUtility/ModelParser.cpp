/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
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
	texcord_cnt = 0;
	_objLoader = NULL;
	_pfUniqueVertices = NULL;
	_nUniqueVerticesIndexCnt = 0;
	_pnVerticesIndex = NULL;
	_nVerticesIndexCnt = 0;
	_pfUniqueNormals = NULL;
	_nUniqueNormalsIndexCnt = 0;
	_pnNormalsIndex = NULL;
	_nNormalsIndexCnt = 0;
	_pfUniqueTextures = NULL;
	_nUniqueTexturesIndexCnt = 0;
	_pnTexturesIndex = NULL;
	_nTexturesIndexCnt = 0;


	_objLoader = new OBJLoader(filenfullpathname);
	CHECK_ON_NULL_THROW_EXCEPTION(_objLoader)

	_objLoader->parse();

	if (_objLoader->getVertices()->size() > 0)
	{
		_pfvertices = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3); // 9 because of  3 vertices with 3 points x,y,z i.e 3*3
		CHECK_ON_NULL_THROW_EXCEPTION(_pfvertices)
	}

	if (_objLoader->getTextureCoordinates()->size()>0)
	{
		if (_objLoader->getTexCordArrayCount() == 2)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3/*vertices*/ * 2 /*cords*/);

		if (_objLoader->getTexCordArrayCount() == 3)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3);

		if (_objLoader->getTexCordArrayCount() == 4)
			_pftexturesCoords = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 4);

		CHECK_ON_NULL_THROW_EXCEPTION(_pftexturesCoords)
	}

	
	if (_objLoader->getNormals()->size() > 0)
	{
		_pfnormals = (GLfloat*)malloc(sizeof(GLfloat)*(_objLoader->getFaces()->size()) * 3 * 3);
		CHECK_ON_NULL_THROW_EXCEPTION(_pfnormals)
	}

	_gl_object = new GLObject();
	CHECK_ON_NULL_THROW_EXCEPTION(_gl_object)

}

void ThreeDModelLoader::ModelParser::parse(PROCESS_TYPE processtype, DRAW_TYPE drawtype)
{

	_process_type = processtype;
	_draw_type = drawtype;

	int vertices_cnt = 0;
	int normals_cnt = 0;
	int i = 0;
	
		for ( i = 0; i < (_objLoader->getFaces()->size()); i++)
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

		}


		if (_draw_type == DRAW_TYPE::DRAW_USING_ARRAYS)
		{
			_gl_object->initialize_for_draw_array_from_cpu(vertices_cnt, _pfvertices, texcord_cnt, _pftexturesCoords, normals_cnt, _pfnormals);
		}
		else
		{ //draw using draw elements
			reduceMesh();
		}
			

}

void ThreeDModelLoader::ModelParser::draw()
{
	if (_draw_type == DRAW_TYPE::DRAW_USING_ARRAYS)
	{
		_gl_object->draw_using_draw_array((int)(_objLoader->getFaces()->size()) * 3 * 3);
	}
	else
	{
		_gl_object->draw_using_draw_elements(_nVerticesIndexCnt);
	}
}

void ThreeDModelLoader::ModelParser::getTriangle(int position, OGLTRIANGLE * triangle)
{
	
	//point 1
	if (_objLoader->getVertices()->size() > 0)
	{
		

		triangle->FirstPointVertices.X1 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1).X1;
		triangle->FirstPointVertices.X2 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1).X2;
		triangle->FirstPointVertices.X3 = _objLoader->getVertices()->at(_objLoader->getFaces()->at(position).V1).X3;
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

void ThreeDModelLoader::ModelParser::reduceMesh()
{
	const float TOKEN = 123;


	int lenghtoffaces = (int)_objLoader->getFaces()->size();
	
	//_verticesUnique and fill with IDENTIFIER , position ,IDENTIFIER,
	int lengthofvertices =(int) _objLoader->getVertices()->size();
	int lengthoftextures = (int)_objLoader->getTextureCoordinates()->size();
	int lengthofnormals = (int)_objLoader->getNormals()->size();

	int step1 = (lengthofvertices > lengthoftextures) ? lengthofvertices : lengthoftextures;
	int max = (step1 > lengthofnormals) ? step1 : lengthofnormals;
	max = max * 3; // multiply with 3 because of three x,y,z

	GLfloat *pfVertices = (GLfloat*)malloc(sizeof(GLfloat)*lengthofvertices * 3); 
	CHECK_ON_NULL_THROW_EXCEPTION(pfVertices)
	int vertices_cnt = 0;
	
	GLfloat *pfTex_cords = (GLfloat*)malloc(sizeof(GLfloat)*lengthoftextures * 2);
	CHECK_ON_NULL_THROW_EXCEPTION(pfTex_cords)
	int tex_coords_cnt = 0;

	GLfloat *pfNormals = (GLfloat*)malloc(sizeof(GLfloat)*lengthofnormals * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(pfNormals)
	int  normals_cnt = 0;



	_pnVerticesIndex = (unsigned int*)malloc(sizeof(unsigned int)*lenghtoffaces * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(_pnVerticesIndex)
	_nVerticesIndexCnt=0;
	
	_pnTexturesIndex = (unsigned int*)malloc(sizeof(unsigned int)*lenghtoffaces * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(_pnTexturesIndex)
	_nTexturesIndexCnt = 0;


	_pnNormalsIndex = (unsigned int*)malloc(sizeof(unsigned int)*lenghtoffaces * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(_pnNormalsIndex)
	_nNormalsIndexCnt=0;	


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

			//copy texture coords
			if (i < lengthoftextures)
			{
				pfTex_cords[tex_coords_cnt++] = _objLoader->getTextureCoordinates()->at(i).S;
				pfTex_cords[tex_coords_cnt++] = _objLoader->getTextureCoordinates()->at(i).T;

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
				_pnVerticesIndex[_nVerticesIndexCnt++] = _objLoader->getFaces()->at(i).V1;
				_pnVerticesIndex[_nVerticesIndexCnt++] = _objLoader->getFaces()->at(i).V2;
				_pnVerticesIndex[_nVerticesIndexCnt++] = _objLoader->getFaces()->at(i).V3;

				_pnTexturesIndex[_nTexturesIndexCnt++] = _objLoader->getFaces()->at(i).T1;
				_pnTexturesIndex[_nTexturesIndexCnt++] = _objLoader->getFaces()->at(i).T2;
				_pnTexturesIndex[_nTexturesIndexCnt++] = _objLoader->getFaces()->at(i).T3;
				
				_pnNormalsIndex[_nNormalsIndexCnt++] = _objLoader->getFaces()->at(i).N1;
				_pnNormalsIndex[_nNormalsIndexCnt++] = _objLoader->getFaces()->at(i).N2;
				_pnNormalsIndex[_nNormalsIndexCnt++] = _objLoader->getFaces()->at(i).N3;
			}
			//

	}
	//end of copying

	

	max = (vertices_cnt > normals_cnt) ? vertices_cnt : normals_cnt;

	int *verticesDuplcation = (int*)malloc(sizeof(int)*lengthofvertices*2); 
	CHECK_ON_NULL_THROW_EXCEPTION(verticesDuplcation)
	int vertices_duplcation_cnt = 0;


	int *normalsDuplcation = (int*)malloc(sizeof(int)*lengthofnormals * 2);
	CHECK_ON_NULL_THROW_EXCEPTION(normalsDuplcation)
	int normals_duplcation_cnt = 0;
	
	

	_pfUniqueVertices = (GLfloat*)malloc(sizeof(GLfloat)*lenghtoffaces * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(_pfUniqueVertices)
	_nUniqueVerticesIndexCnt = 0;


	_pfUniqueNormals= (GLfloat*)malloc(sizeof(GLfloat)*lenghtoffaces * 3);
	CHECK_ON_NULL_THROW_EXCEPTION(_pfUniqueNormals)
	_nUniqueNormalsIndexCnt = 0;

	int isUniqueVerticesElement = 1;
	int isUniqueNormalsElement = 1;


	if (_process_type != PROCESS_TYPE::GPU)
	{
		// reduce mesh on CPU:
		//reduce mesh vertices , normals.
		for (int stride_i = 0; stride_i < (int)max / 3; stride_i++)
		{

			int i_1 = (stride_i * 3) + 0;
			int i_2 = (stride_i * 3) + 1;
			int i_3 = (stride_i * 3) + 2;

			for (int stride_j = stride_i + 1; stride_j < max / 3; stride_j++)
			{

				int j_1 = (stride_j * 3) + 0;
				int j_2 = (stride_j * 3) + 1;
				int j_3 = (stride_j * 3) + 2;

				//compare vertices for duplication
				if (stride_j < vertices_cnt / 3)
				{
					isUniqueVerticesElement = 1;

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

						isUniqueVerticesElement = 1;

						if (stride_j != _nUniqueVerticesIndexCnt)
						{
							for (size_t i = 0; i < _nVerticesIndexCnt; i++)
							{
								if (_pnVerticesIndex[i] == stride_j)
								{
									_pnVerticesIndex[i] = _nUniqueVerticesIndexCnt / 3;
								}
							}

						}
					}
				}
				//end



				//compare normals for duplication
				if (stride_j < normals_cnt / 3)
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

						isUniqueNormalsElement = 1;

						if (stride_j != _nUniqueNormalsIndexCnt)
						{
							for (size_t i = 0; i < _nNormalsIndexCnt; i++)
							{
								if (_pnNormalsIndex[i] == stride_j)
								{
									_pnNormalsIndex[i] = _nUniqueNormalsIndexCnt / 3;
								}
							}

						}
					}
				}
				//end
			}


			//copy unique vertices in new array
			if (isUniqueVerticesElement == 1)
			{
				//update the vertices index in face table
				if (stride_i != _nUniqueVerticesIndexCnt)
				{
					for (size_t i = 0; i < _nVerticesIndexCnt; i++)
					{
						if (_pnVerticesIndex[i] == stride_i)
						{
							_pnVerticesIndex[i] = _nUniqueVerticesIndexCnt / 3;
						}
					}

				}//end

				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_1];
				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_2];
				_pfUniqueVertices[_nUniqueVerticesIndexCnt++] = pfVertices[i_3];

				isUniqueVerticesElement = 0;

			}
			//end


			//copy unique normals in new array
			if (isUniqueNormalsElement == 1)
			{
				//update the normals index in face table
				if (stride_i != _nUniqueNormalsIndexCnt)
				{
					for (size_t i = 0; i < _nNormalsIndexCnt; i++)
					{
						if (_pnNormalsIndex[i] == stride_i)
						{
							_pnNormalsIndex[i] = _nUniqueNormalsIndexCnt / 3;
						}
					}

				}//end

				_pfUniqueNormals[_nUniqueNormalsIndexCnt++] = pfNormals[i_1];
				_pfUniqueNormals[_nUniqueNormalsIndexCnt++] = pfNormals[i_2];
				_pfUniqueNormals[_nUniqueNormalsIndexCnt++] = pfNormals[i_3];

				isUniqueNormalsElement = 0;

			}
			//end of for 2

		}
		//end of for 1
		
		_gl_object->initialize_for_draw_elements_from_cpu(_nUniqueVerticesIndexCnt, _pfUniqueVertices, tex_coords_cnt, pfTex_cords, _nUniqueNormalsIndexCnt, _pfUniqueNormals, _nVerticesIndexCnt, _pnVerticesIndex);
	}
	else
	{
		//reduce mesh on GPU;
		_ocl = new CLGL();
		if (_ocl == NULL)
			throw "EXCEPTION : OBJECT NULL EXCEPTION";
		
		_ocl->initialize_CL();
		_gl_object->initialize_for_draw_elements_from_gpu(_nUniqueVerticesIndexCnt, _pfUniqueVertices, tex_coords_cnt, pfTex_cords, _nUniqueNormalsIndexCnt, _pfUniqueNormals, _nVerticesIndexCnt, _pnVerticesIndex);
		
		_ocl->test(
			pfVertices,
			vertices_cnt,
			_gl_object->get_vbo_object_positions()
		
		);
		/*_ocl->reduce_vertices
		(
			pfVertices,
			 vertices_cnt,
			_pnVerticesIndex,
			_nVerticesIndexCnt,
			verticesDuplcation,
			_pfUniqueVertices,
			lengthofvertices,
			lenghtoffaces
		);*/

	}

	//dispose local malloc.
	SAFE_FREE(pfVertices);
	SAFE_FREE(pfTex_cords);
	SAFE_FREE(pfNormals);
	SAFE_FREE(verticesDuplcation);
	SAFE_FREE(normalsDuplcation);
}


/*GLfloat * ThreeDModelLoader::ModelParser::getVerticesArrayForDrawUsingArrays()
{
	return (_pfvertices!=NULL)? _pfvertices:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfVerticesArrayForDrawUsingArrays()
{
	return _objLoader != NULL ? (int)(_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat):0;
}

GLfloat * ThreeDModelLoader::ModelParser::getTexturesCoords()
{
	return (_pftexturesCoords!=NULL)? _pftexturesCoords:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfTexturesCoords()
{
	int size = 0;

	if (_objLoader->getTexCordArrayCount() == 2)
		size =(int)(_objLoader->getFaces()->size()) * 3 * 2 * sizeof(GLfloat);

	if (_objLoader->getTexCordArrayCount() == 3)
		size =(int) (_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat);

	if (_objLoader->getTexCordArrayCount() == 4)
		size =(int) (_objLoader->getFaces()->size()) * 3 * 4 * sizeof(GLfloat);

	return size;
}

int ThreeDModelLoader::ModelParser::getTexCordArrayCount()
{
	return _objLoader!=NULL?(int)_objLoader->getTexCordArrayCount():0;
}


GLfloat * ThreeDModelLoader::ModelParser::getVerticesArrayForDrawUsingElements()
{
	return  (_pfUniqueVertices!=NULL)? _pfUniqueVertices:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfVerticesArrayForDrawUsingElements()
{
	return _nUniqueVerticesIndexCnt * sizeof(GLfloat);
}


unsigned int * ThreeDModelLoader::ModelParser::getIndicesArray()
{
	return  (_pnVerticesIndex != NULL) ? _pnVerticesIndex : NULL;
}


int ThreeDModelLoader::ModelParser::getSizeOfIndicesArray()
{
	return  _nVerticesIndexCnt * sizeof(int);
}


int ThreeDModelLoader::ModelParser::getIndicesArrayCount()
{
	return _nVerticesIndexCnt;
}

GLfloat * ThreeDModelLoader::ModelParser::getTexturesArrayForDrawUsingElements()
{
	return (_pftexturesCoords != NULL) ? _pftexturesCoords : NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfTexturesArrayForDrawUsingElements()
{
	return texcord_cnt * sizeof(GLfloat);
}

unsigned int * ThreeDModelLoader::ModelParser::getTexturesIndicesArray()
{
	return (_pnTexturesIndex != NULL) ? _pnTexturesIndex : NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfTexturesIndicesArray()
{
	return _nTexturesIndexCnt * sizeof(int);
}

GLfloat * ThreeDModelLoader::ModelParser::getNormalsArrayForDrawUsingElements()
{
	return (_pfUniqueNormals != NULL) ? _pfUniqueNormals : NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfNormalsArrayForDrawUsingElements()
{
	return _nUniqueNormalsIndexCnt * sizeof(GLfloat);
}

unsigned int * ThreeDModelLoader::ModelParser::getNormalsIndicesArray()
{
	return (_pnNormalsIndex != NULL) ? _pnNormalsIndex : NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfNormalsIndicesArray()
{
	return _nNormalsIndexCnt * sizeof(int);
}

GLfloat * ThreeDModelLoader::ModelParser::getNormalArray()
{
	return (_pfnormals!=NULL)? _pfnormals:NULL;
}

int ThreeDModelLoader::ModelParser::getSizeOfNormalArray()
{
	return  _objLoader!=NULL?((int)_objLoader->getFaces()->size()) * 3 * 3 * sizeof(GLfloat):0;
}*/


ThreeDModelLoader::ModelParser::~ModelParser()
{

	SAFE_DELETE(_objLoader)
	
	SAFE_FREE(_pfvertices)
	SAFE_FREE(_pftexturesCoords)
	SAFE_FREE(_pfnormals)
	SAFE_FREE(_pfUniqueVertices)
	SAFE_FREE(_pnVerticesIndex)
	SAFE_FREE(_pfUniqueNormals)
	SAFE_FREE(_pnNormalsIndex)
	SAFE_FREE(_pfUniqueTextures)
	SAFE_FREE(_pnTexturesIndex)
	
	SAFE_DELETE(_ocl)
	SAFE_DELETE(_gl_object)
}

