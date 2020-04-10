#include <limits>

#include "Keyboard.h"

Keyboard::Keyboard()
	: m_cameraRef{ nullptr }
{
	AddBlockKeys(SDLK_w, SDLK_s);
	AddBlockKeys(SDLK_a, SDLK_d);
	AddBlockKeys(SDLK_UP, SDLK_DOWN);
	AddBlockKeys(SDLK_LEFT, SDLK_RIGHT);
	AddBlockKeys(SDLK_x, SDLK_SPACE);
}


Keyboard::~Keyboard()
{
}


void Keyboard::ApplyKeys() {
	if (m_cameraRef != nullptr) {
		(*m_cameraRef)->m_isAccelerated = IsPressed(SDLK_z);
		(*m_cameraRef)->m_moveForward = IsTriggerValid(SDLK_w);
		(*m_cameraRef)->m_moveBackward = IsTriggerValid(SDLK_s);
		(*m_cameraRef)->m_moveLeft = IsTriggerValid(SDLK_a);
		(*m_cameraRef)->m_moveRight = IsTriggerValid(SDLK_d);
		(*m_cameraRef)->m_moveUp = IsTriggerValid(SDLK_SPACE);
		(*m_cameraRef)->m_moveDown = IsTriggerValid(SDLK_x);
		(*m_cameraRef)->m_pitchUp = IsTriggerValid(SDLK_UP);
		(*m_cameraRef)->m_pitchDown = IsTriggerValid(SDLK_DOWN);
		(*m_cameraRef)->m_yawLeft = IsTriggerValid(SDLK_LEFT);
		(*m_cameraRef)->m_yawRight = IsTriggerValid(SDLK_RIGHT);
	}
}

void Keyboard::HandleKeyPress(SDL_Keycode key) {
	float time = SDL_GetTicks();

	bool wasPressed = IsPressed(key);
	if (!wasPressed) {
		//auto mit = mapKeyLastTimePressed.insert(std::make_pair(key, time));
		//mit.first->second = time;
		m_mapKeyLastTimePressed[key] = time;
	}
	/*auto mit = mapKeyPressed.insert(std::make_pair(key, true));
	mit.first->second = true;*/
	m_mapKeyPressed[key] = true;

	/*switch (key) {
	case SDLK_UP:
	break;
	}*/
}

void Keyboard::HandleKeyRelease(SDL_Keycode key) {
	/*auto mit = mapKeyPressed.insert(std::make_pair(key, false));
	mit.first->second = false;*/
	m_mapKeyPressed[key] = false;
}

void Keyboard::SetCameraRef(Camera** cameraRef) {
	m_cameraRef = cameraRef;
}

bool Keyboard::IsTriggerValid(SDL_Keycode keyID) {
	if (!IsPressed(keyID)) {
		return false;
	}

	SDL_Keycode blockingKey = m_mapKeyBlocking.at(keyID);
	bool isBlockingKeyPressed = IsPressed(blockingKey);
	if (isBlockingKeyPressed) {
		// Can get directly from map since the key is there definitely (it has been
		// presssed at least once)
		float thisKeyTime = m_mapKeyLastTimePressed.at(keyID);
		float blockKeyTime = m_mapKeyLastTimePressed.at(blockingKey);
		return thisKeyTime < blockKeyTime;
	}
	else {
		return true;
	}
}

bool Keyboard::IsPressed(SDL_Keycode keyID) {
	auto mapIt = m_mapKeyPressed.find(keyID);
	bool found = mapIt != m_mapKeyPressed.end();

	return (!found) ? false : mapIt->second;
}

float Keyboard::GetLastTimePressed(SDL_Keycode keyID) {
	auto mapIt = m_mapKeyLastTimePressed.find(keyID);
	bool found = mapIt != m_mapKeyLastTimePressed.end();

	return (found) ? mapIt->second : std::numeric_limits<float>::max();
}

void Keyboard::AddBlockKeys(SDL_Keycode keyID1, SDL_Keycode keyID2) {
	m_mapKeyBlocking.insert(std::make_pair(keyID1, keyID2));
	m_mapKeyBlocking.insert(std::make_pair(keyID2, keyID1));
}