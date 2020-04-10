#include "Line.h"



Line::Line()
	: m_vao{ 0 }
	, m_vbo{ 0 }
	, m_pointStart{ 0.0f, 0.0f, 0.0f }
	, m_pointEnd{ 0.0f, 0.0f, 0.0f }
	, m_lineWidth{ 1.0f }
	, m_color{ 1.0f, 1.0f ,1.0f }
{

}


Line::~Line()
{
}

void Line::UpdateBuffer() {
	GLfloat data[6] = {
		m_pointStart[0], m_pointStart[1], m_pointStart[2],
		m_pointEnd[0], m_pointEnd[1], m_pointEnd[2]
	};

	if (m_vao == 0) {
		glGenVertexArrays(1, &m_vao);
	}

	if (m_vbo == 0) {
		glGenBuffers(1, &m_vbo);
	}

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::RenderLighting() {
	glLineWidth(m_lineWidth);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}
