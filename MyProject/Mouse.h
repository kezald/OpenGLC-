#pragma once
#include <SDL/SDL.h>
#include <glm\glm.hpp>

class Mouse
{
public:
	Mouse();
	~Mouse();

	void HandleMouseMotion(SDL_MouseMotionEvent& e);
	void HandleMouseButtonDown(SDL_MouseButtonEvent& e);
	void HandleMouseButtonUp(SDL_MouseButtonEvent& e);

	bool IsMouseLeftButtonPressed();
	bool IsMouseRightButtonPressed();
	glm::vec2 GetMousePosOnLastMainButtonPress();
	glm::vec2 GetMousePosCurrent();

private:
	bool m_mouseLeftButtonPressed;
	bool m_mouseRightButtonPressed;
	glm::vec2 m_mousePosOnLastMainButtonPress;
	glm::vec2 m_mousePosCurrent;
};

