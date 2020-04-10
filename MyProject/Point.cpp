#include "Point.h"



Point::Point()
	: m_vao{ 0 }
	, m_vbo{ 0 }
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_color{ 1.0f, 1.0f ,1.0f }
	, m_size{ 1.0f }
{

}

void Point::UpdateBuffer() {
	GLfloat data[3] = { m_position[0], m_position[1], m_position[2] };

	if (m_vao == 0) {
		glGenVertexArrays(1, &m_vao);
	}

	if (m_vbo == 0) {
		glGenBuffers(1, &m_vbo);
	}

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Point::RenderLighting() {
	glPointSize(m_size);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}