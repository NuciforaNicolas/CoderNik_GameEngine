#include <glew.h>
#include "Renderer.h"
#include "Game.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"

Renderer::Renderer(Game* game) :
	mGame(game),
	mSpriteShader(nullptr),
	mMeshShader(nullptr)
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
	mSpriteShader->Unload();
	delete mSpriteShader;
	mMeshShader->Unload();
	delete mMeshShader;
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
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	// Clear the color buffer (equivalent to SDL_RenderClear of SDL) and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This parameter clear the buffer with the specified color and Depth Buffer

	// Draw meshes
	// Enable depth buffer and disable alpha blending when draw meshes
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Disable alpha blending when using depth buffer
	glDisable(GL_BLEND);

	// Set the basic mesh shader active
	mMeshShader->SetActive();
	// Update view-projection matrix (is necessary to account, for example, camera moving)
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	for (auto mc : mMeshComponents)
		mc->Draw(mMeshShader);

	// Draw sprites
	// Enable alpha blending and disable depth buffer when drawing sprites
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	// On each frime, active the vertex array object and shader before drawing any model (sprite in this case)
	// Active vertex array object and shader
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	// Draw all sprites
	for (auto sprite : mSprites)
		sprite->Draw(mSpriteShader);

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

void Renderer::AddMeshComp(MeshComponent* mesh) {
	mMeshComponents.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh) {
	auto iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), mesh);
	if (iter != mMeshComponents.end())
		mMeshComponents.erase(iter);
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
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	// Create mesh shader
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag")) return false;
	mMeshShader->SetActive();
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
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

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