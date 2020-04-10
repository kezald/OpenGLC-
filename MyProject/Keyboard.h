#pragma once
#include <map>
#include <SDL\SDL.h>
#include "Camera.h"

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	void ApplyKeys();
	void HandleKeyPress(SDL_Keycode keyID);
	void HandleKeyRelease(SDL_Keycode keyID);

	void SetCameraRef(Camera** cameraRef);

private:
	bool IsTriggerValid(SDL_Keycode keyID);
	bool IsPressed(SDL_Keycode keyID);
	float GetLastTimePressed(SDL_Keycode keyID);

	// ============CONVINIENCE METHOD==============
	void AddBlockKeys(SDL_Keycode keyID1, SDL_Keycode keyID2);


private:
	Camera ** m_cameraRef;
	std::map<SDL_Keycode, float> m_mapKeyLastTimePressed;
	std::map<SDL_Keycode, bool> m_mapKeyPressed;
	std::map<SDL_Keycode, SDL_Keycode> m_mapKeyBlocking;
};

