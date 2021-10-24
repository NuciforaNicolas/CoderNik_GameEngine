#include "Texture.h"
#include <SOIL.h>
#include <SDL.h>
#include <glew.h>

Texture::Texture() :
	mTextureID(0),
	mWidth(0),
	mHeight(0){}

Texture::~Texture(){}

bool Texture::Load(const std::string& fileName) {
	// Number of color channel
	int channels = 0;
	// Load the texture
	unsigned char* image = SOIL_load_image(
		fileName.c_str(),	// Name of file
		&mWidth,			// Stores width
		&mHeight,			// Stores height
		&channels,			// Stores number of channels
		SOIL_LOAD_AUTO		// Specify type of image. Use auto if any
	);
	if (!image) {
		SDL_Log("Failed to load texture %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}
	// Set color format. Check channels: RGB = 3; RGBA = 4
	int format = GL_RGB;
	if (channels == 4) format = GL_RGBA;

	// Generate a openGL texture object and store its ID
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID); // GL_TEXTURE_2D is the most commot texture target

	// Copy the raw image data into the texture
	glTexImage2D(
		GL_TEXTURE_2D,		// Texture target
		0,					// Level of detail (assume 0 for now)
		format,				// Color format OpenGL should use (RGB/RGBA)
		mWidth,				// Width of texture
		mHeight,			// Height of texture
		0,					// Border - "MUST BE 0"
		format,				// Color format of input data
		GL_UNSIGNED_BYTE,	// Bit depth of input data. Unsigned byte specifies 8-bit channels
		image				// Pointer to image data
	);

	// Tell SOIL to free the image from memory once loaded
	SOIL_free_image_data(image);

	// Enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload() {
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(){
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}