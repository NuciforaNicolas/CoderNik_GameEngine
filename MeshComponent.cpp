#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner) :
	Component(owner),
	mMesh(nullptr),
	mTextureIndex(0){
	owner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent(){
	mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader) {
	if (mMesh) {
		// Set the world transform
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
		// Set the specular uniform
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// Set the active texture
		Texture* tex = mMesh->GetTexture(mTextureIndex);
		if (tex) tex->SetActive();
		// Set the mesh's vertex array object as active
		VertexArray* vao = mMesh->GetVertexArray();
		if (vao) vao->SetActive();
		// Draw mesh
		glDrawElements(GL_TRIANGLES, vao->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void MeshComponent::SetMesh(Mesh* mesh) {
	mMesh = mesh;
}

void MeshComponent::SetTextureIndex(size_t index) {
	mTextureIndex = index;
}