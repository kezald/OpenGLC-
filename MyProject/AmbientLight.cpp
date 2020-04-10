#include "AmbientLight.h"



AmbientLight::AmbientLight(glm::vec3 ambient)
	: Light(ambient, ambient, ambient, 1.0f, LightType::Ambient)
{
}


AmbientLight::~AmbientLight()
{
}

void AmbientLight::Apply() {
	Light::Apply();

	GLboolean isAmbientLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isAmbient");
	glUniform1i(isAmbientLoc, true);
}