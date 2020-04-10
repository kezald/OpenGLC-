#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	~PointLight();

	void Apply() override;

	void SetAttenuation(float cons, float linear, float quadratic) override;
};

