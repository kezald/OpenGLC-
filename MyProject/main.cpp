#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "SOIL.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")

#include "GraphicsScene.h"

#include <glm/glm.hpp>
#include <map>
#include <gl\GLU.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//static _CrtMemState gMemState;

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtMemCheckpoint(&gMemState);

	GraphicsScene::GetInstance().Start();

	//atexit([]() {_CrtMemDumpAllObjectsSince(&gMemState); });

	return 0;
}
