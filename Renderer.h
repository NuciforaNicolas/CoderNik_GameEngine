#pragma once
#include <SDL.h>
#include <unordered_map>
#include "Math.h"

class Renderer {
public:
	Renderer(class Game* game);
	~Renderer();

	// Initialize and shutdown renderer
	bool Initialize(float screenWidth, float screenHeight);
	void ShutDown();

	// Unload all textures/meshes
	void UnloadData();

	// Draw the fram
	void Draw();

	// add sprite
	void AddSprite(class SpriteComponent* sprite);
	// remove sprite
	void RemoveSprite(class SpriteComponent* sprite);

	// Get a texture from map
	class Texture* GetTexture(const std::string& fileName);
	// Get a mesh from map
	class Mesh* GetMesh(const std::string& mesh);

	void SetViewMatrix(const Matrix4& view) { mView = view; }

private:
	// Load sprite shader program and active it
	bool LoadShaders();
	// Create a quad shader
	void CreateSpriteVerts();

	// map of textures
	std::unordered_map<std::string, class Texture*> mTextures;
	// map of meshes
	std::unordered_map<std::string, class Mesh*> mMeshes;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	// Game
	class Game* mGame;

	// Sprite shader
	class Shader* mSpriteShader;
	// Vertex array for sprites
	class VertexArray* mSpriteVerts;

	// Mesh shader
	class Shader* mMeshShader;

	// View/projection for 3D shaders
	Matrix4 mView;
	Matrix4 mProjection;
	// Width/height of screen
	float mScreenWidth;
	float mScreenHeight;

	// Window created by SDL
	SDL_Window* mWindow;
	// OpenGL Context: this is the "world" of OpenGL that contains every item that OpengGl knows about
	// such as color buffer, images, model loaded and any other OpenGL objects
	SDL_GLContext mContext;
};