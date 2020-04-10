#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>

class Point
{
public:
	Point();

	void UpdateBuffer();
	void RenderLighting();

public:
	glm::vec3 m_position;
	glm::vec3 m_color;
	float m_size;

private:
	GLuint m_vao;
	GLuint m_vbo;
};

