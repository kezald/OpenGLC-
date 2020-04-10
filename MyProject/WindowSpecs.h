#pragma once
#include <SDL/SDL.h>
#include <glm\glm.hpp>

class WindowSpecs
{
public:
	WindowSpecs();
	~WindowSpecs();

	void Init(Sint32 windowStartWidth, Sint32 windowStartHeight);
	void HandleWindowEvent(SDL_WindowEvent& e);
	Sint32 GetWindowWidth();
	Sint32 GetWindowHeight();
	glm::vec2 ConvertWindowCoordToNDC(glm::vec2 pos);
	glm::vec2 ConvertWindowCoordToNDC(float windowX, float windowY);

private:
	Sint32 m_windowWidth;
	Sint32 m_windowHeight;
};

