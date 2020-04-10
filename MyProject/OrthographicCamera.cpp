#include "OrthographicCamera.h"



OrthographicCamera::OrthographicCamera(
	float left, float right,
	float top, float bottom,
	float near, float far)
	: Camera(glm::ortho(left, right, bottom, top, near, far))
	, m_left{ left }
	, m_right{ right }
	, m_top{ top }
	, m_bottom{ bottom }
{
	m_nearValue = near;
	m_farValue = far;
}

OrthographicCamera::OrthographicCamera(
	float width, float height,
	float near, float far)
	: OrthographicCamera(
		-width / 2.0f, 
		width / 2.0f,
		height / 2.0f, 
		-height / 2.0f, 
		near, far)
{

}

OrthographicCamera::~OrthographicCamera()
{
}
