#pragma once
#include "Component.h"
#include "SDL.h"
#include "Shader.h"
#include "Texture.h"

class SpriteComponent : public Component {
public:
	// Costructor/destructor
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	// draw sprite
	virtual void Draw(Shader* shader);
	virtual void SetTexture(Texture* texture);

	int GetDrawOrder() { return mDrawOrder; }
	int GetTextureWidth() { return mWidth; }
	int GetTextureHeight() { return mHeight; }

private:
	// texture to draw
	Texture* mTexture;
	// draw order
	int mDrawOrder;
	// dimensions of the texture
	int mWidth, mHeight;

};