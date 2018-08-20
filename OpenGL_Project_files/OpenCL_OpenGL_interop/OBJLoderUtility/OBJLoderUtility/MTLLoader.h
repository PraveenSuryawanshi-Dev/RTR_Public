/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/

#pragma once
#ifndef  _MTL_LOADER_
#define _MTL_LOADER_
#include<Windows.h>
#include<gl/GL.h>
#include <vector>
//http://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
//https://www.youtube.com/watch?v=XIVUxywOyjE
//https://xiangchen.wordpress.com/2010/05/04/loading-a-obj-file-in-opengl/
//http://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
//https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/27982/versions/5/previews/help%20file%20format/MTL_format.html
//https://en.wikipedia.org/wiki/Wavefront_.obj_file

#define CHECK_ON_NULL_THROW_EXCEPTION(obj){if(obj == NULL){ delete obj;obj=NULL;throw "EXCEPTION: OBJECT NULL EXCEPTION";}} 

namespace ThreeDModelLoader
{
	typedef struct _texture_
	{
		char * modelname;
		char * texturename;

	}TEXTURE;

	typedef struct _material_ {
		char  name[256];
		GLfloat Ns;
		GLfloat Ni;

		GLfloat Ka[3];
		GLfloat Kd[3];
		GLfloat Ks[3];
		GLfloat Ke[3];
		
		GLfloat d; //alpha >> d instead of Tr
		GLfloat Tr; //aloha >> Tr instred of d
		GLfloat Tf; //need to add prototype
		
		GLfloat illum;
		
		TEXTURE map_Ka;
		TEXTURE map_Kd;
		TEXTURE map_Ks;
		
	}MATERIAL;

	class MTLLoader
	{

		const char *separater_space = " ";

	private:

		int getTotalTokens(char *line);

		bool isNewMaterial(char *token);
		bool onNewMaterial(FILE *mtlFileFp, char *materialName);

		bool isMaterialAmbient(char *token);
		void onMaterialAmbient(char *line);

		bool isMaterialDiffuse(char *token);
		void onMaterialDiffuse(char *line);


		bool isMaterialSpecular(char *token);
		void onMaterialSpecular(char *line);

		bool isMaterialEmission(char *token);
		void onMaterialEmission(char *line);


		bool isMaterialAlpha(char *token);
		void onMaterialAlpha(char *line);

		bool isMaterialIllum(char *token);
		void onMaterialIllum( char *line);

		bool isMaterialTextureMapAmbient(char *token);
		void onMaterialTextureMapAmbient(char *line);


		bool isMaterialTextureMapDiffuse(char *token);
		void onMaterialTextureMapDiffuse( char *line);

		bool isMaterialTextureMapSpecular(char *token);
		void onMaterialTextureMapSpecular(char *line);

		bool isMaterialSpecularExponent(char *token);
		void onMaterialSpecularExponent(char *line);

		bool isMaterialOpticalDensity(char *token);
		void onMaterialOpticalDensity( char *line);



	public:
		
		MTLLoader(char *filename);


		void parse();


		char * getMtlLibFileName();

		std::vector<MATERIAL*> *getMaterials();

		~MTLLoader();

	private:

		char *_szMtlLib;
		std::vector<MATERIAL*> *_pvMaterial;
		MATERIAL *_material;
		char *_szNextLine;

	};
}
#endif