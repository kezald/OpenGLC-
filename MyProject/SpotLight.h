#pragma once

#include "Light.h"

#include <glm\glm.hpp>

class SpotLight : public Light
{
public:
	SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	~SpotLight();

	void Apply() override;
	void BindShadowMap() override;

	void SetAttenuation(float cons, float linear, float quadratic) override;
	void SetCutoffAngleInner(float angle);
	void SetCutoffAngleOuter(float angle);

	float GetCutoffAngle();

private:
	float m_cutoffAngleInner;
	float m_cutoffAngleOuter;
};


