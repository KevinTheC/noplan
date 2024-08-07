#include "VBO.h"

VBO::VBO(std::vector<GLfloat>* vec)
{
	vertices = vec;
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices->size()*sizeof(GLfloat), vertices->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
VBO::~VBO()
{
	delete(vertices);
}
void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VBO::erase()
{
	glDeleteBuffers(1, &ID);
}

void VBO::update() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), vertices->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
const std::vector<GLfloat>& VBO::getValues()
{
	return *(vertices);
}