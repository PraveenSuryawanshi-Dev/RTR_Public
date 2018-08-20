/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/
#pragma once

#ifndef _GL_OBJECT_H_
#include<stdio.h>
#include <gl\glew.h>
//#include<gl/GL.h>

enum
{
	VDG_ATTRIBUTE_POSITION = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};


#define GL_SAFE_DELETE_BUFFERS(x)if(x){glDeleteBuffers(1, &x);x=0;}
#define GL_SAFE_DELETE_ARRAYS(x)if(x){glDeleteVertexArrays(1, &x);x=0;}


class GLObject
{
public:

	GLObject();

	void initialize_for_draw_array_from_cpu(int vertices_size, GLfloat *vertices_data,int tex_cord_size , GLfloat *tex_cords_data, int normal_size, GLfloat *normals_data);
	void draw_using_draw_array(int count);

	void initialize_for_draw_elements_from_cpu(int vertices_size, GLfloat *vertices_data, int tex_cord_size, GLfloat *tex_cords_data, int normal_size, GLfloat *normals_data, int elements_size, unsigned int *elements_data);
	void initialize_for_draw_elements_from_gpu(int vertices_size, GLfloat *vertices_data, int tex_cord_size, GLfloat *tex_cords_data, int normal_size, GLfloat *normals_data, int elements_size, unsigned int *elements_data);
	void draw_using_draw_elements(int count);

	GLuint get_vao_object();

	GLuint get_vbo_object_positions();

	GLuint get_vbo_object_colors();

	GLuint get_vbo_object_tex_cords();

	GLuint get_vbo_object_normals();

	GLuint get_vbo_object_elements();

	~GLObject();

private:

	GLuint _vao_object;
	
	GLuint _vbo_object_positions;
	
	GLuint _vbo_object_colors;

	GLuint _vbo_object_tex_cords;
	
	GLuint _vbo_object_normals;
	
	GLuint _vbo_object_elements;

};

#endif // !_GL_OBJECT_H_

