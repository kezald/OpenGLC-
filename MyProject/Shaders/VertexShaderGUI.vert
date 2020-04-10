#version 330 core

uniform mat4 Pmatrix;

layout(location = 0) in vec3 vertexPosition;

void main() 
{
	gl_Position = Pmatrix * vec4(vertexPosition, 1.0f);
}