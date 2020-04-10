#include "PointLight.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight(
	glm::vec3 ambient, glm::vec3 diffuse,
	glm::vec3 specular)
	: Light(ambient, diffuse, specular, 1.0f, LightType::Point)
{

}


PointLight::~PointLight()
{
}

void PointLight::Apply() {
	Light::Apply();

	GLint lightPosLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".positionViewSpace");
	glm::vec3 lightPosViewSpace = glm::vec3(sm_pCamera->GetViewMatrix() * glm::vec4(GetPosition(), 1.0f));
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPosViewSpace));

	GLboolean isLocal = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isLocal");
	glUniform1i(isLocal, true);
}

void PointLight::SetAttenuation(float cons, float linear, float quadratic) {
	Light::SetAttenuation(cons, linear, quadratic);
}