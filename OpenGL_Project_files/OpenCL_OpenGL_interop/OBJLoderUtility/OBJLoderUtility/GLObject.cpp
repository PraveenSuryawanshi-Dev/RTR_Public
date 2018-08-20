/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/
#include "GLObject.h"

GLObject::GLObject()
{
	

}

void GLObject::initialize_for_draw_array_from_cpu(int vertices_size, GLfloat * vertices_data, int tex_cord_size, GLfloat * tex_cords_data, int normal_size, GLfloat * normals_data)
{
	/*Rectangle*/
	glGenVertexArrays(1, &_vao_object);
	glBindVertexArray(_vao_object);

	/*position*/
	glGenBuffers(1, &_vbo_object_positions);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_positions);

	glBufferData(GL_ARRAY_BUFFER,   /**/vertices_size/**/ * sizeof(GL_FLOAT), vertices_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*texture*/
	glGenBuffers(1, &_vbo_object_tex_cords);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_tex_cords);

	glBufferData(GL_ARRAY_BUFFER,  /**/tex_cord_size/**/ * sizeof(GL_FLOAT), tex_cords_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*normals*/
	glGenBuffers(1, &_vbo_object_normals);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_normals);

	glBufferData(GL_ARRAY_BUFFER,  /**/normal_size/**/ * sizeof(GL_FLOAT), normals_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/

	glBindVertexArray(0);
	/**/
}

void GLObject::draw_using_draw_array(int count)
{
	glBindVertexArray(_vao_object);
	glDrawArrays(GL_TRIANGLES, 0, count);
	glBindVertexArray(0);
}

void GLObject::initialize_for_draw_elements_from_cpu(int vertices_size, GLfloat * vertices_data, int tex_cord_size, GLfloat * tex_cords_data, int normal_size, GLfloat * normals_data, int elements_size,unsigned int * elements_data)
{
	/*Rectangle*/
	glGenVertexArrays(1, &_vao_object);
	glBindVertexArray(_vao_object);

	/*position*/
	glGenBuffers(1, &_vbo_object_positions);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_positions);

	glBufferData(GL_ARRAY_BUFFER,   /**/vertices_size/**/ * sizeof(GL_FLOAT), vertices_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*texture*/
	glGenBuffers(1, &_vbo_object_tex_cords);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_tex_cords);

	glBufferData(GL_ARRAY_BUFFER,  /**/tex_cord_size/**/ * sizeof(GL_FLOAT), tex_cords_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*normals*/
	glGenBuffers(1, &_vbo_object_normals);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_normals);

	glBufferData(GL_ARRAY_BUFFER,  /**/normal_size/**/ * sizeof(GL_FLOAT), normals_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*ELEMENT VBO*/
	glGenBuffers(1, &_vbo_object_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_object_elements);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, /**/elements_size/**/ * sizeof(int), elements_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	/**/

	glBindVertexArray(0);
	/**/
}

void GLObject::initialize_for_draw_elements_from_gpu(int vertices_size, GLfloat * vertices_data, int tex_cord_size, GLfloat * tex_cords_data, int normal_size, GLfloat * normals_data, int elements_size, unsigned int * elements_data)
{
	/*Rectangle*/
	glGenVertexArrays(1, &_vao_object);
	glBindVertexArray(_vao_object);

	/*position*/
	glGenBuffers(1, &_vbo_object_positions);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_positions);

	glBufferData(GL_ARRAY_BUFFER,   /**/vertices_size/**/ * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*texture*/
	glGenBuffers(1, &_vbo_object_tex_cords);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_tex_cords);

	glBufferData(GL_ARRAY_BUFFER,  /**/tex_cord_size/**/ * sizeof(GL_FLOAT), tex_cords_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*normals*/
	glGenBuffers(1, &_vbo_object_normals);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_object_normals);

	glBufferData(GL_ARRAY_BUFFER,  /**/normal_size/**/ * sizeof(GL_FLOAT), normals_data, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*ELEMENT VBO*/
	glGenBuffers(1, &_vbo_object_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_object_elements);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, /**/elements_size/**/ * sizeof(int), elements_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	/**/

	glBindVertexArray(0);
	/**/
}

void GLObject::draw_using_draw_elements(int count)
{
	glBindVertexArray(_vao_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_object_elements);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

GLuint GLObject::get_vao_object()
{
	return _vao_object;
}

GLuint GLObject::get_vbo_object_positions()
{
	return _vbo_object_positions;
}

GLuint GLObject::get_vbo_object_colors()
{
	return _vbo_object_colors;
}

GLuint GLObject::get_vbo_object_tex_cords()
{
	return _vbo_object_tex_cords;
}

GLuint GLObject::get_vbo_object_normals()
{
	return _vbo_object_normals;
}

GLuint GLObject::get_vbo_object_elements()
{
	return _vbo_object_elements;
}

GLObject::~GLObject()
{
	GL_SAFE_DELETE_ARRAYS(_vao_object)
	GL_SAFE_DELETE_BUFFERS(_vbo_object_positions)
	GL_SAFE_DELETE_BUFFERS(_vbo_object_colors)
	GL_SAFE_DELETE_BUFFERS(_vbo_object_tex_cords)
	GL_SAFE_DELETE_BUFFERS(_vbo_object_normals)
	GL_SAFE_DELETE_BUFFERS(_vbo_object_elements)
}
