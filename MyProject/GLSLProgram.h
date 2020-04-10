#pragma once
#include <string>
#include <GL\glew.h>
#include <unordered_map>

class GLSLProgram
{
public:
	GLSLProgram();
	~GLSLProgram();

	void CompileShaders(
		const char* vertShaderFilePath, 
		const char* fragShaderFilePath,
		const char* geometryShaderFilePath = nullptr);
	void LinkShaders();
	//void AddAttribute(const std::string& attributeName);
	bool IsUniformPresent(const char* uniformName);
	GLint GetUniformLocation(const std::string& uniformName);
	GLint GetUniformLocation(const char* uniformName);
	void Recompile();

	void UseProgram();
	void UnuseProgram();

	void DeleteProgram();

private:
	void CompileShader(const char* shaderFilePath, GLuint shaderID);

private:
	int m_numAttributes;
	GLuint m_programID;
	GLuint m_vertShaderID;
	GLuint m_fragShaderID;
	GLuint m_geomShaderID;

	std::string m_vertSource;
	std::string m_fragSource;
	std::string m_geomSource;

	//std::unordered_map<std::string, GLint> m_uniformsMap;
};