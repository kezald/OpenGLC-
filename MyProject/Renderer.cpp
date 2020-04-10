#include "Renderer.h"
#include "LogManager.h"
#include "Constants.h"
#include "ProceduralGenerator.h"
#include "Line.h"
#include "Point.h"
#include "GraphicsScene.h"

#include <glm\gtc\type_ptr.hpp>
#include <assert.h>

#include "Debug.h"
#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

const int Renderer::OFFSCREEN_FBO_WIDTH = 2920;
const int Renderer::OFFSCREEN_FBO_HEIGHT = 2080;

Renderer::Renderer(WindowSpecs& windowSpecs)
	: m_windowSpecs{ windowSpecs }
	, m_textureColorOffscreenID{ 0 }
	, m_rboDepthStencilOffscreenID{ 0 }
	, m_fboOffscreenID{ 0 }
{

}

Renderer::~Renderer()
{
}

void Renderer::Init() {
	MeshDataHolder dataHolder = ProceduralGenerator::Generate2DScreenQuad();
	m_2dQuad.AddMesh(dataHolder);

	dataHolder = ProceduralGenerator::GenerateSphereModel();
	m_lightSphere.AddMesh(dataHolder);
	m_lightSphere.m_meshes[0].m_ambient = glm::vec3(0.0f, 1.0f, 1.0f);
	m_lightSphere.m_meshes[0].m_scale = glm::vec3(0.1f);

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureBindings);

	// ========== Off-screen Framebuffer Init ===============
	glGenTextures(1, &m_textureColorOffscreenID);
	glBindTexture(GL_TEXTURE_2D, m_textureColorOffscreenID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		OFFSCREEN_FBO_WIDTH, OFFSCREEN_FBO_HEIGHT,
		0, GL_RGB, GL_UNSIGNED_BYTE /*any valid value*/, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &m_rboDepthStencilOffscreenID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepthStencilOffscreenID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		OFFSCREEN_FBO_WIDTH, OFFSCREEN_FBO_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &m_fboOffscreenID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboOffscreenID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_textureColorOffscreenID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepthStencilOffscreenID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		assert(false);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ApplyLighting(std::vector<Light*>& lights) {
	GLSLProgram* sm_pLightingProgram = &GraphicsScene::GetInstance().m_glslLightingProgram;

	sm_pLightingProgram->UseProgram();

	static const glm::mat4 scaleBiasMatrix = glm::mat4(
		glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	int depthMapStartIndex = (int)TextureMaps::TotalCount;
	int depthMapCount = 0;

	for (int i = 0; i < lights.size(); i++) {
		Light* pLight = lights[i];

		pLight->SetLightIndex(i);
		pLight->Apply();

		if (pLight->GetIsShadowEnabled()) {
			assert(depthMapCount < m_maxTextureBindings);

			GLint lightProjMatrixLoc = sm_pLightingProgram->GetUniformLocation(pLight->GetLightIndexString() + ".scaleBiasPVLightMatrix");
			glUniformMatrix4fv(lightProjMatrixLoc, 1, GL_FALSE,
				glm::value_ptr(scaleBiasMatrix *
					pLight->GetShadowCamera()->GetProjectionMatrix() *
					pLight->GetShadowCamera()->GetViewMatrix()));

			GLint depthMapLoc = sm_pLightingProgram->GetUniformLocation(pLight->GetLightIndexString() + ".depthMap");
			glUniform1i(depthMapLoc, depthMapStartIndex + depthMapCount);
			glActiveTexture(GL_TEXTURE0 + depthMapStartIndex + depthMapCount);
			glBindTexture(GL_TEXTURE_2D, pLight->GetDepthTextureID());
			depthMapCount++;
		}
	}

	sm_pLightingProgram->UnuseProgram();
}

void Renderer::RenderLighting(std::vector<Model*>& models, Camera* camera) {
	for (Model* model : models) {
		RenderLighting(model, camera);
	}
}

void Renderer::ApplyHighlights(std::vector<Model>& models, Camera* camera) {
	GLSLProgram* sm_pObjectHighlightProgram = &GraphicsScene::GetInstance().m_glslObjectHighlightProgram;

	// ================== Object Highlight Phase =====================
	sm_pObjectHighlightProgram->UseProgram();
	for (Model& rModel : models) {
		Model* model = &rModel;
		if (model->m_isHighlight) {
			glClearStencil(0);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);
			glDepthFunc(GL_NEVER);

			for (Mesh& mesh : model->m_meshes) {
				GLint mvpLoc = sm_pObjectHighlightProgram->GetUniformLocation("MVPmatrix");
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(
					camera->GetViewProjectionMatrix() *
					model->GetModelMatrix() * mesh.GetModelMatrix()));
				Render(&mesh);
			}

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glDepthFunc(GL_LESS);

			glm::vec3 scaleTemp = model->m_scale;
			model->m_scale += 0.3f;

			for (Mesh& mesh : model->m_meshes) {
				GLint mvpLoc = sm_pObjectHighlightProgram->GetUniformLocation("MVPmatrix");
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(
					camera->GetViewProjectionMatrix() *
					model->GetModelMatrix() * mesh.GetModelMatrix()));
				Render(&mesh);
			}

			model->m_scale = scaleTemp;
		}
	}

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDepthFunc(GL_LESS);

	sm_pObjectHighlightProgram->UnuseProgram();
}

void Renderer::RenderLighting(Model* modelParent, Camera* camera, std::vector<std::pair<Model*, Mesh*>>* sortedMeshes, GLuint textureCubemapID) {
	GLSLProgram* sm_pLightingProgram = &GraphicsScene::GetInstance().m_glslLightingProgram;

	sm_pLightingProgram->UseProgram();

	if (textureCubemapID != 0) {
		GLint cubeMapEnvironmentLoc = sm_pLightingProgram->GetUniformLocation("cubeMapEnvironmentMapping");
		glUniform1i(cubeMapEnvironmentLoc, (int)TextureMaps::CubeMapEnvironmentMap);
		glActiveTexture(GL_TEXTURE0 + (int)TextureMaps::CubeMapEnvironmentMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubemapID);
	}

	GLint cameraPosWorldLoc = sm_pLightingProgram->GetUniformLocation("CameraPositionWorldSpace");
	glUniform3fv(cameraPosWorldLoc, 1, glm::value_ptr(camera->GetPosition()));

	GLint diffuseMapSamplerLoc = sm_pLightingProgram->GetUniformLocation("Material.diffuseMap");
	glUniform1i(diffuseMapSamplerLoc, (int)TextureMaps::DiffuseMap);
	GLint specularMapSamplerLoc = sm_pLightingProgram->GetUniformLocation("Material.specularMap");
	glUniform1i(specularMapSamplerLoc, (int)TextureMaps::SpecularMap);
	GLint cubeMapSamplerLoc = sm_pLightingProgram->GetUniformLocation("Material.cubeMap");
	glUniform1i(cubeMapSamplerLoc, (int)TextureMaps::CubeMap);

	GLint pLoc = sm_pLightingProgram->GetUniformLocation("Pmatrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	GLint mLoc = sm_pLightingProgram->GetUniformLocation("Mmatrix");
	GLint mvLoc = sm_pLightingProgram->GetUniformLocation("MVmatrix");
	//GLint mvpLoc = sm_pLightingProgram->GetUniformLocation("MVPmatrix");
	GLint normalMatrixMVLoc = sm_pLightingProgram->GetUniformLocation("NormalMatrixMV");
	GLint normalMatrixMLoc = sm_pLightingProgram->GetUniformLocation("NormalMatrixM");

	GLint ambientLoc = sm_pLightingProgram->GetUniformLocation("Material.ambient");
	GLint diffuseLoc = sm_pLightingProgram->GetUniformLocation("Material.diffuse");
	GLint specularLoc = sm_pLightingProgram->GetUniformLocation("Material.specular");
	GLint shininessLoc = sm_pLightingProgram->GetUniformLocation("Material.shininess");

	std::vector<std::pair<Model*, Mesh*>>::iterator it;
	if (sortedMeshes != nullptr) {
		it = std::begin(*sortedMeshes);
	}
	int i = 0;

	// If sortedMeshes is present - iteratie over them. Otherwise, iterate over children of modelParent
	while (true) {
		Mesh* pMesh = nullptr;
		Model* pModel = modelParent;
		if (sortedMeshes != nullptr) {
			if (it == sortedMeshes->end()) {
				break;
			}
			else {
				pModel = (*it).first;
				pMesh = (*it).second;
				it++;
			}
		}
		else {
			if (i == modelParent->m_meshes.size()) {
				break;
			}
			else {
				pMesh = &modelParent->m_meshes[i];
				i++;
			}
		}
		Mesh& mesh = *pMesh;

		glm::mat4 meshM = pModel->GetModelMatrix() * mesh.GetModelMatrix();
		glm::mat4 viewMatrix =
			(pModel->m_isCenteredOnPlayer) ?
			glm::mat4(glm::mat3(camera->GetViewMatrix())) :
			camera->GetViewMatrix();
		glm::mat4 meshMV = viewMatrix * meshM;
		glm::mat4 meshMVP = camera->GetProjectionMatrix() * meshMV;

		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(meshM));
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(meshMV));
		//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(meshMVP));
		glUniformMatrix4fv(normalMatrixMVLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(meshMV))));
		glUniformMatrix4fv(normalMatrixMLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(meshM))));

		glUniform3fv(ambientLoc, 1, glm::value_ptr(mesh.m_ambient));
		glUniform3fv(diffuseLoc, 1, glm::value_ptr(mesh.m_diffuse));
		glUniform3fv(specularLoc, 1, glm::value_ptr(mesh.m_specular));
		glUniform1f(shininessLoc, mesh.m_shininess);

		GLint getAlphaFromDiffuseMapLoc = sm_pLightingProgram->GetUniformLocation("Material.isGetAlphaFromDiffuseMap");
		glUniform1i(getAlphaFromDiffuseMapLoc, pModel->m_isGetAlphaFromDiffuseMap);

		GLint diffuseMapPresentLoc = sm_pLightingProgram->GetUniformLocation("Material.isDiffuseMapPresent");
		glUniform1i(diffuseMapPresentLoc, mesh.isDiffuseMapPresent());

		GLint specularMapPresentLoc = sm_pLightingProgram->GetUniformLocation("Material.isSpecularMapPresent");
		glUniform1i(specularMapPresentLoc, mesh.isSpecularMapPresent());

		GLint cubeMapPresentLoc = sm_pLightingProgram->GetUniformLocation("Material.isCubeMapPresent");
		glUniform1i(cubeMapPresentLoc, mesh.isCubeMapPresent());

		GLint isReflectionMappedLoc = sm_pLightingProgram->GetUniformLocation("Material.isReflectionMapped");
		glUniform1i(isReflectionMappedLoc, pModel->m_isReflectionMapped);

		GLint isRefractionMappedLoc = sm_pLightingProgram->GetUniformLocation("Material.isRefractionMapped");
		glUniform1i(isRefractionMappedLoc, pModel->m_isRefractionMapped);

		glActiveTexture(GL_TEXTURE0 + (int)TextureMaps::DiffuseMap);
		glBindTexture(GL_TEXTURE_2D, mesh.GetDiffuseMapTextureID());

		glActiveTexture(GL_TEXTURE0 + (int)TextureMaps::SpecularMap);
		glBindTexture(GL_TEXTURE_2D, mesh.GetSpecularMapTextureID());

		glActiveTexture(GL_TEXTURE0 + (int)TextureMaps::CubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mesh.GetCubeMapTextureID());

		Render(&mesh);
	}
	sm_pLightingProgram->UnuseProgram();

	glCheckError();
}

void Renderer::RenderNonLighting(Model* model, Camera* camera) {
	GLSLProgram* sm_pNonLightingProgram = &GraphicsScene::GetInstance().m_glslNonLightingProgram;

	GLint mvpLoc = sm_pNonLightingProgram->GetUniformLocation("MVPmatrix");
	GLint objectColorLoc = sm_pNonLightingProgram->GetUniformLocation("objectColor");

	for (Mesh& mesh : model->m_meshes) {
		glm::mat4 meshMVP = camera->GetProjectionMatrix() * camera->GetViewMatrix() * model->GetModelMatrix() * mesh.GetModelMatrix();
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(meshMVP));

		glUniform3fv(objectColorLoc, 1, glm::value_ptr(mesh.GetColor()));

		Render(&mesh);
	}
}

void Renderer::RenderShadowMaps(std::vector<Model*> models, std::vector<Light*> lights) {
	GLSLProgram* sm_pShadowMapProgram = &GraphicsScene::GetInstance().m_glslShadowMapProgram;
	
	sm_pShadowMapProgram->UseProgram();
	for (Light* pLight : lights) {
		if (pLight->GetIsShadowEnabled()) {
			pLight->BindShadowMap();

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(2.0f, 4.0f);

			for (Model* model : models) {
				if (model->m_isShadowCaster) {
					RenderShadowMap(model, pLight);
					glCheckError();
				}
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
			glCheckError();

			// =============== Direct Reading of Depth Values ==============
			//glBindTexture(GL_TEXTURE_2D, pLight->GetDepthTextureID());
			//static GLfloat* pix = new GLfloat[pLight->DEPTH_TEXTURE_SIZE * pLight->DEPTH_TEXTURE_SIZE];
			//glPixelStorei(GL_PACK_ALIGNMENT, 1);
			//glCheckError();
			//glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pix);
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glCheckError();
			//for (int i = 0; i < pLight->DEPTH_TEXTURE_SIZE * pLight->DEPTH_TEXTURE_SIZE; i++) {
			//	if (pix[i] < 1.0f) {
			//		int j = 3;
			//	}
			//}

			//// ============== Default Framebuffer ============
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, m_windowSpecs.getWindowWidth(), m_windowSpecs.getWindowHeight());
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glCheckError();

			//// ============= Display the contents of Depth Map ================
			//GLint mvpLoc = sm_pShadowMapProgram->GetUniformLocation("MVPmatrix");
			//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
			//GLint samplerLoc = sm_pShadowMapProgram->GetUniformLocation("depthMap");
			//glUniform1i(samplerLoc, 0);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, pLight->GetDepthTextureID());
			//Render(&(m_2dQuad.m_meshes[0]));
			//glBindTexture(GL_TEXTURE_2D, 0);

			// ============= Copy contents of framebuffer ===============
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboShadowMap);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			//glBlitFramebuffer(
			//	0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE,
			//	0, 0, m_windowSpecs.getWindowWidth(), m_windowSpecs.getWindowHeight(),
			//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
	}
	sm_pShadowMapProgram->UnuseProgram();
}

void Renderer::RenderShadowMap(Model* model, Light* pLight) {
	GLSLProgram* sm_pShadowMapProgram = &GraphicsScene::GetInstance().m_glslShadowMapProgram;

	GLint mvpLoc = sm_pShadowMapProgram->GetUniformLocation("MVPmatrix");

	Camera* shadowCamera = pLight->GetShadowCamera();

	for (Mesh& mesh : model->m_meshes) {
		glm::mat4 meshMVP =
			shadowCamera->GetProjectionMatrix() *
			shadowCamera->GetViewMatrix() *
			model->GetModelMatrix() *
			mesh.GetModelMatrix();
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(meshMVP));
		Render(&mesh);
	}
}

void Renderer::Render(Mesh* mesh) {
	glBindVertexArray(mesh->m_meshBuffers->m_vaoID);

	if (mesh->m_meshBuffers->m_indicesEBO != 0) {
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(Constants::PRIMITIVE_RESTART_INDEX);
		glDrawElements(mesh->m_drawingMode, mesh->m_meshBuffers->m_indexCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(mesh->m_drawingMode, 0, mesh->m_meshBuffers->m_vertexCount);
	}

	glBindVertexArray(0);
}

void Renderer::RenderLightBulbs(std::vector<Light*> lights, Camera* camera) {
	GLSLProgram* sm_pNonLightingProgram = &GraphicsScene::GetInstance().m_glslNonLightingProgram;

	sm_pNonLightingProgram->UseProgram();

	for (int i = 0; i < lights.size(); i++) {
		Light* pLight = lights[i];

		if (pLight->GetLightType() == LightType::Point ||
			pLight->GetLightType() == LightType::Spot ||
			pLight->GetLightType() == LightType::Hemisphere) {
			m_lightSphere.m_meshes[0].m_position = pLight->GetPosition();
			m_lightSphere.m_meshes[0].m_ambient = pLight->GetDominantColor();
			m_lightSphere.m_meshes[0].m_diffuse = pLight->GetDominantColor();
			m_lightSphere.m_meshes[0].m_specular = pLight->GetDominantColor();
			RenderNonLighting(&m_lightSphere, camera);
		}
	}

	sm_pNonLightingProgram->UnuseProgram();
}

void Renderer::RenderAxes(Camera* camera) {
	GLSLProgram* sm_pNonLightingProgram = &GraphicsScene::GetInstance().m_glslNonLightingProgram;

	sm_pNonLightingProgram->UseProgram();
	static Line axes[3];
	axes[0].m_pointEnd = glm::vec3(1.0f, 0.0f, 0.0f);
	axes[1].m_pointEnd = glm::vec3(0.0f, 1.0f, 0.0f);
	axes[2].m_pointEnd = glm::vec3(0.0f, 0.0f, 1.0f);
	axes[0].m_color = glm::vec3(1.0f, 0.0f, 0.0f);
	axes[1].m_color = glm::vec3(0.0f, 1.0f, 0.0f);
	axes[2].m_color = glm::vec3(0.0f, 0.0f, 1.0f);

	GLint mvpLoc = sm_pNonLightingProgram->GetUniformLocation("MVPmatrix");
	GLint objectColorLoc = sm_pNonLightingProgram->GetUniformLocation("objectColor");

	for (Line& axis : axes) {
		axis.m_lineWidth = 1.0f;
		axis.m_pointStart = glm::vec3(0.0f, 0.0f, 0.0f);
		axis.UpdateBuffer();

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewProjectionMatrix()));
		glUniform3fv(objectColorLoc, 1, glm::value_ptr(axis.m_color));
		axis.RenderLighting();
	}
	sm_pNonLightingProgram->UnuseProgram();
}

void Renderer::RenderMouseOverlay(Mouse& mouse) {
	GLSLProgram* sm_pGUIProgram = &GraphicsScene::GetInstance().m_glslGUIProgram;

	sm_pGUIProgram->UseProgram();

	static const glm::mat4 projMatrix = glm::ortho(-1, 1, -1, 1);
	static Point point;
	static Line line;
	point.m_color = glm::vec3(1.0f, 1.0f, 1.0f);
	point.m_size = 5;
	line.m_lineWidth = 2;
	line.m_color = glm::vec3(0.0f, 0.0f, 1.0f);

	GLint objectColorLoc = sm_pGUIProgram->GetUniformLocation("objectColor");

	if (mouse.IsMouseLeftButtonPressed() || mouse.IsMouseRightButtonPressed()) {
		glm::vec3 mouseNDCCoordsPress = glm::vec3(m_windowSpecs.ConvertWindowCoordToNDC(mouse.GetMousePosOnLastMainButtonPress()), 0);
		glm::vec3 mouseNDCCoordsCur = glm::vec3(m_windowSpecs.ConvertWindowCoordToNDC(mouse.GetMousePosCurrent()), 0);

		point.m_position = mouseNDCCoordsCur;
		point.UpdateBuffer();
		line.m_pointStart = mouseNDCCoordsPress;
		line.m_pointEnd = mouseNDCCoordsCur;
		line.UpdateBuffer();

		GLint pLoc = sm_pGUIProgram->GetUniformLocation("Pmatrix");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glUniform3fv(objectColorLoc, 1, glm::value_ptr(point.m_color));
		point.RenderLighting();
		glUniform3fv(objectColorLoc, 1, glm::value_ptr(line.m_color));
		line.RenderLighting();
		glCheckError();
	}

	sm_pGUIProgram->UnuseProgram();
}

void Renderer::RenderNormals(std::vector<Model>& models, Camera* camera) {
	GLSLProgram* sm_pNormalsProgram = &GraphicsScene::GetInstance().m_glslNormalsProgram;

	sm_pNormalsProgram->UseProgram();

	GLint mvpLoc = sm_pNormalsProgram->GetUniformLocation("MVPmatrix");

	for (Model& model : models) {
		for (Mesh& mesh : model.m_meshes) {
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(
				camera->GetViewProjectionMatrix() *
				model.GetModelMatrix() *
				mesh.GetModelMatrix()));
			Render(&mesh);
		}
	}

	sm_pNormalsProgram->UnuseProgram();
}

void Renderer::DisplayOffscreenFramebuffer() {
	GLSLProgram* sm_pPostprocessingProgram = &GraphicsScene::GetInstance().m_glslPostprocessingProgram;

	sm_pPostprocessingProgram->UseProgram();
	GLint sceneTextureLoc = sm_pPostprocessingProgram->GetUniformLocation("SceneTexture");
	glUniform1i(sceneTextureLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureColorOffscreenID);
	Render(&(m_2dQuad.m_meshes[0]));
	glBindTexture(GL_TEXTURE_2D, 0);
	sm_pPostprocessingProgram->UnuseProgram();
}

void Renderer::BindDefaultFramebuffer() {
	// ============== Default Framebuffer ============
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_windowSpecs.GetWindowWidth(), m_windowSpecs.GetWindowHeight());
}

void Renderer::BindOffscreenFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboOffscreenID);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, OFFSCREEN_FBO_WIDTH, OFFSCREEN_FBO_HEIGHT);
}

void Renderer::ClearFramebuffer() {
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glCheckError();
}
