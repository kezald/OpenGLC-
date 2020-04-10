#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>

struct Position {
	GLfloat x, y, z;
};

struct Color {
	GLubyte r, g, b, a;
};

struct UV {
	GLfloat u, v;
};


struct Vertex3D {
	Position coord;
	Color color;
	UV uv;
	//float pad[2]; //Add padding to make size of struct be a multiple of 2

	void SetCoord(GLfloat x, GLfloat y, GLfloat z) {
		coord.x = x;
		coord.y = y;
		coord.z = z;
	}

	void SetCoord(glm::vec3&& coords) {
		coord.x = coords.x;
		coord.y = coords.y;
		coord.z = coords.z;
	}

	void SetCoord(glm::vec2&& coords) {
		coord.x = coords.x;
		coord.y = coords.y;
	}

	void SetColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	void SetColor(glm::u8vec4& colors) {
		color.r = colors.r;
		color.g = colors.g;
		color.b = colors.b;
		color.a = colors.a;
	}

	void SetColor(glm::vec4& colors) {
		color.r = static_cast<GLubyte>(colors.r * 255);
		color.g = static_cast<GLubyte>(colors.g * 255);
		color.b = static_cast<GLubyte>(colors.b * 255);
		color.a = static_cast<GLubyte>(colors.a * 255);
	}

	void SetUV(GLfloat u, GLfloat v) {
		uv.u = u;
		uv.v = v;
	}

	void SetUV(glm::vec2 UVs) {
		uv.u = UVs[0];
		uv.v = UVs[1];
	}
};