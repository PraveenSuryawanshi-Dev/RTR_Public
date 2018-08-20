#pragma once
//////////////////////////////////////////////////////// sphere code //////////////////////////////////////////////////////////////
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
//#include <gl\glew.h>
//#include<gl/GL.h>

/*
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
*/
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

enum
{
	SPHERE_ATTRIBUTE_POSITION = 0,
	SPHERE_ATTRIBUTE_COLOR,
	SPHERE_ATTRIBUTE_NORMAL,
	SPHERE_ATTRIBUTE_TEXTURE0,
};

#define SPHERE_VDG_PI  3.14159265358979323846

unsigned short* sphere_elements;
GLfloat* sphere_verts;
GLfloat* sphere_norms;
GLfloat* sphere_texCoords;


int sphere_numElements;
int sphere_maxElements;
int sphere_numVertices;


GLuint sphere_vbo_position;
GLuint sphere_vbo_normal;
GLuint sphere_vbo_texture;
GLuint sphere_vbo_index;
GLuint sphere_vao;


void spehereMesh_allocate(int numIndices);
void sphereMesh_AddTriangle(GLfloat **single_vertex, GLfloat **single_normal, GLfloat **single_texture);
void sphereMesh_prepareToDraw();
void drawSphere();
int getSphereIndexCount();
int getSphereVertexCount();
void SpherenormalizeVector(GLfloat *v);
bool isFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff);
void cleanupSphereMeshData();
void cleanupSphereBuffers();



void spehereMesh_allocate(int numIndices)
{

	cleanupSphereMeshData();

	sphere_maxElements = numIndices;
	sphere_numElements = 0;
	sphere_numVertices = 0;

	int iNumIndices = numIndices / 3;

	sphere_elements = (unsigned short*)malloc(iNumIndices * 3 * sizeof(unsigned short));// 3 is x,y,z and 2 is sizeof short
	sphere_verts = (GLfloat*)malloc(iNumIndices * 3 * sizeof(GLfloat)); // 3 is x,y,z and 4 is sizeof float
	sphere_norms = (GLfloat*)malloc(iNumIndices * 3 * sizeof(GLfloat)); // 3 is x,y,z and 4 is sizeof float
	sphere_texCoords = (GLfloat*)malloc(iNumIndices * 2 * sizeof(GLfloat));// 2 is s,t and 4 is sizeof float

}


// Add 3 vertices, 3 normal and 2 texcoords i.e. one triangle to the geometry.
// This searches the current list for identical vertices (exactly or nearly) and
// if one is found, it is added to the index array.
// if not, it is added to both the index array and the vertex array.
void sphereMesh_AddTriangle(GLfloat **single_vertex, GLfloat **single_normal, GLfloat **single_texture)
{
	const GLfloat diff = 0.00001f;
	int i, j;


	SpherenormalizeVector(single_normal[0]);
	SpherenormalizeVector(single_normal[1]);
	SpherenormalizeVector(single_normal[2]);


	for (i = 0; i < 3; i++)
	{
		/* code */

		for (j = 0; j < sphere_numVertices; j++)
		{
			/* code */

			if (isFoundIdentical(sphere_verts[j * 3], single_vertex[i][0], diff) &&
				isFoundIdentical(sphere_verts[(j * 3) + 1], single_vertex[i][1], diff) &&
				isFoundIdentical(sphere_verts[(j * 3) + 2], single_vertex[i][2], diff) &&

				isFoundIdentical(sphere_norms[j * 3], single_normal[i][0], diff) &&
				isFoundIdentical(sphere_norms[(j * 3) + 1], single_normal[i][1], diff) &&
				isFoundIdentical(sphere_norms[(j * 3) + 2], single_normal[i][2], diff) &&

				isFoundIdentical(sphere_texCoords[j * 2], single_texture[i][0], diff) &&
				isFoundIdentical(sphere_texCoords[(j * 2) + 1], single_texture[i][1], diff)
				) {
				sphere_elements[sphere_numElements] = (short)j;
				sphere_numElements++;
				break;
			}
		}


		if ((j == sphere_numVertices) && (sphere_numVertices < sphere_maxElements) && (sphere_numElements < sphere_maxElements))
		{

			sphere_verts[sphere_numVertices * 3] = single_vertex[i][0];
			sphere_verts[(sphere_numVertices * 3) + 1] = single_vertex[i][1];
			sphere_verts[(sphere_numVertices * 3) + 2] = single_vertex[i][2];

			sphere_norms[sphere_numVertices * 3] = single_normal[i][0];
			sphere_norms[(sphere_numVertices * 3) + 1] = single_normal[i][1];
			sphere_norms[(sphere_numVertices * 3) + 2] = single_normal[i][2];

			sphere_texCoords[sphere_numVertices * 2] = single_texture[i][0];
			sphere_texCoords[(sphere_numVertices * 2) + 1] = single_texture[i][1];

			sphere_elements[sphere_numElements] = (short)sphere_numVertices;
			sphere_numElements++;
			sphere_numVertices++;

		}

	}
	//
}

void sphereMesh_prepareToDraw()
{
	// vao
	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);

	// vbo for position
	glGenBuffers(1, &sphere_vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_position);

	glBufferData(GL_ARRAY_BUFFER, (sphere_maxElements * 3 * sizeof(GLfloat) / 3), sphere_verts, GL_STATIC_DRAW);
	glVertexAttribPointer(SPHERE_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPHERE_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//


	// vbo for normals
	glGenBuffers(1, &sphere_vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_normal);

	glBufferData(GL_ARRAY_BUFFER, (sphere_maxElements * 3 * sizeof(GLfloat) / 3), sphere_norms, GL_STATIC_DRAW);
	glVertexAttribPointer(SPHERE_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPHERE_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//


	// vbo for texture
	glGenBuffers(1, &sphere_vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_texture);

	glBufferData(GL_ARRAY_BUFFER, (sphere_maxElements * 2 * sizeof(GLfloat) / 3), sphere_texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(SPHERE_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPHERE_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//


	// vbo for index
	glGenBuffers(1, &sphere_vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbo_index);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sphere_maxElements * 3 * sizeof(unsigned short) / 3), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//


	// after sending data to GPU, now we can free our arrays
	cleanupSphereMeshData();

}

void drawSphere()
{
	//code
	//bind vao
	glBindVertexArray(sphere_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbo_index);
	glDrawElements(GL_TRIANGLES, sphere_numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);

}


int getSphereIndexCount()
{
	return sphere_numElements;
}




int getSphereVertexCount()
{
	return sphere_numVertices;
}



void SpherenormalizeVector(GLfloat *v)
{
	// square the vector length
	GLfloat squaredVectorLength = ((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));

	// get square root of above 'squared vector length'
	GLfloat squareRootOfSquaredVectorLength = (GLfloat)sqrt(squaredVectorLength);

	// scale the vector with 1/squareRootOfSquaredVectorLength
	v[0] = v[0] * 1.0f / squareRootOfSquaredVectorLength;
	v[1] = v[1] * 1.0f / squareRootOfSquaredVectorLength;
	v[2] = v[2] * 1.0f / squareRootOfSquaredVectorLength;
}



bool isFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff)
{
	if (fabs(val1 - val2) < diff)
		return true;
	else
		return false;
}


void cleanupSphereMeshData()
{

	//dispose
	// code
	if (sphere_elements != NULL)
	{
		free(sphere_elements);
		sphere_elements = NULL;
	}

	if (sphere_verts != NULL)
	{
		free(sphere_verts);
		sphere_verts = NULL;
	}

	if (sphere_norms != NULL)
	{
		free(sphere_norms);
		sphere_norms = NULL;
	}

	if (sphere_texCoords != NULL)
	{
		free(sphere_texCoords);
		sphere_texCoords = NULL;
	}
	//

}

inline void cleanupSphereBuffers()
{
	if (sphere_vao)
	{
		glDeleteBuffers(1, &sphere_vao);
		sphere_vao = (GLint)NULL;
	}

	if (sphere_vbo_index)
	{
		glDeleteBuffers(1, &sphere_vbo_index);
		sphere_vbo_index = (GLint)NULL;
	}

	if (sphere_vbo_texture)
	{
		glDeleteBuffers(1, &sphere_vbo_texture);
		sphere_vbo_texture =(GLint)NULL;
	}

	if (sphere_vbo_normal)
	{
		glDeleteBuffers(1, &sphere_vbo_normal);
		sphere_vbo_normal = (GLint)NULL;
	}

	if (sphere_vbo_position)
	{
		glDeleteBuffers(1, &sphere_vbo_position);
		sphere_vbo_position = (GLint)NULL;
	}
}











void makeSphere(int fRadius, int iSlices, int iStacks)
{
	//code
	

	GLfloat drho = (GLfloat)SPHERE_VDG_PI / (GLfloat)iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)SPHERE_VDG_PI / (GLfloat)iSlices;
	GLfloat ds = 1.0f / (GLfloat)iSlices;
	GLfloat dt = 1.0f / (GLfloat)iStacks;
	GLfloat t = 1.0f;
	GLfloat s = 0.0f;
	int i = 0;
	int j = 0;



	spehereMesh_allocate(iSlices * iStacks * 6);


	for (i = 0; i < iStacks; i++)
	{
		/* code */

		GLfloat rho = (GLfloat)(i * drho);
		GLfloat srho = (GLfloat)(sin(rho));
		GLfloat crho = (GLfloat)(cos(rho));
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));

		// Many sources of OpenGL sphere drawing code uses a triangle fan
		// for the caps of the sphere. This however introduces texturing
		// artifacts at the poles on some OpenGL implementations
		s = 0.0f;

		// initialization of three 2-D arrays, two are 4 x 3 and one is 4 x 2
		GLfloat **vertex = (GLfloat **)malloc(sizeof(GLfloat *) * 4); // 4 rows
		for (int a = 0; a < 4; a++)
			vertex[a] = (GLfloat *)malloc(sizeof(GLfloat) * 3); // 3 columns
		
		GLfloat **normal = (GLfloat **)malloc(sizeof(GLfloat *) * 4); // 4 rows
		for (int a = 0;a < 4;a++)
			normal[a] = (GLfloat *)malloc(sizeof(GLfloat) * 3); // 3 columns
		
		GLfloat **texture = (GLfloat **)malloc(sizeof(GLfloat *) * 4); // 4 rows
		for (int a = 0;a < 4;a++)
			texture[a] = (GLfloat *)malloc(sizeof(GLfloat) * 2); // 2 columns



		for (j = 0; j < iSlices; j++)
		{
			/* code */

			GLfloat thetha = (j == iSlices) ? 0.0f : j * dtheta;
			GLfloat sthetha = (GLfloat)(-sin(thetha));
			GLfloat cthetha = (GLfloat)(cos(thetha));

			GLfloat x = sthetha * srho;
			GLfloat y = cthetha * srho;
			GLfloat z = crho;


			texture[0][0] = s;
			texture[0][1] = t;

			normal[0][0] = x;
			normal[0][1] = y;
			normal[0][2] = z;

			vertex[0][0] = x * fRadius;
			vertex[0][1] = y * fRadius;
			vertex[0][2] = z * fRadius;


			x = sthetha * srhodrho;
			y = cthetha * srhodrho;
			z = crhodrho;

			texture[1][0] = s;
			texture[1][1] = t - dt;

			normal[1][0] = x;
			normal[1][1] = y;
			normal[1][2] = z;

			vertex[1][0] = x * fRadius;
			vertex[1][1] = y * fRadius;
			vertex[1][2] = z * fRadius;


			thetha = ((j + 1) == iSlices) ? 0.0f : (j + 1) * dtheta;
			sthetha = (GLfloat)(-sin(thetha));
			cthetha = (GLfloat)(cos(thetha));


			x = sthetha * srho;
			y = cthetha * srho;
			z = crho;

			s += ds;

			texture[2][0] = s;
			texture[2][1] = t;

			normal[2][0] = x;
			normal[2][1] = y;
			normal[2][2] = z;

			vertex[2][0] = x * fRadius;
			vertex[2][1] = y * fRadius;
			vertex[2][2] = z * fRadius;


			x = sthetha * srhodrho;
			y = cthetha * srhodrho;
			z = crhodrho;

			texture[3][0] = s;
			texture[3][1] = t - dt;

			normal[3][0] = x;
			normal[3][1] = y;
			normal[3][2] = z;

			vertex[3][0] = x * fRadius;
			vertex[3][1] = y * fRadius;
			vertex[3][2] = z * fRadius;

			//sphereMesh.AddTriangle
			sphereMesh_AddTriangle(vertex, normal, texture);


			// Rearrange for next triangle
			vertex[0][0] = vertex[1][0];
			vertex[0][1] = vertex[1][1];
			vertex[0][2] = vertex[1][2];

			normal[0][0] = normal[1][0];
			normal[0][1] = normal[1][1];
			normal[0][2] = normal[1][2];

			texture[0][0] = texture[1][0];
			texture[0][1] = texture[1][1];


			vertex[1][0] = vertex[3][0];
			vertex[1][1] = vertex[3][1];
			vertex[1][2] = vertex[3][2];

			normal[1][0] = normal[3][0];
			normal[1][1] = normal[3][1];
			normal[1][2] = normal[3][2];

			texture[1][0] = texture[3][0];
			texture[1][1] = texture[3][1];

			//sphereMesh.addTriangle
			sphereMesh_AddTriangle(vertex, normal, texture);

		}

		t -= dt;
		//release memory.
	}

	//sphereMesh.prepareToDraw();
	sphereMesh_prepareToDraw();
	//
}
