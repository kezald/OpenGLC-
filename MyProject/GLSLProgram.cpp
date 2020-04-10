#include "GLSLProgram.h"
#include "Errors.h"
#include <string>
#include <fstream>
#include <sstream>

#include "Debug.h"
#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

GLSLProgram::GLSLProgram()
	: m_numAttributes(0)
	, m_programID(0)
	, m_vertShaderID(0)
	, m_fragShaderID(0)
	, m_geomShaderID(0)
{

}


GLSLProgram::~GLSLProgram()
{
}

void GLSLProgram::CompileShaders(
	const char* vertShaderFilePath,
	const char* fragShaderFilePath,
	const char* geometryShaderFilePath) {
	m_vertSource = vertShaderFilePath;
	m_fragSource = fragShaderFilePath;

	if (geometryShaderFilePath != nullptr) {
		m_geomSource = geometryShaderFilePath;
	}

	//glCreateProgram was moved here since programID must be created before binding attribute locations
	m_programID = glCreateProgram();

	m_vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (m_vertShaderID == 0) {
		fatalError("Error creating Vertex Shader.");
	}
	CompileShader(vertShaderFilePath, m_vertShaderID);


	m_fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (m_fragShaderID == 0) {
		fatalError("Error creating Fragment Shader.");
	}
	CompileShader(fragShaderFilePath, m_fragShaderID);

	if (geometryShaderFilePath != nullptr) {
		m_geomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
		if (m_geomShaderID == 0) {
			fatalError("Error creating Geometry Shader.");
		}
		CompileShader(geometryShaderFilePath, m_geomShaderID);
	}
}

void GLSLProgram::CompileShader(const char* shaderFilePath, GLuint shaderID)
{
	//=============OPENING SHADER FILE============
	std::ifstream file(shaderFilePath);
	if (file.fail()) {
		perror(shaderFilePath);
		fatalError("Error opening " + std::string(shaderFilePath));
	}

	std::string fileContents = "";

	//=============READING FROM FILE (METHOD 1)============
	//std::string line;
	//while (std::getline(file, line)) {
	//	//Adding new line since it's not added by default
	//	//Without it, the preprocessor commands (#version 130) will be mixed with the code!
	//	fileContents += line + "\n";
	//}
	//file.close();

	//=============READING FROM FILE (METHOD 2)============
	std::stringstream shaderStringStream;
	shaderStringStream << file.rdbuf();
	file.close();
	fileContents = shaderStringStream.str();

	//===============COMPILING SHADER===============
	const char* sourcePtr = fileContents.c_str();
	glShaderSource(shaderID, 1, &sourcePtr, 0);
	glCompileShader(shaderID);

	GLint isCompiled = 0;
	glGetShaderiv(m_vertShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* errorLog = new GLchar[maxLength + 1];
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, errorLog);
		errorLog[maxLength] = '\0';

		glDeleteShader(shaderID);

		std::printf(errorLog);
		fatalError("Shader compilation failed. Path: " + std::string(shaderFilePath));
	}
}

void GLSLProgram::LinkShaders() {
	glAttachShader(m_programID, m_vertShaderID);
	glAttachShader(m_programID, m_fragShaderID);
	if (m_geomShaderID != 0) {
		glAttachShader(m_programID, m_geomShaderID);
	}
	glLinkProgram(m_programID);

	GLint isLinked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* errorLog = new GLchar[maxLength + 1];
		glGetProgramInfoLog(m_programID, maxLength, &maxLength, errorLog);
		errorLog[maxLength] = '\0';

		glDeleteProgram(m_programID);

		std::printf(errorLog);
		fatalError("Shaders failed to link!");
		return;
	}
	else {
		//Detaching shaders after a successful link
		glDetachShader(m_programID, m_vertShaderID);
		glDetachShader(m_programID, m_fragShaderID);
		if (m_geomShaderID != 0) {
			glDetachShader(m_programID, m_geomShaderID);
		}
	}

	//Don't leak shaders - returning resources
	glDeleteShader(m_vertShaderID);
	glDeleteShader(m_fragShaderID);
	if (m_geomShaderID != 0) {
		glDeleteShader(m_geomShaderID);
	}
}

//void GLSLProgram::AddAttribute(const std::string& attributeName) {
//	//programID must have been created
//	glBindAttribLocation(m_programID, m_numAttributes, attributeName.c_str());
//	m_numAttributes++;
//}

bool GLSLProgram::IsUniformPresent(const char* uniformName) {
	return GL_INVALID_INDEX != glGetUniformLocation(m_programID, uniformName);
}

GLint GLSLProgram::GetUniformLocation(const std::string& uniformName) {
	return GetUniformLocation(uniformName.c_str());
}

GLint GLSLProgram::GetUniformLocation(const char* uniformName) {
	//if (m_uniformsMap.find(uniformName) != m_uniformsMap.end()) {
	//	return m_uniformsMap[uniformName];
	//}
	//else {
	GLint location = glGetUniformLocation(m_programID, uniformName);

	if (location == GL_INVALID_INDEX) {
		//The uniform doesn't exist
		fatalError("Uniform " + std::string(uniformName) + " not found in shader!");
	}

	//m_uniformsMap[uniformName] = location;

	return location;
	//}
}

void GLSLProgram::Recompile() {
	DeleteProgram();
	CompileShaders(
		m_vertSource.c_str(),
		m_fragSource.c_str(),
		(m_geomShaderID != 0) ? m_geomSource.c_str() : nullptr);
	LinkShaders();
}

void GLSLProgram::UseProgram() {
	glUseProgram(m_programID);
}

void GLSLProgram::UnuseProgram() {
	glUseProgram(0);
}

void GLSLProgram::DeleteProgram() {
	glDeleteProgram(m_programID);
	m_programID = 0;
	//m_uniformsMap.clear();
}