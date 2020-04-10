#version 330 core

uniform mat4 MVPmatrix;
uniform mat4 MVmatrix;
uniform mat4 Mmatrix;
uniform mat4 NormalMatrixM;
uniform mat4 NormalMatrixMV;

layout(location = 0) in vec3 VertexPosition;
layout(location = 2) in vec2 VertexUV;
layout(location = 3) in vec3 VertexNormal;

out VS_OUT {
	vec3 FragmentNormal;
	vec3 FragmentPosition;
	vec3 FragmentPositionM;
	vec3 FragmentPositionMV;
	vec2 FragmentUV;
	vec3 FragmentNormalM;
	vec3 FragmentNormalMV;
	vec4 FragmentShadowCoord;
};

void main()
{
	FragmentNormal = VertexNormal;
	FragmentPosition = VertexPosition;
	FragmentPositionM = vec3(Mmatrix * vec4(VertexPosition, 1.0f));
	FragmentPositionMV = vec3(MVmatrix * vec4(VertexPosition, 1.0f));
	FragmentUV = VertexUV;
	FragmentNormalM = mat3(NormalMatrixM) * VertexNormal;
	FragmentNormalMV = mat3(NormalMatrixMV) * VertexNormal;
	FragmentShadowCoord = Mmatrix * vec4(VertexPosition, 1.0f);

	gl_Position = MVPmatrix * vec4(VertexPosition, 1.0f);
}
