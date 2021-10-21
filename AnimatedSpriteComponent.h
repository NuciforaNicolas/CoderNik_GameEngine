#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <map>
#include <string>

class AnimatedSpriteComponent : public SpriteComponent {
public:
	AnimatedSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame
	void Update(float deltatime) override;
	// Initialize the Animator textures map
	void InitializeAnimTextures(const std::map<std::string, std::vector<SDL_Texture*>> textures);
	// Set current Animation
	void SetCurrentAnimation(std::string animation);
	// Set/Get animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }

private:
	// All texture in the animation
	std::map<std::string, std::vector<SDL_Texture*>> mAnimTexMap;
	// Current animation
	std::vector<SDL_Texture*> mCurrentAnimation;
	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;
};