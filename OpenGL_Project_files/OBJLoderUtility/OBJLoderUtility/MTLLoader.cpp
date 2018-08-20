/*
* @author: Praveen Suryawanshi
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/
#include "MTLLoader.h"


ThreeDModelLoader::MTLLoader::MTLLoader(char * filename)
{
	_szMtlLib = NULL;

	_szMtlLib = filename;

	_szNextLine = NULL;

	_pvMaterial = NULL;


	_pvMaterial = new std::vector<MATERIAL*>();
	if (_pvMaterial == NULL)
		throw "EXCEPTION: OBJECT NULL EXCEPTION";
}

void ThreeDModelLoader::MTLLoader::parse()
{
	const int line_size = 256;
	char line[line_size];
	char *token = NULL;

	if (_szMtlLib == NULL)
		return;

	FILE * mtlFileFp= NULL;

	fopen_s(&mtlFileFp, _szMtlLib, "r");
	if (mtlFileFp == NULL) throw "EXCEPTION: UNABLE TO OPEN FILE";

	
	while (true)
	{
		memset((void*)line, (int)'\0', line_size);
		token = NULL;

		if (_szNextLine == NULL)
		{
			if (fgets(line, line_size, mtlFileFp) == NULL)
				break;
		}
		else
		{
			strcpy(line, _szNextLine);
		}

		int number_of_tokens_in_line = getTotalTokens(line);
		
		char *line_clone = strdup(line);
		token = strtok(line, separater_space);
		
		if (isNewMaterial(token))
		{
			char temp[100];
			char materialname[100];
			sscanf(line_clone,"%s %s",&temp,&materialname);
			if (onNewMaterial(mtlFileFp, materialname) == false)
				break;
		}

		if (line_clone != NULL)
		{
			free(line_clone);
			line_clone = NULL;
		}
	}

	fclose(mtlFileFp);	
}

int ThreeDModelLoader::MTLLoader::getTotalTokens(char * line)
{
	int count = 0, i;

	for (i = 0;line[i] != '\0';i++)
	{
		if (line[i] == ' ')
			count++;
	}

	return count;
}

bool ThreeDModelLoader::MTLLoader::isNewMaterial(char * token)
{
	return  token != NULL ? (strcmp(token, "newmtl") == 0) : false;
}

bool ThreeDModelLoader::MTLLoader::onNewMaterial(FILE *mtlFileFp, char *materialName)
{
	const int line_size = 256;
	char line[line_size];
	char *token = NULL;
	bool eof = false;

	_material = NULL;
	_material = (MATERIAL*)malloc(sizeof(MATERIAL));
	/*DEFAULT VALUES INITIALIZATION*/
	_material->Ka[0] = 0.2f;_material->Ka[1] = 0.2f;_material->Ka[2] = 0.2f;
	_material->Kd[0] = 0.8f;_material->Ka[1] = 0.8f;_material->Ka[2] = 0.8f;
	_material->Ks[0] = 1.0f;_material->Ka[1] = 1.0f;_material->Ka[2] = 1.0f;
	_material->d = 1.0f;
	_material->Tr = 1.0f;
	_material->Ns = 0.0f;
	//illum = 1 :ndicates a flat material with no specular highlights so the value of Ks is not used
	//illum = 2: denotes the presence of specular highlights, and so a specification for Ks is required.
	/**/

	strcpy(_material->name, materialName);

	while (true)
	{
		
		
		memset((void*)line, (int)'\0', line_size);
		token = NULL;

		
		if (fgets(line, line_size, mtlFileFp) == NULL)
		{
			eof = true;
			break;
		}

		char * line_clone = strdup(line);

		token = strtok(line, separater_space);

		if (isNewMaterial(token))
		{
			_szNextLine = line_clone;
			break;
			
		}
		
		if (isMaterialAmbient(token))
			onMaterialAmbient(line_clone);

		if (isMaterialDiffuse(token))
			onMaterialDiffuse(line_clone);

		if (isMaterialSpecular(token))
			onMaterialSpecular(line_clone);

		if (isMaterialEmission(token))
			onMaterialEmission(line_clone);

		if (isMaterialAlpha(token))
			onMaterialAlpha(line_clone);

		if (isMaterialIllum(token))
			onMaterialIllum(line_clone);

		if (isMaterialTextureMapDiffuse(token))
			onMaterialTextureMapDiffuse(line_clone);

		if (isMaterialTextureMapSpecular(token))
			onMaterialTextureMapSpecular(line_clone);

		if (line_clone != NULL)
		{
			free(line_clone);
			line_clone = NULL;
		}
	}

	_pvMaterial->push_back(_material);
	return (eof == true)?false:true;

	
}

bool ThreeDModelLoader::MTLLoader::isMaterialAmbient(char * token)
{
	return  token != NULL ? (strcmp(token, "Ka") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialAmbient(char *line)
{
	GLfloat r = NULL, g = NULL, b = NULL;
	char token_name[25];
	sscanf(line,"%s %f %f %f",token_name,&r,&g,&b);
	_material->Ka[0] = r;
	_material->Ka[1] = g;
	_material->Ka[2] = b;
}

bool ThreeDModelLoader::MTLLoader::isMaterialDiffuse(char * token)
{
	return  token != NULL ? (strcmp(token, "Kd") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialDiffuse( char *line)
{
	GLfloat r = NULL, g = NULL, b = NULL;
	char token_name[25];
	sscanf(line, "%s %f %f %f", token_name, &r, &g, &b);
	_material->Kd[0] = r;
	_material->Kd[1] = g;
	_material->Kd[2] = b;
}

bool ThreeDModelLoader::MTLLoader::isMaterialSpecular(char * token)
{
	return   token != NULL ? (strcmp(token, "Ks") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialSpecular(char *line)
{
	GLfloat r = NULL, g = NULL, b = NULL;
	char token_name[25];
	sscanf(line, "%s %f %f %f", token_name, &r, &g, &b);
	_material->Ks[0] = r;
	_material->Ks[1] = g;
	_material->Ks[2] = b;
}

bool ThreeDModelLoader::MTLLoader::isMaterialEmission(char * token)
{
	return   token != NULL ? (strcmp(token, "Ke") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialEmission(char *line)
{
	GLfloat r = NULL, g = NULL, b = NULL;
	char token_name[25];
	sscanf(line, "%s %f %f %f", token_name, &r, &g, &b);
	_material->Ke[0] = r;
	_material->Ke[1] = g;
	_material->Ke[2] = b;
}

bool ThreeDModelLoader::MTLLoader::isMaterialAlpha(char * token)
{
	return token != NULL ? (strcmp(token, "d") == 0) || (strcmp(token, "Tr") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialAlpha(char *line)
{
	GLfloat val = NULL;
	char token_name[25];
	sscanf(line, "%s %f", token_name, &val);
	_material->d = val;
	_material->Tr = val;

}

bool ThreeDModelLoader::MTLLoader::isMaterialIllum(char * token)
{
	return token != NULL ? (strcmp(token, "illum") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialIllum( char *line)
{
	GLfloat val = NULL;
	char token_name[25];
	sscanf(line, "%s %f", token_name, &val);
	_material->illum = val;
}

bool ThreeDModelLoader::MTLLoader::isMaterialTextureMapAmbient(char * token)
{
	return token != NULL ? (strcmp(token, "map_Ka") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialTextureMapAmbient( char * line)
{
	char token_name[25], model_name[50], texture_name[256];
	sscanf(line, "%s %s %s", token_name, model_name, texture_name);
	_material->map_Ka.modelname = model_name;
	_material->map_Ka.texturename = texture_name;
}

bool ThreeDModelLoader::MTLLoader::isMaterialTextureMapDiffuse(char * token)
{
	return token != NULL ? (strcmp(token, "map_Kd") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialTextureMapDiffuse( char *line)
{
	char token_name[25], model_name[50], texture_name[256];
	sscanf(line, "%s %s %s", token_name,model_name,texture_name);
	_material->map_Kd.modelname = model_name;
	_material->map_Kd.texturename = texture_name;
}

bool ThreeDModelLoader::MTLLoader::isMaterialTextureMapSpecular(char * token)
{
	return token != NULL ? (strcmp(token, "map_Kd") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialTextureMapSpecular(char * line)
{
	char token_name[25], model_name[50], texture_name[256];
	sscanf(line, "%s %s %s", token_name, model_name, texture_name);
	_material->map_Ks.modelname = model_name;
	_material->map_Ks.texturename = texture_name;
}

bool ThreeDModelLoader::MTLLoader::isMaterialSpecularExponent(char * token)
{
	return token != NULL ? (strcmp(token, "Ns") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialSpecularExponent( char * line)
{
	char token_name[25];
	GLfloat  val = 0;
	sscanf(line, "%s %f", token_name,&val);
	_material->Ns = val;
}

bool ThreeDModelLoader::MTLLoader::isMaterialOpticalDensity(char * token)
{
	return token != NULL ? (strcmp(token, "Ni") == 0) : false;
}

void ThreeDModelLoader::MTLLoader::onMaterialOpticalDensity( char * line)
{
	char token_name[25];
	GLfloat  val = NULL;
	sscanf(line, "%s %f", token_name, &val);
	_material->Ni = val;
}

char * ThreeDModelLoader::MTLLoader::getMtlLibFileName()
{
	return _szMtlLib;
}

std::vector<ThreeDModelLoader::MATERIAL*>* ThreeDModelLoader::MTLLoader::getMaterials()
{
	return _pvMaterial;
}

ThreeDModelLoader::MTLLoader::~MTLLoader()
{
	if (_pvMaterial != NULL)
	{
		_pvMaterial->clear();
		delete _pvMaterial;
	}

	if (_szNextLine != NULL)
	{
		free(_szNextLine);
		_szNextLine = NULL;
	}

}
