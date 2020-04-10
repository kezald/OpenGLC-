#version 330 core

struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	bool isAmbient;
	bool isHemisphere;
	
	vec3 positionViewSpace;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	
	vec3 directionViewSpace;
	float cutoffAngleCosineInner;
	float cutoffAngleCosineOuter;
	
	//vec3 halfVector;  //Computed in application
	
	vec3 skyColorDiffuse;
	vec3 groundColorDiffuse;
	
	bool isShadowEnabled;
	mat4 scaleBiasPVLightMatrix;
	sampler2DShadow depthMap;
};

struct MaterialProperties {
	bool isReflectionMapped; //Reflection mapped
	bool isRefractionMapped; // Refraction mapped

	bool isGetAlphaFromDiffuseMap;

	bool isDiffuseMapPresent;
	sampler2D diffuseMap;
	bool isSpecularMapPresent;
	sampler2D specularMap;
	bool isCubeMapPresent;
	samplerCube cubeMap;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform samplerCube cubeMapEnvironmentMapping;
uniform vec3 CameraPositionWorldSpace;

const int MaxLights = 10;
uniform LightProperties Lights[MaxLights];
uniform MaterialProperties Material;

in GS_OUT {
	vec3 FragmentPosition;
	vec3 FragmentPositionM;
	vec3 FragmentPositionMV;
	vec2 FragmentUV;
	vec3 FragmentNormalM;
	vec3 FragmentNormalMV;
	vec4 FragmentShadowCoord;
};

out vec4 FragColor;
 
void main()
{
	if (Material.isCubeMapPresent) {
		FragColor = vec4(min(2.0f * texture(Material.cubeMap, FragmentPosition).rgb, vec3(1.0f)), 1.0f);
		return;
	}

	vec3 ambientAccum = vec3(0.0f);
	vec3 diffuseAccum = vec3(0.0f);
	vec3 specularAccum = vec3(0.0f);
	
	for (int i = 0; i < Lights.length(); i++) {
		if (Lights[i].isEnabled) {
			vec3 cameraPos = vec3(0.0f, 0.0f, 0.0f);
			vec3 viewDir = normalize(cameraPos - FragmentPositionMV);
			vec3 normMV = normalize(FragmentNormalMV);
		
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			
			if (Lights[i].isAmbient) {
				// No diffuse or specular with ambient light
				ambient = Lights[i].ambient;
				diffuse = vec3(0.0f);
				specular = vec3(0.0f);
			} else if(Lights[i].isHemisphere){
				ambient = vec3(0.0f);
				specular = vec3(0.0f);
				
				vec3 lightDir = normalize(Lights[i].positionViewSpace - FragmentPositionMV);
				float costheta = dot(lightDir, normMV);
				float a = costheta * 0.5 + 0.5;
				diffuse = mix(Lights[i].groundColorDiffuse, Lights[i].skyColorDiffuse, a);
			} else {
				ambient = Lights[i].ambient;
				diffuse = Lights[i].diffuse;
				specular = Lights[i].specular;
			
				vec3 lightDir;
						
				if (Lights[i].isLocal) {
					lightDir = normalize(Lights[i].positionViewSpace - FragmentPositionMV);
				} else {
					lightDir = normalize(-Lights[i].directionViewSpace);
				}
				
				//For Specular
				vec3 reflectDir = reflect(-lightDir, normMV);
				float cosineReflectCameraDir = max(dot(reflectDir, viewDir), 0.0f);
				
				//float cosineReflectCameraDir = max(dot(Lights[i].halfVector, normMV), 0.0f); // halfVector optimisation
				
				//Diffuse and Specular Assignments
				if (Lights[i].isSpot) {
					float thetaCosine = dot(normalize(-Lights[i].directionViewSpace), lightDir);
					
					float spotIntensity = (-thetaCosine + Lights[i].cutoffAngleCosineOuter) / 
						(-Lights[i].cutoffAngleCosineInner + Lights[i].cutoffAngleCosineOuter);
					spotIntensity = clamp(spotIntensity, 0.0f, 1.0f);
						
					diffuse *= spotIntensity * max(dot(lightDir, normMV), 0.0f);
					specular *= spotIntensity * pow(cosineReflectCameraDir, Material.shininess);
				} else {
					diffuse *= max(dot(lightDir, normMV), 0.0f);
					specular *= pow(cosineReflectCameraDir, Material.shininess);
				}
			}
			
			if (Lights[i].isLocal) {
					float lightDistance = length(Lights[i].positionViewSpace - FragmentPositionMV);
					float attenuation = 1/(Lights[i].constantAttenuation 
					+ Lights[i].linearAttenuation * lightDistance 
					+ Lights[i].quadraticAttenuation * lightDistance * lightDistance);
					ambient *= attenuation;
					diffuse *= attenuation;
					specular *= attenuation; // Optional
			}
			
			if (Lights[i].isShadowEnabled) {
				vec4 shadowCoord = Lights[i].scaleBiasPVLightMatrix * (FragmentShadowCoord);
				shadowCoord /= shadowCoord.w;
				
				if (shadowCoord.x >= 0.0f && shadowCoord.x <= 1.0f && 
					shadowCoord.y >= 0.0f && shadowCoord.y <= 1.0f) {
						
					float shadowFactor = textureProj(Lights[i].depthMap, shadowCoord);
					diffuse *= shadowFactor;
					specular *= shadowFactor;
				}
			}
			
			ambientAccum += ambient;
			diffuseAccum += diffuse;
			specularAccum += specular;
		}
	}
	
	float alpha = 1.0f;
	
	if (Material.isReflectionMapped) {
		vec3 viewDir = normalize(CameraPositionWorldSpace - FragmentPositionM);
		vec3 norm = normalize(FragmentNormalM);
	
		vec3 reflection = reflect(-viewDir, norm);
		vec3 envMapColor = min(2.0f*texture(cubeMapEnvironmentMapping, reflection).rgb, vec3(1.0f));
		
		ambientAccum *= envMapColor;
		diffuseAccum *= envMapColor;
		specularAccum *= envMapColor;
	} else if (Material.isRefractionMapped) {
		vec3 viewDir = normalize(CameraPositionWorldSpace - FragmentPositionM);
		vec3 norm = normalize(FragmentNormalM);
	
		float ratio = 1.00 / 1.52;  //Air - Glass refraction
		vec3 refraction = refract(-viewDir, norm, ratio);
		vec3 envMapColor = min(2.0f*texture(cubeMapEnvironmentMapping, refraction).rgb, vec3(1.0f));
		
		ambientAccum = envMapColor;
		diffuseAccum = vec3(0.0f);
		specularAccum = vec3(0.0f);
	} else if (Material.isCubeMapPresent) {
		//vec4 cubeMapColor = texture(Material.cubeMap, FragmentPosition);
		//ambientAccum = cubeMapColor.rgb;
		//diffuseAccum = vec3(0.0f);
		//specularAccum = vec3(0.0f);
	} else {
		// Diffuse Map Sampling
		if (Material.isDiffuseMapPresent) {
			vec4 diffuseMapColor = texture(Material.diffuseMap, FragmentUV);
			
			if (Material.isGetAlphaFromDiffuseMap) {
				alpha = diffuseMapColor.a;
			}
			
			if (alpha <= 0.0f) {
				discard;
			}
			
			ambientAccum *= diffuseMapColor.rgb;
			diffuseAccum *= diffuseMapColor.rgb;
		} else {
			ambientAccum *= Material.ambient;
			diffuseAccum *= Material.diffuse;
		}
		
		// Specular Map Sampling
		if (Material.isSpecularMapPresent) {
			vec3 specularMapColor = texture(Material.specularMap, FragmentUV).rgb;
			specularAccum *= specularMapColor;
		} else {
			specularAccum *= Material.specular;
		}
	}
	
	vec3 finalObjColor = min((ambientAccum + diffuseAccum + specularAccum), vec3(1.0f));
	FragColor = vec4(finalObjColor, alpha);
	
	float near = 0.1f;
	float far = 1000.0f;
	float z = gl_FragCoord.z * 2.0 - 1.0; 
	float linearDepthPerspective = (2.0 * near * far) / (far + near - z * (far - near));
	linearDepthPerspective = clamp((linearDepthPerspective - near)/(far - near), 0.0f, 1.0f);
	//FragColor = vec4(vec3(linearDepthPerspective), 1.0f) - vec4(finalObjColor * vec3(0.00000001f), 0.0f);
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0f) - vec4(finalObjColor * vec3(0.00000001f), 0.0f);
}
