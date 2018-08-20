/*
* @author: Praveen Suryawanshi
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/


#include "OBJLoader.h"
#include<stdio.h>
#include<amp.h>



using namespace concurrency;



ThreeDModelLoader::OBJLoader::OBJLoader(char * szfileFullPath)
{
	//_nVertexsCount = 0;
	//_nTexturesCoordsCount = 0;
	//_nNormalsCount = 0;
	//_nFacesCount = 0;
	_nTexCordCount = 0;
	//_nIndicesCount = 0;
	//_nUniqueVerticesArrayCount = 0;
	//_nFacesCounter = 0;
	_pvVertices = NULL;
	//_pvVerticesClone = NULL;
	//_nVertexsUniqueCount = 0;

	_pvTexturesCoords = NULL;
	_pvNormals = NULL;
	_pvFaces= NULL;
	_model = NULL;

	//_pfUniqueVerticeArray = NULL;

	if (szfileFullPath == NULL) throw "EXCEPTION: FILE NOT FOUND";

	_szfileFullPath = szfileFullPath;
	

	_pvModelIndicesMapTable = new std::vector<MODEL_INDICES_MAP_TABLE*>();
	if (_pvModelIndicesMapTable == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";


	_pvVertices = new std::vector<VERTEX>();
	if (_pvVertices == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";

	/*_pvVerticesClone = new std::vector<VERTEX>();
	if (_pvVerticesClone == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";
	*/
	

	_pvTexturesCoords = new std::vector<TEXTURE_CORD>();
	if (_pvTexturesCoords == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";

	_pvNormals = new std::vector<NORMAL>();
	if (_pvNormals == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";

	_pvFaces = new std::vector<TRIANGLE_FACE_INDICES>();
	if (_pvFaces == NULL) throw "EXCEPTION: OBJECT NULL EXCEPTION";

	_pMtlLoader = NULL;
}

void ThreeDModelLoader::OBJLoader::parse()
{
	const int line_size = 256;

	FILE *fpMeshFile = NULL;
	char line[line_size];
	char *token = NULL;



	fopen_s(&fpMeshFile, _szfileFullPath, "r");

	if (fpMeshFile == NULL) throw "EXCEPTION: UNABLE TO OPEN FILE";

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

		/*if (isMtlLib(token))
			onMtlLib();

		if (isStartUseMtl(token))
			onStartUseMtl();

		if (isEndUseMtl(token))
			onEndUseMtl();*/

	}

	fclose(fpMeshFile);

	onEndUseMtl(); // to finish material vector;

	/*if (useHpp)
		reduceMeshOnGPU();
	else
		reduceMeshOnCPU();
		*/
}

/*
int ThreeDModelLoader::OBJLoader::getTotalTriangles()
{
	return _pvFaces!=NULL?_pvFaces->size():0;
}
*/

/*
void ThreeDModelLoader::OBJLoader::getTriangle(int position, OGLTRIANGLE * triangle)
{

		//point 1
	if (_pvVertices->size() > 0)
	{
		triangle->FirstPointVertices.X = _pvVertices->at(_pvFaces->at(position).V1 - 1).X;
		triangle->FirstPointVertices.Y = _pvVertices->at(_pvFaces->at(position).V1 - 1).Y;
		triangle->FirstPointVertices.Z = _pvVertices->at(_pvFaces->at(position).V1 - 1).Z;
	}
	if (_nTexCordCount > 0)
	{
		triangle->FirstPointTextureCoord.S = _pvTexturesCoords->at(_pvFaces->at(position).T1 - 1).S;
		triangle->FirstPointTextureCoord.T = _pvTexturesCoords->at(_pvFaces->at(position).T1 - 1).T;
		triangle->FirstPointTextureCoord.R = _pvTexturesCoords->at(_pvFaces->at(position).T1 - 1).R;
		triangle->FirstPointTextureCoord.Q = _pvTexturesCoords->at(_pvFaces->at(position).T1 - 1).Q;
	}
	if (_pvNormals->size() > 0)
	{
		triangle->FirstPointNormals.X = _pvNormals->at(_pvFaces->at(position).N1 - 1).X;
		triangle->FirstPointNormals.Y = _pvNormals->at(_pvFaces->at(position).N1 - 1).Y;
		triangle->FirstPointNormals.Z = _pvNormals->at(_pvFaces->at(position).N1 - 1).Z;
	}
	//


	//point 2
	if (_pvVertices->size()> 0)
	{
		triangle->SecondPointVertices.X = _pvVertices->at(_pvFaces->at(position).V2 - 1).X;
		triangle->SecondPointVertices.Y = _pvVertices->at(_pvFaces->at(position).V2 - 1).Y;
		triangle->SecondPointVertices.Z = _pvVertices->at(_pvFaces->at(position).V2 - 1).Z;
	}
	if (_pvTexturesCoords->size() > 0)
	{
		triangle->SecondPointTextureCoord.S = _pvTexturesCoords->at(_pvFaces->at(position).T2 - 1).S;
		triangle->SecondPointTextureCoord.T = _pvTexturesCoords->at(_pvFaces->at(position).T2 - 1).T;
		triangle->SecondPointTextureCoord.R = _pvTexturesCoords->at(_pvFaces->at(position).T2 - 1).R;
		triangle->SecondPointTextureCoord.Q = _pvTexturesCoords->at(_pvFaces->at(position).T2 - 1).Q;
	}
	if (_pvNormals->size() > 0)
	{
		triangle->SecondPointNormals.X = _pvNormals->at(_pvFaces->at(position).N2 - 1).X;
		triangle->SecondPointNormals.Y = _pvNormals->at(_pvFaces->at(position).N2 - 1).Y;
		triangle->SecondPointNormals.Z = _pvNormals->at(_pvFaces->at(position).N2 - 1).Z;
	}
	//



	//point 3
	if (_pvVertices->size() > 0)
	{
		triangle->ThirdPointVertices.X = _pvVertices->at(_pvFaces->at(position).V3 - 1).X;
		triangle->ThirdPointVertices.Y = _pvVertices->at(_pvFaces->at(position).V3 - 1).Y;
		triangle->ThirdPointVertices.Z = _pvVertices->at(_pvFaces->at(position).V3 - 1).Z;
	}
	if (_pvTexturesCoords->size() > 0)
	{
		triangle->ThirdPointTextureCoord.S = _pvTexturesCoords->at(_pvFaces->at(position).T3 - 1).S;
		triangle->ThirdPointTextureCoord.T = _pvTexturesCoords->at(_pvFaces->at(position).T3 - 1).T;
		triangle->ThirdPointTextureCoord.R = _pvTexturesCoords->at(_pvFaces->at(position).T3 - 1).R;
		triangle->ThirdPointTextureCoord.Q = _pvTexturesCoords->at(_pvFaces->at(position).T3 - 1).Q;
	}
	if (_pvNormals->size() > 0)
	{
		triangle->ThirdPointNormals.X = _pvNormals->at(_pvFaces->at(position).N3 - 1).X;
		triangle->ThirdPointNormals.Y = _pvNormals->at(_pvFaces->at(position).N3 - 1).Y;
		triangle->ThirdPointNormals.Z = _pvNormals->at(_pvFaces->at(position).N3 - 1).Z;
	}
	//
}
*/

/*
GLfloat * ThreeDModelLoader::OBJLoader::getVerticesArrayForDrawUsingElements()
{
	return _pfUniqueVerticeArray;
}
*/


/*
int ThreeDModelLoader::OBJLoader::getSizeOfVerticesArrayForDrawUsingElements()
{
	//return (sizeof(GLfloat)*_nVertexsUniqueCount * 3);
}
*/

/*
int ThreeDModelLoader::OBJLoader::getVerticesCountForDrawUsingElements()
{
	return _nUniqueVerticesArrayCount;
}
*/

/*
unsigned int * ThreeDModelLoader::OBJLoader::getIndicesArray()
{
	return _nIndices;
}
*/

/*
int ThreeDModelLoader::OBJLoader::getIndicesArrayCount()
{
	return _nIndicesCount;
}
*/


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
	//_nVertexsCount++;

	VERTEX v;
	v.X1= (GLfloat)atof(strtok(NULL, separater_space));
	v.X2 = (GLfloat)atof(strtok(NULL, separater_space));
	v.X3 = (GLfloat)atof(strtok(NULL, separater_space));

	_pvVertices->push_back(v);
	//_pvVerticesClone->push_back(v);
}

/*
int ThreeDModelLoader::OBJLoader::getVertexArrayCount()
{
	return _pvVertices->size();
}
*/
bool ThreeDModelLoader::OBJLoader::isTextureCoords(char* token)
{
	return token!=NULL?(strcmp(token, "vt") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onTextureCoords(int token_count)
{
	//_nTexturesCoordsCount++;

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

/*
int ThreeDModelLoader::OBJLoader::getTextureCoordsArrayCount()
{
	return _pvTexturesCoords->size();
}
*/

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
	//_nNormalsCount++;

	NORMAL n;

	n.N1 = (GLfloat)atof(strtok(NULL, separater_space));
	n.N2 = (GLfloat)atof(strtok(NULL, separater_space));
	n.N3 = (GLfloat)atof(strtok(NULL, separater_space));

	_pvNormals->push_back(n);
}

/*
int ThreeDModelLoader::OBJLoader::getNormalArrayCount()
{
	return _pvNormals->size();
}
*/

bool ThreeDModelLoader::OBJLoader::isFaces(char* token)
{

	return  token!=NULL?(strcmp(token, "f") == 0):false;
}

void ThreeDModelLoader::OBJLoader::onFaces()
{
	//_nFacesCount++;

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
		char *duplicate = strdup(_szfileFullPath);
		int len = strlen(duplicate);
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
		_model->materialName = filename;
		_model->material_index = -1;
		_model->start_index = _pvFaces->size();
		
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
		_model->end_index = _pvFaces ->size()-1;
		_pvModelIndicesMapTable->push_back(_model);
		_model = NULL;
	}

}

/*
void ThreeDModelLoader::OBJLoader::reduceMeshOnGPU()
{
	FILE *gpuLogFP;
	bool isToDebugGPUInfo = true;
	fopen_s(&gpuLogFP, "gpudetails.txt", "w");

	auto updatedFaces = new std::vector<TRIANGLE_FACE_INDICES>(_pvFaces->size());
	if (updatedFaces == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";

	auto uniqueVertices = new std::vector<VERTEX>();
	if (uniqueVertices == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";

	fprintf(gpuLogFP,"IS GPU INFO ON: %d\n",isToDebugGPUInfo);
	//GPU DETAILS
	if (isToDebugGPUInfo)
	{
		fprintf(gpuLogFP, "\n\n\n********* GPU  INFO **************\n\n\n");

		std::vector<accelerator> accs = accelerator::get_all();
		for (int i = 0; i < (int)accs.size();i++) 
		{

			fprintf(gpuLogFP, "\n\n\n");
			fprintf(gpuLogFP, "\nDEVICE device_path: %S", accs[i].device_path.c_str());
			fprintf(gpuLogFP, "\nDEVICE description: %S", accs[i].description.c_str());
			fprintf(gpuLogFP, "\nDEVICE get_has_display: %d", accs[i].get_has_display());
			fprintf(gpuLogFP, "\nDEVICE get_is_emulated: %d", accs[i].get_is_emulated());
			fprintf(gpuLogFP, "\nDEVICE get_supports_cpu_shared_memory: %d", accs[i].get_supports_cpu_shared_memory());
			fprintf(gpuLogFP, "\nDEVICE get_version: %d", accs[i].get_version());
			fprintf(gpuLogFP, "\nDEVICE get_supports_double_precision: %d", accs[i].get_supports_double_precision());
			fprintf(gpuLogFP, "\nDEVICE get_supports_limited_double_precision: %d", accs[i].get_supports_limited_double_precision());
		}
	}
	//
	


	
	//pick_with_most_memory
	fprintf(gpuLogFP, "\n\n\n********* ACCELERATOR SETTING INFO **************\n\n\n");
	std::vector<accelerator> accs = accelerator::get_all();
	accelerator acc_chosen = accs[0];
	for (int i = 0; i < (int)accs.size();	i++) {
		if (accs[i].dedicated_memory> acc_chosen.dedicated_memory) {
			acc_chosen = accs[i];
		}
	}
	if (accelerator::set_default(acc_chosen.device_path))
	{
		fprintf(gpuLogFP, "\nACCELERATOR SET: %S", acc_chosen.description.c_str());
	}
	else
	{
		fprintf(gpuLogFP, "\nACCELERATOR NOT SET: %S", acc_chosen.description.c_str());
	}
	//



	//_verticesUnique and fill with IDENTIFIER , position ,IDENTIFIER,
	concurrency::array<VERTEX, 1> verticesClone_GPU(_pvVerticesClone->size(), _pvVerticesClone->begin(), _pvVerticesClone->end());
	int n = _pvVerticesClone->size();
	concurrency::parallel_for_each(acc_chosen.default_view,verticesClone_GPU.extent,[=, &verticesClone_GPU](index<1> i) restrict(amp)
	{
		for (int j = i[0]+1; j < (int)verticesClone_GPU.extent[0]; j++)
		{
			if ( (verticesClone_GPU[i].X == verticesClone_GPU[j].X) &&  (verticesClone_GPU[i].Y == verticesClone_GPU[j].Y) && (verticesClone_GPU[i].Z == verticesClone_GPU[j].Z))
			{
				if ((verticesClone_GPU[j].X != (GLfloat)IDENTIFIER_TOKEN) && (verticesClone_GPU[j].Z != (GLfloat)IDENTIFIER_TOKEN))
				{
					verticesClone_GPU[j] = { (GLfloat)IDENTIFIER_TOKEN,(GLfloat)i[0],(GLfloat)IDENTIFIER_TOKEN };
				}
			}
		}

	});
	verticesClone_GPU.accelerator_view.wait();
	concurrency::array_view<VERTEX, 1> av_verticesClone_GPU(_pvVerticesClone->size());
	verticesClone_GPU.copy_to(av_verticesClone_GPU);
	for (int i = 0; i < (int)_pvVerticesClone->size(); i++)
	{
		_pvVerticesClone->at(i) = av_verticesClone_GPU[i];
	}
	//





	//remove reduntant enteries from _vertices
	array<VERTEX, 1> uniqueVertices_GPU(_pvVerticesClone->size());
	int global_counter_storage_bsize[1] = { 0 };
	array_view<int> global_counter_bsize{ global_counter_storage_bsize };
	concurrency::parallel_for_each(acc_chosen.default_view,uniqueVertices_GPU.extent, [=, &uniqueVertices_GPU, &verticesClone_GPU](index<1> i)   restrict(amp)
	{
		index<1> index(0);
		if (verticesClone_GPU[i].X != (GLfloat)IDENTIFIER_TOKEN   &&   verticesClone_GPU[i].Z != (GLfloat)IDENTIFIER_TOKEN)
		{
			uniqueVertices_GPU[atomic_fetch_inc(&global_counter_bsize[0])] = verticesClone_GPU[i];
		}


	});
	verticesClone_GPU.accelerator_view.wait();
	_nVertexsUniqueCount = global_counter_bsize[0];
	concurrency::array_view<VERTEX, 1> av_uniqueVertices(_pvVerticesClone->size());
	uniqueVertices_GPU.copy_to(av_uniqueVertices);
	for (int j = 0; j < _nVertexsUniqueCount; j++)
	{
		uniqueVertices->push_back (av_uniqueVertices[j]);
	}
	//

	



	
	//
	concurrency::array<TRIANGLE_FACE_INDICES, 1> faces_GPU(_pvFaces->size(), _pvFaces->begin(), _pvFaces->end());
	concurrency::array<TRIANGLE_FACE_INDICES, 1> faceTable_GPU(_pvFaces->size());
	concurrency::array<VERTEX, 1> vertices_GPU(_pvVertices->size(), _pvVertices->begin(), _pvVertices->end());
	int bsize = _nVertexsUniqueCount;
	concurrency::parallel_for_each(acc_chosen.default_view,faces_GPU.extent, [=, &faceTable_GPU,&faces_GPU,&uniqueVertices_GPU,&vertices_GPU](index<1> i)    restrict(amp)
	{
		int index1 = faces_GPU[i].V1 - 1;
		int index2 = faces_GPU[i].V2 - 1;
		int index3 = faces_GPU[i].V3 - 1;

		GLfloat x1 = vertices_GPU[index1].X;
		GLfloat y1 = vertices_GPU[index1].Y;
		GLfloat z1 = vertices_GPU[index1].Z;

		GLfloat x2 = vertices_GPU[index2].X;
		GLfloat y2 = vertices_GPU[index2].Y;
		GLfloat z2 = vertices_GPU[index2].Z;

		GLfloat x3 = vertices_GPU[index3].X;
		GLfloat y3 = vertices_GPU[index3].Y;
		GLfloat z3 = vertices_GPU[index3].Z;


		int vertex1 = 0, vertex2 = 0, vertex3 = 0, counter = 0;
		int i1 = 0;
		for (int j = 0; j < bsize; j++)
		{
			if (x1 == uniqueVertices_GPU[j].X && y1 == uniqueVertices_GPU[j].Y && z1 == uniqueVertices_GPU[j].Z)
			{
				vertex1 = j;
				counter++;
			}
			if (x2 == uniqueVertices_GPU[j].X && y2 == uniqueVertices_GPU[j].Y && z2 == uniqueVertices_GPU[j].Z)
			{
				vertex2 = j;
				counter++;
			}
			if (x3 == uniqueVertices_GPU[j].X && y3 == uniqueVertices_GPU[j].Y && z3 == uniqueVertices_GPU[j].Z)
			{
				vertex3 = j;
				counter++;
			}
			if (counter == 3)
				break;

		}

		faceTable_GPU[i].V1 = vertex1;
		faceTable_GPU[i].V2 = vertex2;
		faceTable_GPU[i].V3 = vertex3;

	});
	faces_GPU.accelerator_view.wait();	
	concurrency::array_view<TRIANGLE_FACE_INDICES, 1> av_updatedFaces(_pvFaces->size());
	faceTable_GPU.copy_to(av_updatedFaces);
	for (int i = 0; i < (int)_pvFaces->size(); i++)
	{
		updatedFaces->at(i)=av_updatedFaces[i];
	}
	//


	//
	int size = updatedFaces->size() * sizeof(unsigned int)*3; // 3 for x,y,z
	_nIndices = (unsigned int*)malloc(size);
	_nIndicesCount = 0;
	for (int i = 0; i < (int)updatedFaces->size(); i++)
	{
		_nIndices[_nIndicesCount++] =(unsigned int) updatedFaces->at(i).V1;
		_nIndices[_nIndicesCount++] = (unsigned int)updatedFaces->at(i).V2;
		_nIndices[_nIndicesCount++] = (unsigned int)updatedFaces->at(i).V3;

	}
	//

	int orignalvertices = _pvVertices->size();
	int updatedvertices = uniqueVertices->size();


	//generate _uniqueVerticeArray
	_pfUniqueVerticeArray = (GLfloat*)malloc(sizeof(GLfloat)*uniqueVertices->size() * 3);
	_nUniqueVerticesArrayCount = 0;
	for (size_t i = 0; i < uniqueVertices->size(); i++)
	{
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).X;
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).Y;
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).Z;
	}
	//


	//MAP MATERIAL TO OBJECT
	mapOBJToMaterials();
	//


	//GENERATE CACHE FILE
	generateOBJCacheXML(uniqueVertices, updatedFaces, orignalvertices);
	//

	
		
		//DISPOSE/
	if (updatedFaces != NULL)
	{
		updatedFaces->clear();
		delete updatedFaces;
		updatedFaces = NULL;
	}
	if (uniqueVertices != NULL)
	{
		uniqueVertices->clear();
		delete uniqueVertices;
		uniqueVertices = NULL;
	}

	av_uniqueVertices.discard_data();
	av_updatedFaces.discard_data();
	av_verticesClone_GPU.discard_data();
	global_counter_bsize.discard_data();

	fclose(gpuLogFP);

}

void ThreeDModelLoader::OBJLoader::reduceMeshOnCPU()
{
	auto updatedFaces = new std::vector<TRIANGLE_FACE_INDICES>(_pvFaces->size());
	if (updatedFaces == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";

	auto uniqueVertices = new std::vector<VERTEX>();
	if (uniqueVertices == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";

	//_verticesUnique and fill with IDENTIFIER , position ,IDENTIFIER,
	for (int i = 0; i < (int)_pvVerticesClone->size(); i++)
	{

		for (int j = i+1; j < (int)_pvVerticesClone->size(); j++)
		{
			if (_pvVerticesClone->at(i).X == _pvVerticesClone->at(j).X && _pvVerticesClone->at(i).Y == _pvVerticesClone->at(j).Y && _pvVerticesClone->at(i).Z == _pvVerticesClone->at(j).Z)
			{
				if (_pvVerticesClone->at(j).X == IDENTIFIER_TOKEN && _pvVerticesClone->at(j).Z == IDENTIFIER_TOKEN)
				{
					continue;
				}

				_pvVerticesClone->at(j).X = IDENTIFIER_TOKEN;
				_pvVerticesClone->at(j).Y = (GLfloat)i;
				_pvVerticesClone->at(j).Z = IDENTIFIER_TOKEN;
			}

		}

	}
	//



	//remove reduntant enteries from _vertices
	for (int i = 0; i < (int)_pvVerticesClone->size(); i++)
	{
		if (_pvVerticesClone->at(i).X != IDENTIFIER_TOKEN && _pvVerticesClone->at(i).Z != IDENTIFIER_TOKEN)
		{
			uniqueVertices->push_back(_pvVerticesClone->at(i));
		}
	}
	_nVertexsUniqueCount = uniqueVertices->size();
	//






	//modifiy the enteries in _face/
	for (int i = 0; i < (int)_pvFaces->size(); i++)
	{
		int index1 = _pvFaces->at(i).V1 - 1;
		int index2 = _pvFaces->at(i).V2 - 1;
		int index3 = _pvFaces->at(i).V3 - 1;

		GLfloat x1 = _pvVertices->at(index1).X;
		GLfloat y1 = _pvVertices->at(index1).Y;
		GLfloat z1 = _pvVertices->at(index1).Z;

		GLfloat x2 = _pvVertices->at(index2).X;
		GLfloat y2 = _pvVertices->at(index2).Y;
		GLfloat z2 = _pvVertices->at(index2).Z;

		GLfloat x3 = _pvVertices->at(index3).X;
		GLfloat y3 = _pvVertices->at(index3).Y;
		GLfloat z3 = _pvVertices->at(index3).Z;


		int vertex1 = 0, vertex2 = 0, vertex3 = 0, counter = 0;
		for (int j = 0; j < (int)uniqueVertices->size(); j++)
		{
			if (x1 == uniqueVertices->at(j).X && y1 == uniqueVertices->at(j).Y && z1 == uniqueVertices->at(j).Z)
			{
				vertex1 = j;
				counter++;
			}
			if (x2 == uniqueVertices->at(j).X && y2 == uniqueVertices->at(j).Y && z2 == uniqueVertices->at(j).Z)
			{
				vertex2 = j;
				counter++;
			}
			if (x3 == uniqueVertices->at(j).X && y3 == uniqueVertices->at(j).Y && z3 == uniqueVertices->at(j).Z)
			{
				vertex3 = j;
				counter++;
			}
			if (counter == 3)
				break;

		}

		updatedFaces->at(i).V1 = vertex1;
		updatedFaces->at(i).V2 = vertex2;
		updatedFaces->at(i).V3 = vertex3;
	
	}
	//


	//generate _nIndices array
	int size = updatedFaces->size() * sizeof(unsigned int) * 3; // 3 for x,y,z
	_nIndices = (unsigned int*)malloc(size);

	_nIndicesCount = 0;
	for (int i = 0; i < (int)updatedFaces->size(); i++)
	{
		_nIndices[_nIndicesCount++] = (unsigned int)updatedFaces->at(i).V1;
		_nIndices[_nIndicesCount++] = (unsigned int)updatedFaces->at(i).V2;
		_nIndices[_nIndicesCount++] = (unsigned int)updatedFaces->at(i).V3;

	}
	//


	int orignalvertices = _pvVertices->size();
	int uniquevertices = uniqueVertices->size();


	//generate _uniqueVerticeArray
	_pfUniqueVerticeArray = (GLfloat*)malloc(sizeof(GLfloat)*uniqueVertices->size() * 3);
	_nUniqueVerticesArrayCount = 0;

	for (size_t i = 0; i < uniqueVertices->size(); i++)
	{
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).X;
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).Y;
		_pfUniqueVerticeArray[_nUniqueVerticesArrayCount++] = uniqueVertices->at(i).Z;
	}
	//


	//MAP MATERIAL TO OBJECT//
	mapOBJToMaterials();
	//


	//GENERATE CACHE FILE
	generateOBJCacheXML(uniqueVertices, updatedFaces, orignalvertices);
	//

	
	//DISPOSE//
	if (updatedFaces != NULL)
	
	{
		updatedFaces->clear();
		delete updatedFaces;
		updatedFaces = NULL;
	}
	if (uniqueVertices != NULL)
	{
		uniqueVertices->clear();
		delete uniqueVertices;
		uniqueVertices = NULL;
	}
	//
}
*/

/*
int ThreeDModelLoader::OBJLoader::getFaceArrayCount()
{
	return _pvFaces->size();
}
*/

/*
void ThreeDModelLoader::OBJLoader::generateOBJCacheXML(std::vector<VERTEX>  *updatedvertices, std::vector<TRIANGLE_FACE_INDICES>  *updatedfaces ,int orignalvertices)
{
	FILE *pcacheFp = NULL;

	int length = strlen(_szfileFullPath)+5;
	char *newFilename = NULL;
	newFilename = (char*)malloc(sizeof(char)*length);

	strcpy(newFilename,_szfileFullPath);
	strcat(newFilename,".cache");

	fopen_s(&pcacheFp, newFilename, "w");
	if (pcacheFp == NULL) throw "EXCEPTION: UNABLE TO CREATE CACHE FILE";

	//init XML file
	fprintf(pcacheFp, "\n%s", "<!-- @author :Praveen Suryawanshi-->");
	fprintf(pcacheFp, "\n%s", "<XML>");
	//

	//

	//
	fprintf(pcacheFp, "\n<!--ORIGNAL VERTICES : %d : REDUCED TO %d-->", orignalvertices, updatedvertices->size());
	//

	//write vertices
	fprintf(pcacheFp, "\n<vertices length=%d count=3 >", updatedvertices->size());
	for (size_t i = 0; i < updatedvertices->size(); i = i+1)
	{
		fprintf(pcacheFp, "\n\t%f %f %f", updatedvertices->at(i).X, updatedvertices->at(i).Y, updatedvertices->at(i).Z);
	}
	fprintf(pcacheFp, "\n</vertices>");
	//


	//write texture coord
	fprintf(pcacheFp, "\n<texture_coord length=%d count=%d >", _pvTexturesCoords->size(), _nTexCordCount);
	for (size_t i = 0; i < _pvTexturesCoords->size(); i = i + 1)
	{
		if (_nTexCordCount == 2)
		{
			fprintf(pcacheFp, "\n\t%f %f", _pvTexturesCoords->at(i).S, _pvTexturesCoords->at(i).T);
		}
		if (_nTexCordCount == 3)
		{
			fprintf(pcacheFp, "\n\t%f %f %f", _pvTexturesCoords->at(i).S, _pvTexturesCoords->at(i).T, _pvTexturesCoords->at(i).R);
		}
		if (_nTexCordCount == 4)
		{
			fprintf(pcacheFp, "\n\t%f %f %f %f", _pvTexturesCoords->at(i).S, _pvTexturesCoords->at(i).T, _pvTexturesCoords->at(i).R, _pvTexturesCoords->at(i).Q);
		}

	}
	fprintf(pcacheFp, "\n</texture_coord>");
	//


	//write normals
	fprintf(pcacheFp, "\n<normals length=%d count=3 >", _pvNormals->size());
	for (size_t i = 0; i < _pvNormals->size(); i = i + 1)
	{
		fprintf(pcacheFp, "\n\t%f %f %f", _pvNormals->at(i).X, _pvNormals->at(i).Y, _pvNormals->at(i).Z);
	}
	fprintf(pcacheFp, "\n</normals>");
	//

	//write faces
	fprintf(pcacheFp, "\n<faces length=%d count=12 >", updatedfaces->size());
	for (size_t i = 0; i < _pvNormals->size(); i = i + 1)
	{
		fprintf(pcacheFp, "\n\t%d/%d/%d\t%d/%d/%d\t%d/%d/%d", 
			updatedfaces->at(i).V1, _pvFaces->at(i).T1, _pvFaces->at(i).N1,
			updatedfaces->at(i).V2, _pvFaces->at(i).T2, _pvFaces->at(i).N2,
			updatedfaces->at(i).V3, _pvFaces->at(i).T3, _pvFaces->at(i).N3);
	}
	fprintf(pcacheFp, "\n</faces>");
	//



	//write components
	if (_pvModelIndicesMapTable->size() > 0)
	{
		fprintf(pcacheFp, "\n<components lenght=%d>", _pvModelIndicesMapTable->size());
		for (size_t i = 0; i < _pvModelIndicesMapTable->size(); i++)
		{
			fprintf(pcacheFp, "\n\t<component>");
			fprintf(pcacheFp, "\n\t\t<start_index>%d</start_index>", _pvModelIndicesMapTable->at(i)->start_index);
			fprintf(pcacheFp, "\n\t\t<end_index>%d</end_index>", _pvModelIndicesMapTable->at(i)->end_index);
			fprintf(pcacheFp, "\n\t\t<material_index>%d</material_index>", _pvModelIndicesMapTable->at(i)->material_index);
			fprintf(pcacheFp, "\n\t\t<material_name>%s</material_name>", _pvModelIndicesMapTable->at(i)->materialName);
			fprintf(pcacheFp, "\n\t</component>");
		}
		fprintf(pcacheFp, "\n</components>", _pvModelIndicesMapTable->size());
	}
	//

	//

	//exit XML file
	fprintf(pcacheFp, "\n%s", "</XML>");

	fclose(pcacheFp);

	if (newFilename != NULL)
	{
		free(newFilename);
		newFilename = NULL;
	}
}

*/


/*
void ThreeDModelLoader::OBJLoader::mapOBJToMaterials()
{
	if (_pMtlLoader == NULL )//when no material file is present
		return;
	
		std::vector<MATERIAL*> *material = _pMtlLoader->getMaterials();


		if (material != NULL && material->size() > 0)
		{
			for (int i = 0; i < (int)_pvModelIndicesMapTable->size(); i++)
			{
				for (int j = 0; j <(int)material->size(); j++)
				{
					if (strcmp(_pvModelIndicesMapTable->at(i)->materialName, material->at(j)->name) == 0)
					{
						_pvModelIndicesMapTable->at(i)->material_index = j;
						break;
					}

				}
			}
		}
}
*/

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
	
	/*if (_pfUniqueVerticeArray != NULL)
	{
		free(_pfUniqueVerticeArray);
		_pfUniqueVerticeArray = NULL;
	}*/


	if (_pvVertices != NULL)
	{
		_pvVertices->clear();
		delete _pvVertices;
		_pvVertices = NULL;
	}
	/*if (_pvVerticesClone != NULL)
	{
		_pvVerticesClone->clear();
		delete _pvVerticesClone;
		_pvVerticesClone = NULL;
	}*/

	if (_pvModelIndicesMapTable != NULL)
	{
		_pvModelIndicesMapTable->clear();
		delete _pvModelIndicesMapTable;
		_pvModelIndicesMapTable = NULL;
	}

	if (_pvTexturesCoords != NULL)
	{
		_pvTexturesCoords->clear();
		delete _pvTexturesCoords;
		_pvTexturesCoords = NULL;
	}

	if (_pvNormals != NULL)
	{
		_pvNormals->clear();
		delete _pvNormals;
		_pvNormals = NULL;
	}
	

	if (_pvFaces != NULL)
	{
		_pvFaces->clear();
		delete _pvFaces;
		_pvFaces = NULL;
	}


	if (_pMtlLoader != NULL)
	{
		delete _pMtlLoader;
		_pMtlLoader = NULL;
	}

		
}
