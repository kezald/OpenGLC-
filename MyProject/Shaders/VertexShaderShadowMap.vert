#version 330 core

uniform mat4 MVPmatrix;

layout(location = 0) in vec3 VertexPosition;
layout(location = 2) in vec2 VertexUV;

out vec2 FragmentUV;

void main() {
	FragmentUV = VertexUV;
	gl_Position = MVPmatrix * vec4(VertexPosition, 1.0f);
}