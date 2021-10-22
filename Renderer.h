#pragma once
#include <SDL.h>
#include <unordered_map>
#include "Math.h"

// Struct for directional light (to pass as uniform to Phong.frag)
struct DirectionaLight {
	// Direction of light
	Vector3 mDirection;
	// Diffuse color
	Vector3 mDiffuseColor;
	// Specular color
	Vector3 mSpecularColor;
};

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

	// Add/Remove mesh components
	void AddMeshComp(class MeshComponent* mesh);
	void RemoveMeshComp(class MeshComponent* mesh);

	// Get a texture from map
	class Texture* GetTexture(const std::string& fileName);
	// Get a mesh from map
	class Mesh* GetMesh(const std::string& mesh);

	void SetViewMatrix(const Matrix4& view) { mView = view; }
	void SetAmbientLight(const Vector3& ambientLight) { mAmbientLight = ambientLight; }
	DirectionaLight& GetDirectionalLight() { return mDirectionalLight; }

private:
	// Load sprite shader program and active it
	bool LoadShaders();
	// Create a quad shader
	void CreateSpriteVerts();
	// Set light uniforms
	void SetLightUniforms(class Shader* shader);

	// map of textures
	std::unordered_map<std::string, class Texture*> mTextures;
	// map of meshes
	std::unordered_map<std::string, class Mesh*> mMeshes;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;
	// All the mesh components drawn
	std::vector<class MeshComponent*> mMeshComponents;

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

	// Light members
	Vector3 mAmbientLight;
	DirectionaLight mDirectionalLight;

	// Window created by SDL
	SDL_Window* mWindow;
	// OpenGL Context: this is the "world" of OpenGL that contains every item that OpengGl knows about
	// such as color buffer, images, model loaded and any other OpenGL objects
	SDL_GLContext mContext;
};