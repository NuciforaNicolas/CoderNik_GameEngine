#include "AnimatedSpriteComponent.h"
#include "Actor.h"
#include <map>
#include <vector>
#include <string>

AnimatedSpriteComponent::AnimatedSpriteComponent(Actor* owner, int drawOrder) :
	SpriteComponent(owner, drawOrder),
	mCurrFrame(0),
	mAnimFPS(0)
{}

void AnimatedSpriteComponent::InitializeAnimTextures(const std::map<std::string, std::vector<SDL_Texture*>> textures) {
	mAnimTexMap = textures;
	mCurrFrame = 0;
	SetTexture(mAnimTexMap.begin()->second[0]);
}

void AnimatedSpriteComponent::SetCurrentAnimation(std::string animation) {
	mCurrentAnimation = mAnimTexMap.at(animation);
}

void AnimatedSpriteComponent::Update(float deltatime) {
	SpriteComponent::Update(deltatime);

	// For now, considere first array only
	if (mCurrentAnimation.size() > 0) {
		// Update current frame based on frame rate and delta time
		mCurrFrame += mAnimFPS * deltatime;

		// Wrap current frame if needed. Make sure Curr frame is less then the number of textures
		while (mCurrFrame >= mCurrentAnimation.size()) mCurrFrame -= mCurrentAnimation.size();

		// Set the current texture
		SetTexture(mCurrentAnimation[static_cast<int>(mCurrFrame)]);
	}
}