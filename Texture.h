#pragma once
#include <string>

class Texture {
public:
	Texture();
	~Texture();

	// load the specified texture
	bool Load(const std::string& fileName);
	void Unload();

	void SetActive();

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	// OpenGL ID of this texture
	unsigned int mTextureID;
	// Width/Height of the texture
	int mWidth, mHeight;
};