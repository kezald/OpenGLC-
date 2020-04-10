#include "Light.h"

#include <glm/gtc/type_ptr.hpp>

GLSLProgram* Light::sm_pLightingProgram = nullptr;
GLSLProgram* Light::sm_pShadowMapProgram = nullptr;
Camera* Light::sm_pCamera = nullptr;
const int Light::DEPTH_TEXTURE_SIZE = 1280;

Light::Light(
	glm::vec3 ambient, glm::vec3 diffuse,
	glm::vec3 specular, float intensity, LightType type)
	: m_isEnabled{ true }
	, m_ambient{ ambient }
	, m_diffuse{ diffuse }
	, m_specular{ specular }
	, m_intensity{ intensity }
	, m_lightIndex{ 0 }
	, m_constantAttenuation(1.0f)
	, m_linearAttenuation(0.0f)
	, m_quadraticAttenuation(0.0f)
	, m_type(type)
	, m_pShadowCamera(nullptr)
	, m_depthTextureID{ 0 }
	, m_isShadowEnabled{ false }
	, m_fboShadowMap{ 0 }
	, m_direction(glm::vec3(0.0f, -1.0f, 0.0f))
{

}

Light::Light(glm::vec3 color, float intensity, LightType type)
	: Light(color, color, color, intensity, type)
{

}

Light::~Light()
{
}

void Light::Apply() {
	GLint isEnabledLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isEnabled");
	GLint lightAmbient = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".ambient");
	GLint lightDiffuse = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".diffuse");
	GLint lightSpecular = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".specular");
	GLint constantAttenuationLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".constantAttenuation");
	GLint linearAttenuationLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".linearAttenuation");
	GLint quadraticAttenuationLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".quadraticAttenuation");
	GLint shadowEnabledLoc = sm_pLightingProgram->GetUniformLocation(GetLightIndexString() + ".isShadowEnabled");

	glUniform3fv(lightAmbient, 1, glm::value_ptr(m_ambient * m_intensity));
	glUniform3fv(lightDiffuse, 1, glm::value_ptr(m_diffuse * m_intensity));
	glUniform3fv(lightSpecular, 1, glm::value_ptr(m_specular * m_intensity));
	glUniform1f(constantAttenuationLoc, m_constantAttenuation);
	glUniform1f(linearAttenuationLoc, m_linearAttenuation);
	glUniform1f(quadraticAttenuationLoc, m_quadraticAttenuation);
	glUniform1i(isEnabledLoc, m_isEnabled);
	glUniform1i(shadowEnabledLoc, m_isShadowEnabled);
}

void Light::BindShadowMap() {
	if (m_depthTextureID == 0 && m_fboShadowMap == 0) {
		//===============INIT FRAMEBUFFER WITH DEPTH ATTACHMENT==============================
		glGenTextures(1, &m_depthTextureID);
		glBindTexture(GL_TEXTURE_2D, m_depthTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
			DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE /*GL_NONE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &m_fboShadowMap);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadowMap);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTextureID, 0);
		glDrawBuffer(GL_NONE); // Modifies framebuffer object state
							   //glReadBuffer(GL_NONE); // Modifies framebuffer object state

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			assert(false);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadowMap);
	glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Light::SetAttenuation(float cons, float linear, float quadratic) {
	m_constantAttenuation = cons;
	m_linearAttenuation = linear;
	m_quadraticAttenuation = quadratic;
}

void Light::SetPosition(glm::vec3 position) {
	m_position = position;
}

void Light::SetLightIndex(int index) {
	m_lightIndex = index;
}

void Light::SetLightingProgram(GLSLProgram* pProgram) {
	sm_pLightingProgram = pProgram;
}

void Light::SetShadowMapProgram(GLSLProgram* pProgram) {
	sm_pShadowMapProgram = pProgram;
}

void Light::SetCamera(Camera* pCamera) {
	sm_pCamera = pCamera;
}

glm::vec3 Light::GetPosition() {
	return m_position;
}

std::string Light::GetLightIndexString() {
	std::string str = "Lights[";
	str += std::to_string(m_lightIndex);
	str += "]";
	return str;
}

LightType Light::GetLightType() {
	return m_type;
}

glm::vec3 Light::GetSpecular() {
	return m_specular;
}

glm::vec3 Light::GetDominantColor() {
	return m_specular;
}

Camera* Light::GetShadowCamera() {
	return m_pShadowCamera.get();
}

GLuint Light::GetDepthTextureID() {
	return m_depthTextureID;
}

bool Light::GetIsShadowEnabled() {
	return m_isShadowEnabled;
}

void Light::SetDirection(glm::vec3 direction) {
	m_direction = glm::normalize(direction);
}

glm::vec3 Light::GetDirection() {
	return glm::normalize(m_direction);
}