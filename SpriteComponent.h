#pragma once
#include "Component.h"
#include "SDL.h"

class SpriteComponent : public Component {
public:
	// Costructor/destructor
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	// draw sprite
	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);

	int GetDrawOrder() { return mDrawOrder; }
	int GetTextureWidth() { return mTextureWidth; }
	int GetTextureHeight() { return mTextureHeight; }

private:
	// texture to draw
	SDL_Texture* mTexture;
	// draw order
	int mDrawOrder;
	// dimensions of the texture
	int mTextureWidth, mTextureHeight;

};