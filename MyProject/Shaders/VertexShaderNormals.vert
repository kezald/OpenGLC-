#version 330 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 3) in vec3 VertexNormal;

out VS_OUT {
	vec3 Normal;
};

void main() {
	Normal = VertexNormal;
	gl_Position = vec4(VertexPosition, 1.0f);
}