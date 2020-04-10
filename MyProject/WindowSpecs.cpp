#include "WindowSpecs.h"
#include <iostream>
#include <gl\glew.h>

WindowSpecs::WindowSpecs()
{
}


WindowSpecs::~WindowSpecs()
{
}

void WindowSpecs::Init(Sint32 windowStartWidth, Sint32 windowStartHeight) {
	m_windowWidth = windowStartWidth;
	m_windowHeight = windowStartHeight;
}

void WindowSpecs::HandleWindowEvent(SDL_WindowEvent& e) {
	switch (e.event) {
	case SDL_WINDOWEVENT_RESIZED:
		std::cout << "Window Resized" << std::endl;
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		std::cout << "Window Size Changed" << std::endl;
		m_windowWidth = e.data1;
		m_windowHeight = e.data2;
		break;
	}
}

Sint32 WindowSpecs::GetWindowWidth() {
	return m_windowWidth;
}

Sint32 WindowSpecs::GetWindowHeight() {
	return m_windowHeight;
}

glm::vec2 WindowSpecs::ConvertWindowCoordToNDC(glm::vec2 pos) {
	return ConvertWindowCoordToNDC(pos[0], pos[1]);
}

glm::vec2 WindowSpecs::ConvertWindowCoordToNDC(float windowX, float windowY) {
	float invertY = m_windowHeight - windowY;
	float Xprime = 2.0f * (windowX / m_windowWidth) - 1.0f;
	float Yprime = 2.0f * (invertY / m_windowHeight) - 1.0f;
	return glm::vec2(Xprime, Yprime);
}