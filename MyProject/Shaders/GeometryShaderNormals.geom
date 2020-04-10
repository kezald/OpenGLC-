#version 330 core

uniform mat4 MVPmatrix;

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
	vec3 Normal;
} gs_in[];

const float normLength = 0.2f;

void main() {
	vec3 averagePos = vec3(0.0f);
	vec3 averageNorm = vec3(0.0f);

	for (int i = 0; i < 3; i++) {
		averagePos += gl_in[i].gl_Position.xyz;
		vec3 norm = normalize(gs_in[i].Normal);
		averageNorm += norm;
	}
	averagePos /= 3.0f;
	averageNorm /= 3.0f;
	
	//gl_Position = MVPmatrix * vec4(averagePos, 1.0f);
	//EmitVertex();
	//gl_Position = MVPmatrix * vec4(averagePos + normLength * averageNorm, 1.0f);	
	//EmitVertex();
	
	for (int i = 0; i < 3; i++) {
		vec3 pos = gl_in[i].gl_Position.xyz;
		vec3 norm = normalize(gs_in[i].Normal);
		gl_Position = MVPmatrix * vec4(pos, 1.0f);
		EmitVertex();
		gl_Position = MVPmatrix * vec4(pos + normLength * norm, 1.0f);	
		EmitVertex();
	}
	
	EndPrimitive();
}  