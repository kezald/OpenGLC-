#pragma once

#include "Camera.h"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(float left, float right,
		float top, float bottom,
		float near, float far);
	OrthographicCamera(float width, float height, float near, float far);
	~OrthographicCamera();

private:
	float m_left, m_right;
	float m_top, m_bottom;
};

