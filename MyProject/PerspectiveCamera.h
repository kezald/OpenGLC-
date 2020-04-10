#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fov, float aspect, float near, float far);
	~PerspectiveCamera();

private:
	float m_FoV;
};

