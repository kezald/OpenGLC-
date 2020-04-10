#include "TextureManager.h"
#include "LogManager.h"

//#include <SOIL\SOIL.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint TextureManager::m_whiteTextureID;

void TextureManager::Init() {
	//====================INIT white texture=======================
	//(for use with fragment that requires a sampler but you only want to supply a color)
	GLfloat whitePixel[4] = { 1.0, 1.0, 1.0, 1.0 };

	glGenTextures(1, &m_whiteTextureID);
	glBindTexture(GL_TEXTURE_2D, m_whiteTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, whitePixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextureManager::GetWhiteTextureID() {
	return m_whiteTextureID;
}

GLuint TextureManager::GetTextureCubemap(std::string skyboxFolderName, std::string extension) {
	//GLuint textureID = SOIL_load_OGL_cubemap(
	//	faceFilePaths[0].c_str(), faceFilePaths[1].c_str(),
	//	faceFilePaths[2].c_str(), faceFilePaths[3].c_str(),
	//	faceFilePaths[4].c_str(), faceFilePaths[5].c_str(),
	//	3, 0, 0);

	GLuint textureID = 0;
	std::string path = "Textures/skyboxes/" + skyboxFolderName + "/";

	if (m_loadedTextures.find(path) != m_loadedTextures.end()) {
		textureID = m_loadedTextures[path];
	}
	else {
		std::vector<std::string> filePaths{
			path + "right" + extension,
			path + "left" + extension,
			path + "top" + extension,
			path + "bottom" + extension,
			path + "back" + extension,
			path + "front" + extension
		};

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (int i = 0; i < filePaths.size(); i++) {
			std::string& filePath = filePaths[i];

			int width;
			int height;
			int channels;

			stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 3);
			//unsigned char* data = SOIL_load_image(filePath.c_str(), &width, &height, &channels, 3);

			if (data == nullptr) {
				std::cout << "CubeMap load failed!" << std::endl;
			}

			GLenum targetReal = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			GLenum targetBackFrontSwap = targetReal;

			////=============== OpenGL Flips all faces (top and bottom - vertically, the rest - horizontally). Apply some workarounds. =======================
			if (targetBackFrontSwap == GL_TEXTURE_CUBE_MAP_POSITIVE_Z) {
				targetBackFrontSwap = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			}
			else if (targetBackFrontSwap == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
				targetBackFrontSwap = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			}
			//==========================================================

			glTexImage2D(targetBackFrontSwap, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glCheckError();
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		// ============= Order of textures ================
		//GL_TEXTURE_CUBE_MAP_POSITIVE_X	Right
		//GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left
		//GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top
		//GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom
		//GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back
		//GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front

		m_loadedTextures[path] = textureID;
	}

	return textureID;
}

GLuint TextureManager::GetTexture(
	const char* pcFilename, const char* pcDirectory,
	bool isRepeat, bool isMipmap) {
	return GetTexture((pcDirectory + std::string("/") + pcFilename).c_str(), isRepeat, isMipmap);
}

GLuint TextureManager::GetTexture(const char* pcFilepath, bool isRepeat, bool isMipmap) {
	GLuint textureID = 0;

	if (m_loadedTextures.find(pcFilepath) == m_loadedTextures.end())
	{
		//// Not already loaded... so load...

		// =============== SOIL Implementation =================
		//unsigned int flags = SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y;
		//if (isRepeat) {
		//	flags |= SOIL_FLAG_TEXTURE_REPEATS;
		//}

		//textureID = SOIL_load_OGL_texture(
		//	pcFilepath,
		//	SOIL_LOAD_AUTO,
		//	SOIL_CREATE_NEW_ID,
		//	flags
		//);

		// ================= STB Image Loader Implementation ===================
		GLint paramWrap = (isRepeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
		GLint paramMipmap = (isMipmap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, paramMipmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, paramWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, paramWrap);

		int width;
		int height;
		int channels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(pcFilepath, &width, &height, &channels, 0);

		GLint paramFormat = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, paramFormat, width, height,
			0, paramFormat, GL_UNSIGNED_BYTE, data);

		if (isMipmap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		if (textureID == 0 || data == nullptr) {
			//std::cout << "SOIL Texture load failed!" << SOIL_last_result() << std::endl;
			std::cout << "Texture load failed!" << std::endl;
		}
		else {
			m_loadedTextures[pcFilepath] = textureID;
		}

		stbi_image_free(data);
	}
	else
	{
		// Is already loaded...
		textureID = m_loadedTextures[pcFilepath];
	}

	return textureID;
}