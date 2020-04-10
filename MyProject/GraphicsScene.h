#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "WindowSpecs.h"
#include "Mesh.h"
#include "GLSLProgram.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "Light.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Model.h"
#include "HemisphereLighting.h"

#include <SDL\SDL.h>
#include <map>

enum class GameState { PLAY, EXIT };

class GraphicsScene
{
public:
	~GraphicsScene();

	static GraphicsScene& GetInstance();

	void Start();

private:
	GraphicsScene();

	void InitSystems();
	void InitScene();
	void GameLoop();
	void ProcessInput();
	void AnimateModels();
	void RenderScene();

	Model& AddModel();

public:
	GLSLProgram m_glslNonLightingProgram;
	GLSLProgram m_glslLightingProgram;
	GLSLProgram m_glslGUIProgram;
	GLSLProgram m_glslShadowMapProgram;
	GLSLProgram m_glslObjectHighlightProgram;
	GLSLProgram m_glslPostprocessingProgram;
	GLSLProgram m_glslNormalsProgram;

private:
	static GraphicsScene sm_graphicsScene;

	float m_timeBefore;

	TextureManager m_textureManager;
	SDL_Window * m_pSDLWindow;

	GameState m_gameState;

	Keyboard m_keyboard;
	Mouse m_mouse;
	WindowSpecs m_windowSpecs;

	PerspectiveCamera m_camera;
	PerspectiveCamera m_cameraLight;
	Camera* m_pCurCamera;
	Light* m_pLightTracedByCamera;

	AmbientLight m_ambientLight;
	PointLight m_pointLight;
	PointLight m_pointLightAux;
	DirectionalLight m_directionalLight;
	SpotLight m_spotLight;
	HemisphereLighting m_hemisphereLight;

	std::vector<Light*> m_lights;
	std::vector<Model> m_models;

	std::vector<Model*> m_modelsOpaque;
	std::vector<Model*> m_modelsTransparent;

	Renderer m_renderer;

	std::map<float, std::pair<Model*, Mesh*>> m_sortedMeshes;

	GLuint m_textureCubeMapEnvironmentMapID;
	
	bool m_isRenderNormals;
};





