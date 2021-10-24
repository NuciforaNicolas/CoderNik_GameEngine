#pragma once
#include "Component.h"
#include "Mesh.h"

class MeshComponent : public Component {
public:
	MeshComponent(class Actor* owner);
	~MeshComponent();
	// Draw this mesh with provided shader
	virtual void Draw(class Shader* shader);
	// Set mesh/texture index used by this mesh
	virtual void SetMesh(class Mesh* mesh);
	Mesh* GetMesh() const { return mMesh; }
	void SetTextureIndex(size_t index) { mTextureIndex = index; };
private:
	class Mesh* mMesh;
	size_t mTextureIndex;
};