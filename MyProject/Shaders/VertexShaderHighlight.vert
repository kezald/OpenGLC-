#version 330 core

uniform mat4 MVPmatrix;

layout(location = 0) in vec3 vertexPosition;

void main()
{
	gl_Position = MVPmatrix * vec4(vertexPosition, 1.0f);
}