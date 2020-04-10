#include "SpotLight.h"
#include "PerspectiveCamera.h"

#include <glm\gtc\type_ptr.hpp>
#include <memory>

SpotLight::SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	: Light(ambient, diffuse, specular, 1.0f, LightType::Spot)
	, m_cutoffAngleInner(glm::radians(45.0f))
	, m_cutoffAngleOuter(glm::radians(60.0f))
{
	m_pShadowCamera = std::make_unique<PerspectiveCamera>(120.0f, 1.0f, 0.1f, 1000.0f);
	m_isShadowEnabled = true;
}


SpotLight::~SpotLight()
{
}

void SpotLight::Apply() {
	Light::Apply();

	GLint isSpotLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isSpot");
	glUniform1i(isSpotLoc, true);

	GLint directionViewSpaceLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".directionViewSpace");
	glm::vec3 directionViewSpace = glm::vec3(sm_pCamera->GetViewMatrix() * glm::vec4(GetDirection(), 0.0f));
	glUniform3fv(directionViewSpaceLoc, 1, glm::value_ptr(directionViewSpace));

	GLint cutoffAngleInnerLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".cutoffAngleCosineInner");
	glUniform1f(cutoffAngleInnerLoc, glm::cos(m_cutoffAngleInner));
	GLint cutoffAngleOuterLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".cutoffAngleCosineOuter");
	glUniform1f(cutoffAngleOuterLoc, glm::cos(m_cutoffAngleOuter));

	GLint lightPosLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".positionViewSpace");
	glm::vec3 lightPosViewSpace = glm::vec3(sm_pCamera->GetViewMatrix() * glm::vec4(GetPosition(), 1.0f));
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosViewSpace));

	GLboolean isLocal = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isLocal");
	glUniform1i(isLocal, true);

	m_pShadowCamera->SetPosition(GetPosition());
	m_pShadowCamera->SetLookAt(GetDirection());
}

void SpotLight::BindShadowMap() {
	Light::BindShadowMap();
}

void SpotLight::SetCutoffAngleInner(float angle) {
	m_cutoffAngleInner = angle;
}

void SpotLight::SetCutoffAngleOuter(float angle) {
	m_cutoffAngleOuter = angle;
}

float SpotLight::GetCutoffAngle() {
	return m_cutoffAngleInner;
}

void SpotLight::SetAttenuation(float cons, float linear, float quadratic) {
	Light::SetAttenuation(cons, linear, quadratic);
}