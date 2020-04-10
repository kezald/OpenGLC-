#version 330 core

uniform mat4 Pmatrix;
uniform float time;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 FragmentNormal;
	vec3 FragmentPosition;
	vec3 FragmentPositionM;
	vec3 FragmentPositionMV;
	vec2 FragmentUV;
	vec3 FragmentNormalM;
	vec3 FragmentNormalMV;
	vec4 FragmentShadowCoord;
} gs_in[];

out GS_OUT {
	vec3 FragmentPosition;
	vec3 FragmentPositionM;
	vec3 FragmentPositionMV;
	vec2 FragmentUV;
	vec3 FragmentNormalM;
	vec3 FragmentNormalMV;
	vec4 FragmentShadowCoord;
};

void copyAttribs(int index) {
	FragmentPosition = gs_in[index].FragmentPosition;
	FragmentPositionM = gs_in[index].FragmentPositionM;
	FragmentPositionMV = gs_in[index].FragmentPositionMV;
	FragmentUV = gs_in[index].FragmentUV;
	FragmentNormalM = gs_in[index].FragmentNormalM;
	FragmentNormalMV = gs_in[index].FragmentNormalMV;
	FragmentShadowCoord = gs_in[index].FragmentShadowCoord;
}

void main() {    
	for (int i = 0; i < 3; i++) {
		vec3 norm = 0.0f * normalize(gs_in[i].FragmentNormalMV);
		//vec3 norm = 0.5f * (sin(0.001f * time) + 1.0f) * normalize(gs_in[i].FragmentNormalMV);	
		
		gl_Position = Pmatrix * vec4(gs_in[i].FragmentPositionMV + norm, 1.0f);
		copyAttribs(i);
		EmitVertex();
	}
	
	EndPrimitive();
}  