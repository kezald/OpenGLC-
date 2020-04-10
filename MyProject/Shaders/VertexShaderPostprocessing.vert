#version 330 core

layout(location = 0) in vec2 VertexPosition;
layout(location = 2) in vec2 VertexUV;

out vec2 FragmentUV;

void main() 
{
	FragmentUV = VertexUV;
	gl_Position = vec4(VertexPosition, 0.0f, 1.0f);
}