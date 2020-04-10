#pragma once

#include "GL\glew.h"

#define glCheckError() LogManager::glCheckError_(__FILE__, __LINE__) 

class LogManager
{
public:
	LogManager();
	~LogManager();

	static GLenum glCheckError_(const char *file, int line);
};

