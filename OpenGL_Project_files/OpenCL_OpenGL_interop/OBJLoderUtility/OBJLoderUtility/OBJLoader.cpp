/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/


#include "OBJLoader.h"
#include<stdio.h>
#include<amp.h>



using namespace concurrency;



ThreeDModelLoader::OBJLoader::OBJLoader(char * szfileFullPath)
{
	_nTexCordCount = 0;
	_pvVertices = NULL;

	_pvTexturesCoords = NULL;
	_pvNormals = NULL;
	_pvFaces= NULL;
	_model = NULL;


	CHECK_ON_NULL_THROW_EXCEPTION(szfileFullPath)

	_szfileFullPath = szfileFullPath;
	

	_pvModelIndicesMapTable = new std::vector<MODEL_INDICES_MAP_TABLE*>();
	CHECK_ON_NULL_THROW_EXCEPTION(_pvModelIndicesMapTable);


	_pvVertices = new std::vector<VERTEX>();
	CHECK_ON_NULL_THROW_EXCEPTION(_pvVertices);


	_pvTexturesCoords = new std::vector<TEXTURE_CORD>();
	CHECK_ON_NULL_THROW_EXCEPTION(_pvTexturesCoords);

	_pvNormals = new std::vector<NORMAL>();
	CHECK_ON_NULL_THROW_EXCEPTION(_pvNormals);

	_pvFaces = new std::vector<TRIANGLE_FACE_INDICES>();
	CHECK_ON_NULL_THROW_EXCEPTION(_pvFaces);

	_pMtlLoader = NULL;
}

void ThreeDModelLoader::OBJLoader::parse()
{
	const int line_size = 256;

	FILE *fpMeshFile = NULL;
	char line[line_size];
	char *token = NULL;



	fopen_s(&fpMeshFile, _szfileFullPath, "r");

	CHECK_ON_NULL_THROW_EXCEPTION(fpMeshFile)

	while (true)
	{
		memset((void*)line, (int)'\0', line_size);
		token = NULL;

		if (fgets(line, line_size, fpMeshFile) == NULL)
			break;

		int number_of_tokens_in_line = getTotalTokens(line);

		token = strtok(line, separater_space);

		if (isVertex(token))
			onVertex();

		if (isTextureCoords(token))
			onTextureCoords(number_of_tokens_in_line);

		if (isNormal(token))
			onNormal();

		if (isFaces(token))
			onFaces();

		if (isMtlLib(token))
			onMtlLib();

		if (isStartUseMtl(token))
			onStartUseMtl();

		if (isEndUseMtl(token))
			onEndUseMtl();

	}

	fclose(fpMeshFile);

	onEndUseMtl(); // to finish material vector
	
	/*MAP MATERIAL TO OBJECT*/
	mapOBJToMaterials();
	/**/

}




int ThreeDModelLoader::OBJLoader::getTotalTokens(char * line)
{
	int count = 0, i;
	
	for (i = 0;line[i] != '\0';i++)
	{
		if (line[i] == ' ')
			count++;
	}

	return count;
}

bool ThreeDModelLoader::OBJLoader::isVertex(char* token)
{
	return token!=NULL?(strcmp(token, "v") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onVertex()
{
	VERTEX v;
	v.X1= (GLfloat)atof(strtok(NULL, separater_space));
	v.X2 = (GLfloat)atof(strtok(NULL, separater_space));
	v.X3 = (GLfloat)atof(strtok(NULL, separater_space));

	_pvVertices->push_back(v);
}


bool ThreeDModelLoader::OBJLoader::isTextureCoords(char* token)
{
	return token!=NULL?(strcmp(token, "vt") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onTextureCoords(int token_count)
{

	_nTexCordCount = token_count;
	 TEXTURE_CORD t;

	t.S = (GLfloat)atof(strtok(NULL, separater_space));
	t.T = (GLfloat)atof(strtok(NULL, separater_space));

	
	if(token_count > 2)
		t.R = (GLfloat)atof(strtok(NULL, separater_space));
	
	if(token_count > 3)
		t.Q = (GLfloat)atof(strtok(NULL, separater_space));
		
	_pvTexturesCoords->push_back(t);
}


int ThreeDModelLoader::OBJLoader::getTexCordArrayCount()
{
	return _nTexCordCount;
}

bool ThreeDModelLoader::OBJLoader::isNormal(char* token)
{
	return token!=NULL?(strcmp(token, "vn") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onNormal()
{

	NORMAL n;

	n.N1 = (GLfloat)atof(strtok(NULL, separater_space));
	n.N2 = (GLfloat)atof(strtok(NULL, separater_space));
	n.N3 = (GLfloat)atof(strtok(NULL, separater_space));

	_pvNormals->push_back(n);
}


bool ThreeDModelLoader::OBJLoader::isFaces(char* token)
{

	return  token!=NULL?(strcmp(token, "f") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onFaces()
{

	char *token_1 = NULL;
	char *token_2 = NULL;
	char *token_3 = NULL;
	TRIANGLE_FACE_INDICES face;

	token_1 = strtok(NULL, separater_space);
	token_2= strtok(NULL, separater_space);
	token_3 = strtok(NULL, separater_space);


	/**/
	face.V1 = (int)(atoi(strtok(token_1, separater_slash)))-1; // -1 because index starts from 0 but obj index start with 1
	
	if(_pvTexturesCoords->size()>0)
		face.T1 = (int)(atoi(strtok(NULL, separater_slash)))-1;
	
	if (_pvNormals->size()>0)
	face.N1 = (int)(atoi(strtok(NULL, separater_slash)))-1;
	/**/

	
	/**/
	face.V2 = (int)(atoi(strtok(token_2, separater_slash)))-1;
	
	if (_pvTexturesCoords->size()>0)
		face.T2 = (int)(atoi(strtok(NULL, separater_slash))-1);
	
	if (_pvNormals->size()>0)
		face.N2 = (int)(atoi(strtok(NULL, separater_slash))-1);	
	/**/



	/**/
	face.V3 = (int)(atoi(strtok(token_3, separater_slash)))-1;
	
	if (_pvTexturesCoords->size()>0)
		face.T3 = (int)(atoi(strtok(NULL, separater_slash))-1);
	
	if (_pvNormals->size()>0)
		face.N3 = (int)(atoi(strtok(NULL, separater_slash))-1);
	/**/

	_pvFaces->push_back(face);
	//_nFacesCounter++;

}

bool ThreeDModelLoader::OBJLoader::isMtlLib(char* token)
{
	return token != NULL ? (strcmp(token, "mtllib") == 0) : false;
}

void ThreeDModelLoader::OBJLoader::onMtlLib()
{
	char *filename = strtok(NULL, separater_space);

	if (filename != NULL)
	{
		char *duplicate = _strdup(_szfileFullPath);
		int len = (int)strlen(duplicate);
		duplicate[len - 3] = 'm';
		duplicate[len - 2] = 't';
		duplicate[len - 1] = 'l';

		_pMtlLoader = new MTLLoader(duplicate);
		if (_pMtlLoader == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";

		_pMtlLoader->parse();
	}
}

bool ThreeDModelLoader::OBJLoader::isStartUseMtl(char * token)
{
	return token != NULL ? (strcmp(token, "usemtl") == 0) : false;
}

void ThreeDModelLoader::OBJLoader::onStartUseMtl()
{
	

	if (_model == NULL)
	{
		_model = (MODEL_INDICES_MAP_TABLE*)malloc(sizeof(MODEL_INDICES_MAP_TABLE));
		char *filename = strtok(NULL, separater_space);
		strcpy(_model->materialName ,filename);
		_model->material_index = -1;
		_model->start_index = (int)_pvFaces->size();
		
	}
}

bool ThreeDModelLoader::OBJLoader::isEndUseMtl(char * token)
{
	return token != NULL ? (strcmp(token, "g") == 0) : false;
}

void ThreeDModelLoader::OBJLoader::onEndUseMtl()
{
	if (_model != NULL)
	{
		_model->end_index = (int)_pvFaces ->size()-1;
		_pvModelIndicesMapTable->push_back(_model);
		_model = NULL;
	}

}

void ThreeDModelLoader::OBJLoader::mapOBJToMaterials()
{
	if (_pMtlLoader == NULL /*when no material file is present*/)
		return;

	std::vector<MATERIAL*> *pvMaterial = _pMtlLoader->getMaterials();
	if (pvMaterial == NULL /*when no material file is present*/)
		return;

	if (pvMaterial->size() > 0)
	{
		for (int i = 0; i < (int)_pvModelIndicesMapTable->size(); i++)
		{
			for (int j = 0; j <(int)pvMaterial->size(); j++)
			{
				char *stringsrc = _pvModelIndicesMapTable->at(i)->materialName;
				char *stringdest = pvMaterial->at(j)->name;


				//if (strcmp(stringsrc,stringdest) == 0)
				if (equals(stringsrc, stringdest) == 0)
				{
					_pvModelIndicesMapTable->at(i)->material_index = j;
					break;
				}

			}
		}
	}
}

int ThreeDModelLoader::OBJLoader::equals(char * src, char * dest)
{
	int flag = 0;
	int i = 0;
	int len = strlen(src)-1;
	while (true)
	{
		if (len == i)
			break;

		char s = src[i];
		char ddes = dest[i];
		if (src[i] != dest[i])
		{
			flag = 1;
			break;
		}

		i++;
	}

	return flag;
}

std::vector<ThreeDModelLoader::VERTEX>* ThreeDModelLoader::OBJLoader::getVertices()
{
	return (_pvVertices !=NULL)?_pvVertices:NULL;
}

std::vector<ThreeDModelLoader::TEXTURE_CORD>* ThreeDModelLoader::OBJLoader::getTextureCoordinates()
{
	return (_pvTexturesCoords != NULL) ? _pvTexturesCoords : NULL;
}

std::vector<ThreeDModelLoader::NORMAL>* ThreeDModelLoader::OBJLoader::getNormals()
{
	return (_pvNormals != NULL) ? _pvNormals : NULL;
}

std::vector<ThreeDModelLoader::TRIANGLE_FACE_INDICES>* ThreeDModelLoader::OBJLoader::getFaces()
{
	return (_pvFaces != NULL) ? _pvFaces : NULL;
}

ThreeDModelLoader::OBJLoader::~OBJLoader()
{

	SAFE_CLEAR_VECTOR_AND_FREE(_pvVertices)
	SAFE_CLEAR_VECTOR_AND_FREE(_pvModelIndicesMapTable)
	SAFE_CLEAR_VECTOR_AND_FREE(_pvTexturesCoords)
	SAFE_CLEAR_VECTOR_AND_FREE(_pvNormals)
	SAFE_CLEAR_VECTOR_AND_FREE(_pvFaces)

	SAFE_DELETE(_pMtlLoader);
		
}
