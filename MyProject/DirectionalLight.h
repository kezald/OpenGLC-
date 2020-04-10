#pragma once

#include "Light.h"
#include "Model.h"
#include "Renderer.h"

#include <vector>

class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 diffuse, glm::vec3 specular);
	~DirectionalLight();

	void Apply() override;
	void BindShadowMap() override;

	glm::vec3 GetPosition() override;

private:

};

