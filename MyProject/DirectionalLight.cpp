#include "DirectionalLight.h"
#include "OrthographicCamera.h"
#include "LogManager.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

DirectionalLight::DirectionalLight(glm::vec3 diffuse, glm::vec3 specular)
	: Light(glm::vec3(0.0f, 0.0f, 0.0f), diffuse, specular,
		1.0f, LightType::Directional)
{
	m_pShadowCamera = std::make_unique<OrthographicCamera>(200.0f, 200.0f, 0.1f, 1000.0f);
	m_isShadowEnabled = true;
}


DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Apply() {
	Light::Apply();

	GLint directionViewSpaceLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".directionViewSpace");
	glm::vec3 directionViewSpace = glm::vec3(sm_pCamera->GetViewMatrix() * glm::vec4(GetDirection(), 0.0f));
	glUniform3fv(directionViewSpaceLoc, 1, glm::value_ptr(directionViewSpace));

	GLboolean isLocal = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isLocal");
	glUniform1i(isLocal, false);

	m_pShadowCamera->SetPosition(glm::normalize(GetDirection()) *
		-(m_pShadowCamera->GetFarValue() + m_pShadowCamera->GetNearValue()) / 2.0f);
	m_pShadowCamera->SetLookAt(GetDirection());
	m_pShadowCamera->Update();
}

void DirectionalLight::BindShadowMap() {
	Light::BindShadowMap();
}

glm::vec3 DirectionalLight::GetPosition() {
	return glm::normalize(GetDirection()) *
		-(m_pShadowCamera->GetFarValue() + m_pShadowCamera->GetNearValue()) / 2.0f;
}



