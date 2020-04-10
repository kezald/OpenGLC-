#version 330 core

uniform sampler2D depthMap;

in vec2 FragmentUV;

out vec4 FragColor;

float LinearizeDepth(in vec2 uv) {
	float near = 0.1f;
	float far = 1000.0f;
	float z = texture(depthMap, uv).x * 2.0 - 1.0; 
	float linearDepthPerspective = (2.0 * near * far) / (far + near - z * (far - near));
	return clamp((linearDepthPerspective - near)/(far - near), 0.0f, 1.0f);
}

void main() {
	float c = LinearizeDepth(FragmentUV);
	//FragColor = vec4(c, c, c, 1.0f);
	
	//FragColor = vec4(vec3(texture(depthMap, FragmentUV).x), 1.0f);
	FragColor = texture(depthMap, FragmentUV);
}