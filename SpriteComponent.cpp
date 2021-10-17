#include "SpriteComponent.h"
#include "Actor.h"
SpriteComponent::SpriteComponent(Actor* owner, int drawOrder) :
	Component(owner),
	mTexture(nullptr),
	mTextureWidth(0),
	mTextureHeight(0){
	// add itself to the sprite list
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
	// remove itself from sprite list
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer) {

}

void SpriteComponent::SetTexture(SDL_Texture* texture) {
	mTexture = texture;
	// query width and height of the texture
	SDL_QueryTexture(texture, nullptr, nullptr, &mTextureWidth, &mTextureHeight);
}