#include "GraphicsScene.h"
#include "TextureManager.h"
#include "Point.h"
#include "Line.h"
#include "ProceduralGenerator.h"
#include "LogManager.h"
#include "Model.h"

#include <GL\glew.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL\SOIL.h>
#include <algorithm>
#include <string>

GraphicsScene GraphicsScene::sm_graphicsScene;

GraphicsScene::GraphicsScene()
	: m_camera{ 60.0f, 4.0f / 3.0f, 0.1f, 1000.0f }
	, m_cameraLight{ 60.0f, 4.0f / 3.0f, 0.1f, 1000.0f }
	, m_keyboard()
	, m_ambientLight(glm::vec3(0.02f, 0.02f, 0.02f))
	, m_pointLight(
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f))
	, m_pointLightAux(
		glm::vec3(0.0f, 0.2f, 0.2f),
		glm::vec3(0.0f, 0.8f, 0.8f),
		glm::vec3(0.0f, 1.0f, 1.0f))
	, m_directionalLight(
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f))
	, m_spotLight(
		glm::vec3(0.2f, 0.0f, 0.2f),
		glm::vec3(0.8f, 0.0f, 0.8f),
		glm::vec3(1.0f, 0.0f, 1.0f))
	, m_lights{
	&m_pointLight, &m_pointLightAux,
	&m_ambientLight,&m_spotLight, &m_directionalLight
	, &m_hemisphereLight }
	, m_pLightTracedByCamera(nullptr)
	, m_hemisphereLight(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.05f))
	, m_renderer{ m_windowSpecs }
	, m_isRenderNormals{ false }
{
	m_directionalLight.SetDirection(glm::vec3(-1.0f, -1.0f, 1.0f));
	m_pointLight.SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
	m_pointLight.SetAttenuation(1.0f, 0.0f, 0.002f);
	m_pointLightAux.SetAttenuation(1.0f, 0.0f, 0.01f);
	m_pointLightAux.SetPosition(glm::vec3(20.0f, 1.0f, -20.0f));
	m_spotLight.SetAttenuation(1.0f, 0.0f, 0.001f);
	m_spotLight.SetPosition(glm::vec3(-4.0f, 2.0f, -4.0f));
	m_spotLight.SetDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	m_spotLight.SetCutoffAngleInner(glm::radians(45.0f));
	m_hemisphereLight.SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	m_hemisphereLight.SetAttenuation(1.0f, 0.0f, 0.002f);
}

GraphicsScene::~GraphicsScene()
{
	
}

GraphicsScene& GraphicsScene::GetInstance() {
	return sm_graphicsScene;
}

void GraphicsScene::Start() {
	InitSystems();
	InitScene();

	for (Model& model : m_models) {
		if (model.m_isTransparentModel) {
			m_modelsTransparent.push_back(&model);
		}
		else {
			m_modelsOpaque.push_back(&model);
		}
	}

	GameLoop();
}

Model& GraphicsScene::AddModel() {
	m_models.emplace_back();
	return m_models.back();
}

void GraphicsScene::InitSystems() {
	int windowStartWidth = 970;
	int windowStartHeight = 970;

	//=========INIT SDL (OpenGL version, Context and Window)===========
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	m_pSDLWindow = SDL_CreateWindow("My Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowStartWidth, windowStartHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext glContext = SDL_GL_CreateContext(m_pSDLWindow);

	glewInit();
	SDL_GL_SetSwapInterval(1);

	m_renderer.Init();

	//===========INIT SHADERS=========
	m_glslLightingProgram.CompileShaders(
		"Shaders/VertexShaderLighting.vert",
		"Shaders/FragmentShaderLighting.frag",
		"Shaders/GeometryShaderExplode.geom");
	m_glslLightingProgram.LinkShaders();
	m_glslLightingProgram.UseProgram();
	glVertexAttrib4f(1, 1.0f, 1.0f, 1.0f, 1.0f);
	glVertexAttrib2f(2, 0.0f, 0.0f);
	glVertexAttrib3f(3, 0.0f, 1.0f, 0.0f);
	m_glslLightingProgram.UnuseProgram();
	Light::SetLightingProgram(&m_glslLightingProgram);

	m_glslGUIProgram.CompileShaders(
		"Shaders/VertexShaderGUI.vert",
		"Shaders/FragmentShaderGUI.frag");
	m_glslGUIProgram.LinkShaders();

	m_glslNonLightingProgram.CompileShaders("Shaders/VertexShaderNonLighting.vert", "Shaders/FragmentShaderNonLighting.frag");
	m_glslNonLightingProgram.LinkShaders();

	m_glslShadowMapProgram.CompileShaders("Shaders/VertexShaderShadowMap.vert", "Shaders/FragmentShaderShadowMap.frag");
	m_glslShadowMapProgram.LinkShaders();
	Light::SetShadowMapProgram(&m_glslShadowMapProgram);

	m_glslObjectHighlightProgram.CompileShaders("Shaders/VertexShaderHighlight.vert", "Shaders/FragmentShaderHighlight.frag");
	m_glslObjectHighlightProgram.LinkShaders();

	m_glslPostprocessingProgram.CompileShaders("Shaders/VertexShaderPostprocessing.vert", "Shaders/FragmentShaderPostprocessing.frag");
	m_glslPostprocessingProgram.LinkShaders();

	m_glslNormalsProgram.CompileShaders(
		"Shaders/VertexShaderNormals.vert",
		"Shaders/FragmentShaderNormals.frag",
		"Shaders/GeometryShaderNormals.geom");
	m_glslNormalsProgram.LinkShaders();

	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CW); // reverse ordering rule
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	//=============INIT system objects==================
	TextureManager::Init();
	Model::SetTextureManager(&m_textureManager);
	m_windowSpecs.Init(windowStartWidth, windowStartHeight);

	m_pCurCamera = &m_camera;
	m_camera.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	m_keyboard.SetCameraRef(&m_pCurCamera);
	m_cameraLight.SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));

	//===========Print Extensions=============
	//int max = 0;
	//glGetIntegerv(GL_NUM_EXTENSIONS, &max);
	//for (int i = 0; i < max; i++)
	//	std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
}

void GraphicsScene::InitScene() {
	//============INIT SCENE=============
	m_gameState = GameState::PLAY;

	m_textureCubeMapEnvironmentMapID = m_textureManager.GetTextureCubemap("Lake", ".jpg");

	MeshDataHolder dataHolder = ProceduralGenerator::GenerateTriangleModel();
	Model& m_triangle = AddModel();
	m_triangle.AddMesh(dataHolder);
	m_triangle.m_position = glm::vec3(-40.0f, 5.0f, 0.0f);
	m_triangle.m_scale = glm::vec3(4.0f);
	m_triangle.m_meshes[0].SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	m_triangle.m_isHighlight = true;
	m_triangle.m_meshes[0].m_shininess = 200.0f;
	m_triangle.m_rotationYDeg = 45.0f;

	dataHolder = ProceduralGenerator::GenerateGridModel();
	Model& m_grid = AddModel();
	m_grid.AddMesh(dataHolder);
	m_grid.m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_grid.m_meshes[0].m_shininess = 200.0f;
	m_grid.m_scale = glm::vec3(100.0f, 1.0f, 100.0f);
	m_grid.m_meshes[0].SetDiffuseMapTextureID(m_textureManager.GetTexture("Textures/container2.png", true, true));
	m_grid.m_meshes[0].SetSpecularMapTextureID(m_textureManager.GetTexture("Textures/container2_specular.png", true, true));

	dataHolder = ProceduralGenerator::GenerateCubeModel();
	Model& m_cube = AddModel();
	m_cube.AddMesh(dataHolder);
	m_cube.m_position = glm::vec3(20.0f, 3.5f, -20.0f);
	m_cube.m_meshes[0].m_shininess = 32.0f;
	m_cube.m_scale = glm::vec3(3.0f);
	m_cube.m_meshes[0].SetCubeMapTextureID(m_textureManager.GetTextureCubemap("mp_bloodgorge", ".tga"));

	Model& m_cube2 = AddModel();
	m_cube2.AddMesh(dataHolder);
	m_cube2.m_position = glm::vec3(-20.0f, 3.5f, -20.0f);
	m_cube2.m_meshes[0].m_shininess = 32.0f;
	m_cube2.m_scale = glm::vec3(3.0f);
	m_cube2.m_meshes[0].SetCubeMapTextureID(m_textureManager.GetTextureCubemap("Lake", ".jpg"));

	Model& m_cube3 = AddModel();
	m_cube3.AddMesh(dataHolder);
	m_cube3.m_position = glm::vec3(10.0f, 1.5f, -10.0f);
	m_cube3.m_meshes[0].m_shininess = 32.0f;
	m_cube3.m_meshes[0].SetDiffuseMapTextureID(m_textureManager.GetTexture("Textures/container2.png"));
	m_cube3.m_meshes[0].SetSpecularMapTextureID(m_textureManager.GetTexture("Textures/container2_specular.png"));
	m_cube3.m_scale = glm::vec3(3.0f);

	Model& m_skybox = AddModel();
	m_skybox.AddMesh(dataHolder);
	m_skybox.m_scale = glm::vec3(1000.0f);
	m_skybox.m_isShadowCaster = false;
	m_skybox.m_meshes[0].SetCubeMapTextureID(m_textureManager.GetTextureCubemap("Lake", ".jpg"));
	m_skybox.m_isCenteredOnPlayer = true;

	dataHolder = ProceduralGenerator::Generate2DScreenQuad();
	Model& m_cubeTransparent = AddModel();
	m_cubeTransparent.AddMesh(dataHolder);
	m_cubeTransparent.AddMesh(dataHolder);
	m_cubeTransparent.AddMesh(dataHolder);
	m_cubeTransparent.AddMesh(dataHolder);
	m_cubeTransparent.AddMesh(dataHolder);
	m_cubeTransparent.AddMesh(dataHolder);
	for (Mesh& mesh : m_cubeTransparent.m_meshes) {
		mesh.SetDiffuseMapTextureID(m_textureManager.GetTexture("Textures/blending_transparent_window.png"));
		mesh.SetSpecularMapTextureID(m_textureManager.GetTexture("Textures/blending_transparent_window.png"));
		mesh.m_shininess = 32.0f;
	}

	m_cubeTransparent.m_position = glm::vec3(-15.0f, 9.0f, 15.0f);
	m_cubeTransparent.m_meshes[0].m_position = glm::vec3(0.0f, 0.0f, 1.0f);
	m_cubeTransparent.m_meshes[1].m_position = glm::vec3(0.0f, 0.0f, -1.0f);
	m_cubeTransparent.m_meshes[2].m_position = glm::vec3(1.0f, 0.0f, 0.0f);
	m_cubeTransparent.m_meshes[3].m_position = glm::vec3(-1.0f, 0.0f, 0.0f);
	m_cubeTransparent.m_meshes[4].m_position = glm::vec3(0.0f, 1.0f, 0.0f);
	m_cubeTransparent.m_meshes[5].m_position = glm::vec3(0.0f, -1.0f, 0.0f);
	m_cubeTransparent.m_meshes[1].m_rotationYDeg = 180.0f;
	m_cubeTransparent.m_meshes[2].m_rotationYDeg = 90.0f;
	m_cubeTransparent.m_meshes[3].m_rotationYDeg = -90.0f;
	m_cubeTransparent.m_meshes[4].m_rotationXDeg = -90.0f;
	m_cubeTransparent.m_meshes[5].m_rotationXDeg = 90.0f;
	m_cubeTransparent.m_scale = glm::vec3(3.0f);
	m_cubeTransparent.m_isTransparentModel = true;
	m_cubeTransparent.m_rotationYDeg = 45.0f;
	m_cubeTransparent.m_rotationXDeg = 45.0f;

	dataHolder = ProceduralGenerator::GenerateSphereModel();
	Model& m_sphereReflection = AddModel();
	m_sphereReflection.AddMesh(dataHolder);
	m_sphereReflection.m_position = glm::vec3(6.0f, 10.0f, -20.0f);
	m_sphereReflection.m_meshes[0].m_shininess = 32.0f;
	m_sphereReflection.m_scale = glm::vec3(3.0f);
	m_sphereReflection.m_isReflectionMapped = true;

	Model& m_sphereRefraction = AddModel();
	m_sphereRefraction.AddMesh(dataHolder);
	m_sphereRefraction.m_position = glm::vec3(-6.0f, 10.0f, -20.0f);
	m_sphereRefraction.m_meshes[0].m_shininess = 32.0f;
	m_sphereRefraction.m_scale = glm::vec3(3.0f);
	m_sphereRefraction.m_isRefractionMapped = true;

	Model& m_sphere = AddModel();
	m_sphere.AddMesh(dataHolder);
	m_sphere.m_position = glm::vec3(-2.0f, 4.0f, -1.0f);
	m_sphere.m_meshes[0].SetColor(glm::vec3(0.0f, 1.0f, 1.0f));
	m_sphere.m_scale = glm::vec3(1.0f);

	Model& m_bigWhiteSphere = AddModel();
	m_bigWhiteSphere.AddMesh(dataHolder);
	m_bigWhiteSphere.m_position = glm::vec3(-15.0f, 8.0f, 0.0f);
	m_bigWhiteSphere.m_meshes[0].m_shininess = 32.0f;
	m_bigWhiteSphere.m_scale = glm::vec3(4.0f);

	dataHolder = ProceduralGenerator::Generate2DScreenQuad();
	Model& m_windowModel = AddModel();
	for (int i = 0; i < 4; i++) {
		m_windowModel.AddMesh(dataHolder);
		float shift = -5.0f * i;
		m_windowModel.m_meshes[i].m_position = glm::vec3(0.0f, 0.0f, shift);
		m_windowModel.m_meshes[i].SetDiffuseMapTextureID(m_textureManager.GetTexture("Textures/blending_transparent_window.png"));
	}
	m_windowModel.m_scale = glm::vec3(5.0f, 5.0f, 1.0f);
	m_windowModel.m_position = glm::vec3(-30.0f, 5.0f, 30.0f);
	m_windowModel.m_rotationYDeg = 135.0f;
	m_windowModel.m_isTransparentModel = true;

	dataHolder = ProceduralGenerator::Generate2DScreenQuad();
	Model& m_vegetationModel = AddModel();
	for (int i = 0; i < 4; i++) {
		m_vegetationModel.AddMesh(dataHolder);
		float shift = -5.0f * i;
		m_vegetationModel.m_meshes[i].m_position = glm::vec3(0.0f, 0.0f, shift);
		m_vegetationModel.m_meshes[i].SetDiffuseMapTextureID(m_textureManager.GetTexture("Textures/grass.png", false));
		m_vegetationModel.m_meshes[i].SetSpecularMapTextureID(m_textureManager.GetTexture("Textures/grass.png", false));
	}
	m_vegetationModel.m_scale = glm::vec3(1.0f, 0.0f, 1.0f);
	m_vegetationModel.m_position = glm::vec3(30.0f, 0.0f, 30.0f);
	m_vegetationModel.m_rotationYDeg = 45.0f;
	m_vegetationModel.m_isShadowCaster = false;

	Model& m_farmhouse = AddModel();
	m_farmhouse.LoadModel("obj_models/farmhouse/farmhouse_obj.obj");
	m_farmhouse.m_position = glm::vec3(0.0f, 0.0f, 70.0f);

	Model& nanosuiteModel = AddModel();
	nanosuiteModel.LoadModel("obj_models/nanosuit/nanosuit.obj");
	nanosuiteModel.m_position = glm::vec3(15.0f, 0.0f, 0.0f);
	nanosuiteModel.m_isGetAlphaFromDiffuseMap = false;
	nanosuiteModel.m_rotationYDeg = 25.0f;

	Model& nanosuiteModel2 = AddModel();
	nanosuiteModel2.LoadModel(nanosuiteModel);
	nanosuiteModel2.m_position = glm::vec3(30.0f, 0.0f, 0.0f);
	nanosuiteModel2.m_isGetAlphaFromDiffuseMap = false;
	nanosuiteModel2.m_isReflectionMapped = true;
	nanosuiteModel2.m_rotationYDeg = -15.0f;

	Model& nanosuiteModel3 = AddModel();
	nanosuiteModel3.LoadModel(nanosuiteModel);
	nanosuiteModel3.m_position = glm::vec3(40.0f, 0.0f, 10.0f);
	nanosuiteModel3.m_rotationYDeg = -75.0f;
	nanosuiteModel3.m_isGetAlphaFromDiffuseMap = false;
	nanosuiteModel3.m_isRefractionMapped = true;

	glCheckError();
}

void GraphicsScene::GameLoop() {
	m_timeBefore = SDL_GetTicks();

	while (m_gameState != GameState::EXIT) {
		ProcessInput();
		AnimateModels();
		RenderScene();
	}
}

void GraphicsScene::ProcessInput() {
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			m_gameState = GameState::EXIT;
			break;
		case SDL_WINDOWEVENT:
			m_windowSpecs.HandleWindowEvent(sdlEvent.window);
			break;
		case SDL_MOUSEMOTION:
			m_mouse.HandleMouseMotion(sdlEvent.motion);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_mouse.HandleMouseButtonDown(sdlEvent.button);
			break;
		case SDL_MOUSEBUTTONUP:
			m_mouse.HandleMouseButtonUp(sdlEvent.button);
			break;
		case SDL_KEYDOWN:
			m_keyboard.HandleKeyPress(sdlEvent.key.keysym.sym);

			if (sdlEvent.key.keysym.sym == SDLK_r) {
				m_glslPostprocessingProgram.Recompile();
				m_glslLightingProgram.Recompile();
				m_glslNormalsProgram.Recompile();
			}
			else if (sdlEvent.key.keysym.sym == SDLK_n) {
				m_isRenderNormals = !m_isRenderNormals;
			}

			if (sdlEvent.key.keysym.sym == SDLK_1) {
				m_pLightTracedByCamera = nullptr;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_2) {
				m_pLightTracedByCamera = &m_pointLight;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_3) {
				m_pLightTracedByCamera = &m_pointLightAux;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_4) {
				m_pLightTracedByCamera = &m_spotLight;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_5) {
				m_pLightTracedByCamera = &m_hemisphereLight;
			}
			else if (sdlEvent.key.keysym.sym == SDLK_6) {
				m_pLightTracedByCamera = &m_directionalLight;
			}

			if (m_pLightTracedByCamera != nullptr) {
				if (m_pLightTracedByCamera->GetShadowCamera() != nullptr) {
					m_cameraLight.SetPosition(m_pLightTracedByCamera->GetShadowCamera()->GetPosition());
					m_cameraLight.SetLookAt(m_pLightTracedByCamera->GetShadowCamera()->GetLookAt());
				}
				else {
					m_cameraLight.SetPosition(m_pLightTracedByCamera->GetPosition());
					m_cameraLight.SetLookAt(m_pLightTracedByCamera->GetDirection());
				}
				m_pCurCamera = &m_cameraLight;
			}
			else {
				m_pCurCamera = &m_camera;
			}

			//std::cout << "Key Down" << std::endl;
			break;
		case SDL_KEYUP:
			m_keyboard.HandleKeyRelease(sdlEvent.key.keysym.sym);
			//std::cout << "Key Up" << std::endl;
			break;
		}
	}
}

void GraphicsScene::AnimateModels() {
	double delta = SDL_GetTicks() - m_timeBefore;
	m_timeBefore += delta;
	delta /= 1000.0f; //Convert to seconds

	m_keyboard.ApplyKeys();

	m_pCurCamera->Move(delta);

	if (m_mouse.IsMouseLeftButtonPressed() || m_mouse.IsMouseRightButtonPressed()) {
		glm::vec3 mouseNDCCoordsPress = glm::vec3(m_windowSpecs.ConvertWindowCoordToNDC(m_mouse.GetMousePosOnLastMainButtonPress()), 0);
		glm::vec3 mouseNDCCoordsCur = glm::vec3(m_windowSpecs.ConvertWindowCoordToNDC(m_mouse.GetMousePosCurrent()), 0);

		m_pCurCamera->ApplyMouseControl(delta, mouseNDCCoordsPress, mouseNDCCoordsCur);
	}
	m_pCurCamera->Update();

	if (m_pLightTracedByCamera != nullptr) {
		m_pLightTracedByCamera->SetPosition(m_cameraLight.GetPosition());
		m_pLightTracedByCamera->SetDirection(m_cameraLight.GetLookAt());
	}
	Light::SetCamera(m_pCurCamera);
}

void GraphicsScene::RenderScene() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_glslLightingProgram.IsUniformPresent("time")) {
		m_glslLightingProgram.UseProgram();
		GLint timeLoc = m_glslLightingProgram.GetUniformLocation("time");
		glUniform1f(timeLoc, m_timeBefore);
		m_glslLightingProgram.UnuseProgram();
	}

	// =================== Shadow Map Updates ===================
	m_renderer.RenderShadowMaps(m_modelsOpaque, m_lights);

	m_renderer.BindOffscreenFramebuffer();
	m_renderer.ClearFramebuffer();

	// ============== Apply Lighting ================
	m_renderer.ApplyLighting(m_lights);

	// ============ Opaque Models Drawing ==============
	for (Model* model : m_modelsOpaque) {
		if (model->m_isReflectionMapped) {
			m_renderer.RenderLighting(model, m_pCurCamera, nullptr, m_textureCubeMapEnvironmentMapID);
		}
		else {
			m_renderer.RenderLighting(model, m_pCurCamera);
		}
	}

	// ============ Transparent Sorted Models Drawing ==============
	m_sortedMeshes.clear();

	for (Model* model : m_modelsTransparent) {
		for (Mesh& mesh : model->m_meshes) {
			glm::vec3 meshPos =
				model->GetModelMatrix() *
				glm::vec4(mesh.m_position, 1.0f);
			float distance = glm::length(m_pCurCamera->GetPosition() - meshPos);
			m_sortedMeshes[distance] = { model, &mesh };
		}
	}

	std::vector<std::pair<Model*, Mesh*>> vectorSorted;
	for (auto itR = m_sortedMeshes.rbegin(); itR != m_sortedMeshes.rend(); itR++) {
		vectorSorted.emplace_back(itR->second.first, itR->second.second);
	}

	m_renderer.RenderLighting(nullptr, m_pCurCamera, &vectorSorted);

	// ============== Apply Object Highlights ================
	m_renderer.ApplyHighlights(m_models, m_pCurCamera);

	// ============== Non-Lighting Drawing ============
	m_renderer.RenderLightBulbs(m_lights, m_pCurCamera);
	m_renderer.RenderAxes(m_pCurCamera);

	if (m_isRenderNormals) {
		glLineWidth(5.0f);
		m_renderer.RenderNormals(m_models, m_pCurCamera);
	}


	// =============== 2D GUI Drawing ==============
	m_renderer.RenderMouseOverlay(m_mouse);

	// =============== Scene Display ==============
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_renderer.BindDefaultFramebuffer();
	m_renderer.ClearFramebuffer();
	m_renderer.DisplayOffscreenFramebuffer();

	glFlush();
	SDL_GL_SwapWindow(m_pSDLWindow);

	glCheckError();
}

