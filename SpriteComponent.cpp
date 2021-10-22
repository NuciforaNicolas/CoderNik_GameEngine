#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Texture.h"
#include "Shader.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder) :
	Component(owner),
	mTexture(nullptr),
	mWidth(0),
	mHeight(0),
	mDrawOrder(drawOrder){
	// add itself to the sprite list
	mOwner->GetGame()->GetRenderer()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
	// remove itself from sprite list
	mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader) {
	if (mTexture) {
		// Create a scale matrix to scale by the width and the height of the texture
		Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(mWidth), static_cast<float>(mHeight), 1.0f);
		// Create the world transform matrix for the sprite using owner's world transform
		Matrix4 worldMat = scaleMat * mOwner->GetWorldTransform();
		// Set world transform
		shader->SetMatrixUniform("uWorldTransform", worldMat);

		// Active current texture
		mTexture->SetActive();

		// Draw a sprite using triangles as polygons
		// This function requires both an active vertex array object and shader program
		glDrawElements(
			GL_TRIANGLES,		// Type of polygon to draw
			6,					// Number of indices in index buffer
			GL_UNSIGNED_INT,	// value type of indices
			nullptr				// usually nullptr
		);
	}
}

void SpriteComponent::SetTexture(Texture* texture) {
	mTexture = texture;
	// query width and height of the texture
	mWidth = texture->GetWidth();
	mHeight = texture->GetHeight();
}