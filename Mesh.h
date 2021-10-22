#pragma once
#include <vector>
#include <string>

class Mesh {
public:
	Mesh();
	~Mesh();

	// Load mesh. Use game reference to get mesh texture from game's texture map
	bool Load(const std::string& fileName, class Renderer* renderer);
	// Unload mesh
	void Unload();
	// Get the vertex array associated with this mesh
	class VertexArray* GetVertexArray() const { return mVertexArray; }
	// Get a texture from i-position
	class Texture* GetTexture(size_t index);
	// Get name of shader
	const std::string& GetShaderName() const { return mShaderName; }
	// Get object space bounding sphere radius
	float GetRadius() const { return mRadius; }
	// Get specular power
	float GetSpecPower() const { return mSpecPower; }

private:
	// Textures associated with this mesh
	std::vector<class Texture*> mTextures;
	// Vertex array associated with this mesh
	class VertexArray* mVertexArray;
	// Name of shader specified by mesh
	std::string mShaderName;
	// Stores object space bounding sphere radius
	// The distance between the object space origin and the point farthest away from origin
	// Used for collision detection
	float mRadius;
	// Specular value
	float mSpecPower;
};