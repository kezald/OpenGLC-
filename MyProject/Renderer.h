#pragma once

#include "Mesh.h"
#include "GLSLProgram.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "WindowSpecs.h"
#include "Model.h"
#include "Mouse.h"

#include <vector>
#include <map>

enum class TextureMaps { 
	DiffuseMap, SpecularMap, CubeMap, CubeMapEnvironmentMap, /*Add material maps here*/
	TotalCount /*Everything else will be used by light shadow maps*/ };

class Renderer
{
public:
	Renderer(WindowSpecs& windowSpecs);
	~Renderer();

	void Init();

	void Render(Mesh* model);
	void ApplyLighting(std::vector<Light*>& lights);
	void RenderLighting(std::vector<Model*>& models, Camera* camera);
	void RenderLighting(
		Model* modelParent, Camera* camera, 
		std::vector<std::pair<Model*, Mesh*>>* sortedMeshes = nullptr, 
		GLuint textureCubemapID = 0);
	void ApplyHighlights(std::vector<Model>& models, Camera* camera);
	void RenderNonLighting(Model* model, Camera* camera);
	void RenderShadowMaps(std::vector<Model*> models, std::vector<Light*> lights);
	void RenderShadowMap(Model* model, Light* pLight);
	void RenderLightBulbs(std::vector<Light*> lights, Camera* camera);
	void RenderAxes(Camera* camera);
	void RenderMouseOverlay(Mouse& mouse);
	void DisplayOffscreenFramebuffer();

	void RenderNormals(std::vector<Model>& models, Camera* camera);

	void BindDefaultFramebuffer();
	void BindOffscreenFramebuffer();
	void ClearFramebuffer();


private:
	WindowSpecs& m_windowSpecs;

	Model m_2dQuad;
	Model m_lightSphere;

	GLint m_maxTextureBindings;

	// ========= Off-screen Rendering Stuff ============
	static const int OFFSCREEN_FBO_WIDTH;
	static const int OFFSCREEN_FBO_HEIGHT;
	GLuint m_fboOffscreenID;
	GLuint m_rboDepthStencilOffscreenID;
	GLuint m_textureColorOffscreenID;
};

