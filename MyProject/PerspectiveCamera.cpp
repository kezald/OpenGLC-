#include "PerspectiveCamera.h"



PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
	: Camera(glm::perspective(glm::radians(fov), aspect, near, far))
	, m_FoV{ fov }
{
	m_nearValue = near;
	m_farValue = far;
}


PerspectiveCamera::~PerspectiveCamera()
{
}
