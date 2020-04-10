#pragma once

#include <gl\glew.h>
#include <unordered_map>
#include <vector>
#include <string>

class TextureManager {
public:
	static void Init();
	static GLuint GetWhiteTextureID();
	GLuint GetTextureCubemap(std::string skyboxFolderName, std::string extension);
	GLuint GetTexture(const char* pcFilepath, 
		bool isRepeat = true, bool isMipmap = false);
	GLuint GetTexture(const char* pcFilename, const char* pcDirectory, 
		bool isRepeat = true, bool isMipmap = false);

private:
	static GLuint m_whiteTextureID;
	std::unordered_map<std::string, GLuint> m_loadedTextures;
};