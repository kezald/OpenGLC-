#pragma once

#include "Light.h"

#include <glm\glm.hpp>

class HemisphereLighting : public Light
{
public:
	HemisphereLighting(glm::vec3 skyColorDiffuse, glm::vec3 groundColorDiffuse);
	~HemisphereLighting();

	void Apply() override;

	glm::vec3 GetDominantColor() override;
	void SetAttenuation(float cons, float linear, float quadratic) override;

private:
	glm::vec3 m_skyColorDiffuse;
	glm::vec3 m_groundColorDiffuse;
};

