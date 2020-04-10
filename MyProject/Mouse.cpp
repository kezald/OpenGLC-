#include "Mouse.h"

#include <iostream>
#include <SDL\SDL.h>

Mouse::Mouse()
{
}


Mouse::~Mouse()
{
}

void Mouse::HandleMouseMotion(SDL_MouseMotionEvent& e) {
	//std::cout << "x: " << e.x << std::endl;
	//std::cout << "y: " << e.y << std::endl;
	//std::cout << "xrel: " << e.xrel << std::endl;
	//std::cout << "yrel: " << e.yrel << std::endl;

	m_mousePosCurrent[0] = e.x;
	m_mousePosCurrent[1] = e.y;
}

void Mouse::HandleMouseButtonDown(SDL_MouseButtonEvent& e) {
	if (e.button == SDL_BUTTON_LEFT || e.button == SDL_BUTTON_RIGHT) {
		m_mousePosOnLastMainButtonPress[0] = e.x;
		m_mousePosOnLastMainButtonPress[1] = e.y;
		SDL_CaptureMouse(SDL_TRUE);
	}

	if (e.button == SDL_BUTTON_LEFT) {
		m_mouseLeftButtonPressed = true;
	}
	else if (e.button == SDL_BUTTON_RIGHT) {
		m_mouseRightButtonPressed = true;
	}
}

void Mouse::HandleMouseButtonUp(SDL_MouseButtonEvent& e) {
	if (e.button == SDL_BUTTON_LEFT) {
		m_mouseLeftButtonPressed = false;
	}
	else if (e.button == SDL_BUTTON_RIGHT) {
		m_mouseRightButtonPressed = false;
	}

	if (!m_mouseLeftButtonPressed && !m_mouseRightButtonPressed) {
		SDL_CaptureMouse(SDL_FALSE);
	}
}

bool Mouse::IsMouseLeftButtonPressed() {
	return m_mouseLeftButtonPressed;
}

bool Mouse::IsMouseRightButtonPressed() {
	return m_mouseRightButtonPressed;
}

glm::vec2 Mouse::GetMousePosOnLastMainButtonPress() {
	return m_mousePosOnLastMainButtonPress;
}

glm::vec2 Mouse::GetMousePosCurrent() {
	return m_mousePosCurrent;
}