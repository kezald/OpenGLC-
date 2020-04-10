#pragma once
#include <glm\glm.hpp>
#include <gl\glew.h>

class Line
{
public:
	Line();
	~Line();

	void UpdateBuffer();
	void RenderLighting();

public:
	GLfloat m_lineWidth;
	glm::vec3 m_pointStart;
	glm::vec3 m_pointEnd;
	glm::vec3 m_color;

private:
	GLuint m_vao;
	GLuint m_vbo;
};

