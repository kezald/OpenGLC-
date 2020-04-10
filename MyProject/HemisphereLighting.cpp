#include "HemisphereLighting.h"

#include <glm\gtc\type_ptr.hpp>

HemisphereLighting::HemisphereLighting(glm::vec3 skyColorDiffuse, glm::vec3 groundColorDiffuse)
	: Light(m_skyColorDiffuse, 1.0f, LightType::Hemisphere)
	, m_skyColorDiffuse(skyColorDiffuse)
	, m_groundColorDiffuse(groundColorDiffuse)
{

}


HemisphereLighting::~HemisphereLighting()
{
}

void HemisphereLighting::Apply() {
	Light::Apply();

	GLint isHemisphereLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isHemisphere");
	glUniform1i(isHemisphereLoc, true);

	GLint skyColorLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".skyColorDiffuse");
	glUniform3fv(skyColorLoc, 1, glm::value_ptr(m_skyColorDiffuse));

	GLint groundColorLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".groundColorDiffuse");
	glUniform3fv(groundColorLoc, 1, glm::value_ptr(m_groundColorDiffuse));

	GLint lightPosLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".positionViewSpace");
	glm::vec3 lightPosViewSpace = glm::vec3(sm_pCamera->GetViewMatrix() * glm::vec4(GetPosition(), 1.0f));
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosViewSpace));

	GLboolean isLocal = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isLocal");
	glUniform1i(isLocal, true);
}

glm::vec3 HemisphereLighting::GetDominantColor() {
	return m_skyColorDiffuse;
}

void HemisphereLighting::SetAttenuation(float cons, float linear, float quadratic) {
	Light::SetAttenuation(cons, linear, quadratic);
}