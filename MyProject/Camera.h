#pragma once

#include <glm/gtx/transform.hpp>

class Camera {

public:
	void Update();
	void Move(float timeElapsed);
	void ApplyMouseControl(float timeElapsed, const glm::vec3& NDCcoordPress, const glm::vec3& NDCcoordCurrent);
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetViewProjectionMatrix();

	glm::vec3 GetPosition();
	glm::vec3 GetLookAt();
	float GetNearValue();
	float GetFarValue();

	void SetPosition(glm::vec3 position);
	void SetLookAt(glm::vec3 lookAt);

protected:
	Camera(glm::mat4 projectionMatrix);

private:
	void UpdateAnglesForXZMovement(float degreesRotated);
	void RotateWithRespectToXZ();
	void MoveWithRespectToXZ(float distanceMoved);

public:
	const float ROTATION_SPEED = 45.0f;
	const float DEFAULT_MOVE_SPEED = 10.0f;
	const float ACCELERATED_SPEED_INCREASE = 60.0f;
	const float PITCH_ANGLE_MAX_LIMITED = 89.0f; // With respect to XZ plane

	bool m_moveForward, m_moveBackward, m_moveLeft, m_moveRight, m_moveUp, m_moveDown,
		m_pitchDown, m_pitchUp, m_yawRight, m_yawLeft = false;
	bool m_isAccelerated = false;

protected:
	glm::mat4 m_projectionMatrix;
	float m_nearValue;
	float m_farValue;

private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_viewProjectionMatrix;

	const glm::vec3 WORLD_UP_VECTOR; // For XY plane movement
	const glm::vec3 START_AT; // For XY plane movement
	const glm::vec3 START_UP; // For XY plane movement
	glm::vec3 m_eye;
	glm::vec3 m_lookAt;
	glm::vec3 m_lookAtPoint;
	glm::vec3 m_up;

	float m_pitchAngleDeg, m_yawAngleDeg;
};