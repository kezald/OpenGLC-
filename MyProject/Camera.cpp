#include <glm\glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "camera.h"


Camera::Camera(glm::mat4 projectionMatrix)
	: START_AT{ 0, 0, -1 }
	, START_UP{ 0, 1, 0 }
	, WORLD_UP_VECTOR{ 0.0f, 1.0f, 0.0f }
	, m_projectionMatrix{ projectionMatrix }
	, m_pitchAngleDeg{ 0.0f }
	, m_yawAngleDeg{ 0.0f }
{
	m_lookAt = START_AT;

	glm::vec3 sideVec = glm::cross(m_lookAt, WORLD_UP_VECTOR);
	m_up = glm::cross(sideVec, m_lookAt);
}

void Camera::Update() {
	m_lookAtPoint[0] = m_eye[0] + m_lookAt[0];
	m_lookAtPoint[1] = m_eye[1] + m_lookAt[1];
	m_lookAtPoint[2] = m_eye[2] + m_lookAt[2];

	m_viewMatrix = glm::lookAt(m_eye, m_lookAtPoint, m_up);

	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

void Camera::Move(float timeElapsed) {
	float spd = DEFAULT_MOVE_SPEED;
	spd += (m_isAccelerated) ? ACCELERATED_SPEED_INCREASE : 0;
	double distanceMoved = spd * timeElapsed;
	double degreesRotated = ROTATION_SPEED * timeElapsed;

	// Normalization prevents too small and too large vectors producing NaNs
	m_lookAt = glm::normalize(m_lookAt);
	m_up = glm::normalize(m_up);

	UpdateAnglesForXZMovement(degreesRotated);
	RotateWithRespectToXZ();
	MoveWithRespectToXZ(distanceMoved);
}

void Camera::ApplyMouseControl(float timeElapsed, const glm::vec3& NDCcoordPress, const glm::vec3& NDCcoordCurrent) {
	double dx = NDCcoordCurrent[0] - NDCcoordPress[0];
	double dy = NDCcoordCurrent[1] - NDCcoordPress[1];

	double speed = 180.0;

	m_yawAngleDeg += -dx * (speed * timeElapsed);
	m_pitchAngleDeg += dy * (speed * timeElapsed);
}

void Camera::SetPosition(glm::vec3 position) {
	m_eye = position;
}

glm::vec3 Camera::GetPosition() {
	return m_eye;
}

glm::vec3 Camera::GetLookAt() {
	assert(m_lookAt != glm::vec3(0.0f));
	return m_lookAt;
}

float Camera::GetNearValue() {
	return m_nearValue;
}

float Camera::GetFarValue() {
	return m_farValue;
}

void Camera::SetLookAt(glm::vec3 lookAt) {
	glm::vec3 lookAtXZ = glm::vec3(lookAt.x, 0.0f, lookAt.z);
	if (lookAtXZ != glm::vec3(0.0f)) {
		m_yawAngleDeg = glm::degrees(glm::orientedAngle(
			glm::normalize(START_AT), 
			glm::normalize(lookAtXZ),
			WORLD_UP_VECTOR));
	}

	m_pitchAngleDeg = glm::degrees(-glm::angle(glm::normalize(lookAt), glm::normalize(WORLD_UP_VECTOR)))
		+ 90.0f;
	Move(0.0f); //Update LookAt
	m_lookAt = glm::normalize(m_lookAt);
	Update();
}

void Camera::UpdateAnglesForXZMovement(float degreesRotated) {
	double dirSign = 1;

	// ============Update angles===============
	if (m_pitchDown || m_pitchUp) {
		dirSign = (m_pitchDown) ? -1 : 1;
		m_pitchAngleDeg += dirSign * degreesRotated;
	}

	if (m_yawRight || m_yawLeft) {
		dirSign = (m_yawRight) ? -1 : 1;
		m_yawAngleDeg += dirSign * degreesRotated;
	}

	// ============Ensure angles are valid==========
	if (m_pitchAngleDeg > PITCH_ANGLE_MAX_LIMITED) {
		m_pitchAngleDeg = PITCH_ANGLE_MAX_LIMITED;
	}
	else if (m_pitchAngleDeg < -PITCH_ANGLE_MAX_LIMITED) {
		m_pitchAngleDeg = -PITCH_ANGLE_MAX_LIMITED;
	}

	while (m_yawAngleDeg >= 360) {
		m_yawAngleDeg -= 360;
	}
	while (m_yawAngleDeg <= 360) {
		m_yawAngleDeg += 360;
	}
}

void Camera::RotateWithRespectToXZ() {
	// Pitch
	glm::vec3 startSideVec = glm::cross(START_AT, START_UP);
	glm::vec3 pitchAt = glm::rotate(START_AT, glm::radians(m_pitchAngleDeg), startSideVec);
	glm::vec3 m_pitchUp = glm::cross(startSideVec, pitchAt);

	// Yaw with respect to Y axis
	m_lookAt = glm::rotate(pitchAt, glm::radians(m_yawAngleDeg), WORLD_UP_VECTOR); //rotatedAt
	//m_up = glm::rotate(m_pitchUp, glm::radians(m_yawAngleDeg), WORLD_UP_VECTOR); //rotatedUp
}

void Camera::MoveWithRespectToXZ(float distanceMoved) {
	float dirSign = 1;

	if (m_moveForward || m_moveBackward) {
		dirSign = (m_moveBackward) ? -1 : 1;
		glm::mat4 displacementMat = glm::translate(glm::normalize(m_lookAt) * (dirSign * distanceMoved));
		m_eye = displacementMat * glm::vec4(m_eye, 1);
	}

	if (m_moveLeft || m_moveRight) {
		dirSign = (m_moveLeft) ? -1 : 1;

		glm::vec3 sideVec = glm::cross(m_lookAt, m_up);
		glm::mat4 displacementMat = glm::translate(glm::normalize(sideVec) * (dirSign * distanceMoved));
		m_eye = displacementMat * glm::vec4(m_eye, 1);
	}

	if (m_moveDown || m_moveUp) {
		dirSign = (m_moveDown) ? -1 : 1;

		glm::mat4 displacementMat = glm::translate(glm::normalize(WORLD_UP_VECTOR) * (dirSign * distanceMoved));
		m_eye = displacementMat * glm::vec4(m_eye, 1);
	}
}

glm::mat4 Camera::GetProjectionMatrix() {
	return m_projectionMatrix;
}

glm::mat4 Camera::GetViewMatrix() {
	return m_viewMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() {
	return m_viewProjectionMatrix;
}

