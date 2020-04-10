#pragma once

#include "GLSLProgram.h"
#include "Camera.h"

#include <glm\glm.hpp>
#include <string>
#include <memory>

enum class LightType { Ambient, Directional, Point, Spot, Hemisphere };

class Light
{
public:
	~Light();

	virtual void Apply();
	virtual void BindShadowMap();

	glm::vec3 GetDirection();
	virtual glm::vec3 GetPosition();
	std::string GetLightIndexString();
	LightType GetLightType();
	glm::vec3 GetSpecular();
	virtual glm::vec3 GetDominantColor();
	Camera* GetShadowCamera();
	GLuint GetDepthTextureID();
	bool GetIsShadowEnabled();

	void SetDirection(glm::vec3 direction);
	void SetPosition(glm::vec3 position);
	void SetLightIndex(int index);

	static void SetLightingProgram(GLSLProgram* pProgram);
	static void SetShadowMapProgram(GLSLProgram* pProgram);
	static void SetCamera(Camera* pCamera);

protected:
	Light(glm::vec3 color, float intensity, LightType type);
	Light(glm::vec3 ambient, glm::vec3 diffuse,
		glm::vec3 specular, float intensity, LightType type);
	virtual void SetAttenuation(float cons, float linear, float quadratic);

public:
	static const int DEPTH_TEXTURE_SIZE;

protected:
	static GLSLProgram* sm_pLightingProgram;
	static GLSLProgram* sm_pShadowMapProgram;
	static Camera* sm_pCamera;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	float m_intensity;
	bool m_isEnabled;
	LightType m_type;
	float m_constantAttenuation;
	float m_linearAttenuation;
	float m_quadraticAttenuation;

	int m_lightIndex;

	//========= Shadow Mapping Part ==========
	bool m_isShadowEnabled;
	std::unique_ptr<Camera> m_pShadowCamera;
	GLuint m_depthTextureID;
	GLuint m_fboShadowMap;
};


