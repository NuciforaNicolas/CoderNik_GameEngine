#include <glew.h>
#include "Renderer.h"
#include "Game.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

Renderer::Renderer(Game* game) :
	mGame(game)
{}

Renderer::~Renderer(){}

bool Renderer::Initialize(float screenWidth, float screenHeight) {
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Set OpneGL window's attribute
	// return 0 if successfull, otherwise negative value
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE
	);
	// Specify OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Request a z-buffer (depth buffer) of 24 bit
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration (GPU)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create a window with support to OpenGL
	mWindow = SDL_CreateWindow("CoderNik Game Engine", 100, 100, static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create OpenGL Context
	mContext = SDL_GL_CreateContext(mWindow);

	// Enable GLEW library. Automatically initialize all extension functions supported by the current OpenGL context's version (3.3 in this case)
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		SDL_Log("Failed to initialize GLEW: %s", SDL_GetError());
		return false;
	}
	// Clear benign error
	glGetError();

	// Call LoadShader after the initialization of OpenGL and GLEW and before creation of the vertex array object
	if (!LoadShaders()) {
		SDL_Log("Failed to load shaders");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	return true;
}

void Renderer::ShutDown() {
delete mSpriteVerts;
for (auto shader : mMeshShaders) {
	shader.second->Unload();
	delete shader.second;
}
SDL_GL_DeleteContext(mContext);
SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData() {
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// Destroy meshes
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw() {
	// Set the clear color (equivalent to SDL_SetRendererDrawColor of SDL): Red: 0-1; Green: 0-1; Blue: 0-1; Alpha: 0-1
	glClearColor(0.f, 0.3f, .5f, 1.f);
	// Clear the color buffer (equivalent to SDL_RenderClear of SDL) and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This parameter clear the buffer with the specified color and Depth Buffer

	// Draw meshes
	// Enable depth buffer and disable alpha blending when draw meshes 
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Disable alpha blending when using depth buffer
	glDisable(GL_BLEND);

	for (auto shader : mMeshShaders) {
		if (shader.first != "Sprite") {
			// Set the basic mesh shader active
			shader.second->SetActive();
			// Update view-projection matrix (is necessary to account, for example, camera moving)
			shader.second->SetMatrixUniform("uViewProj", mView * mProjection);
			if (shader.first == "PhongMesh") {
				SetLightUniforms(shader.second);
			}
			// Iterate and draw all mesh components grouped by shader type
			for (auto mc : mMeshComponents[shader.first]) {
				mc->Draw(shader.second);
			}
		}
	}

	// Draw sprites
	// Enable alpha blending and disable depth buffer when drawing sprites
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	// On each frime, active the vertex array object and shader before drawing any model (sprite in this case)
	// Active vertex array object and shader
	mMeshShaders["Sprite"]->SetActive();
	mSpriteVerts->SetActive();
	// Draw all sprites
	for (auto sprite : mSprites)
		sprite->Draw(mMeshShaders["Sprite"]);

	// Swap back and front buffer to render the scene (equivalent to SDL_RenderPresent of SDL)
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite) {
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite) {
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComp(std::string shader, MeshComponent* mesh)
{
	mMeshComponents[shader].emplace_back(mesh);
}

void Renderer::RemoveMeshComp(std::string shader, MeshComponent* mesh)
{
	if (mMeshComponents.find(shader) != mMeshComponents.end()) {
		auto it = std::find(mMeshComponents[shader].begin(), mMeshComponents[shader].end(), mesh);
		if (it != mMeshComponents[shader].end()) {
			mMeshComponents[shader].erase(it);
		}
	}
}

Texture* Renderer::GetTexture(const std::string& fileName) {
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
	Mesh* m = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			mMeshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadShaders()
{
	// Load all shaders
	std::string shadersPath(fs::current_path().string() + "\\Shaders");
	std::string vertName = "";
	std::string fragName = "";
	for (auto it = fs::directory_iterator(shadersPath); it != fs::directory_iterator(); it++) {
		std::string shadehName = it->path().stem().string();
		// first is a frag, second is a vert (alphabetical order)
		fragName = it->path().filename().string();
		vertName = (++it)->path().filename().string();
		Shader* sh = new Shader();
		if (!sh->Load("Shaders/" + vertName, "Shaders/" + fragName)) return false;
		mMeshShaders[shadehName] = sh;
	}
	
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,	// Camera position (eye)
		Vector3::UnitX, // Target position
		Vector3::UnitZ	// Up
	);
	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.f),	// Horizontal FOV
		mScreenWidth,			// width of view
		mScreenHeight,			// height of view
		25.f,					// near plane
		10000.f					// far plane
	);

	// Activate all shaders
	for (auto shader : mMeshShaders) {
		if (shader.first == "Sprite") {
			shader.second->SetActive();
			// Set the view-projection matrix
			Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
			shader.second->SetMatrixUniform("uViewProj", viewProj);
		}
		else {
			shader.second->SetActive();
			shader.second->SetMatrixUniform("uViewProj", mView * mProjection);
		}
	}
	return true;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader) {
	// Camera position is from inverted view
	// Inverting camera matrix, allow us to get camera position from the first row using GetTranslation()
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection", mDirectionalLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecularColor", mDirectionalLight.mSpecularColor);
	// Point lights
	const unsigned int numPointLights = 4;
	for (unsigned int i = 0; i < numPointLights; i++) {
		shader->SetVectorUniform("uPointLights[" + std::to_string(i) + "].mPosition", mPointLights[i].mPosition);
		shader->SetVectorUniform("uPointLights[" + std::to_string(i) + "].mDiffuseColor", mPointLights[i].mDiffuseColor);
		shader->SetVectorUniform("uPointLights[" + std::to_string(i) + "].mSpecularColor", mPointLights[i].mSpecularColor);
		shader->SetFloatUniform("uPointLights[" + std::to_string(i) + "].mSpecPower", mPointLights[i].mSpecPower);
		shader->SetFloatUniform("uPointLights[" + std::to_string(i) + "].mRadius", mPointLights[i].mRadius);
	}
}