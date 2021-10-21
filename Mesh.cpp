#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL_log.h>
#include "Math.h"
#include "Renderer.h"

Mesh::Mesh() :
	mVertexArray(nullptr),
	mRadius(0.f),
	mSpecPower(100.f)
{}

Mesh::~Mesh(){}

bool Mesh::Load(const std::string& fileName, Renderer* renderer) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		SDL_Log("Failed to open file: %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	// Parse string stream to json
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject()) {
		SDL_Log("Mesh %s is not a valid json", fileName.c_str());
		return false;
	}

	// Get mesh version
	int version = doc["version"].GetInt();
	// Check the version
	if (version != 1) {
		SDL_Log("Mesh %s is not version 1", fileName.c_str());
		return false;
	}

	// get shader name
	mShaderName = doc["shader"].GetString();

	// TO-DO: specify vertex format/shader

	// Size for each vertex
	size_t vertSize = 8;

	// load textures
	const rapidjson::Value& textures = doc["textures"];
	if (!textures.IsArray() || textures.Size() < 1) {
		SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}

	// Get specular power
	mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

	// Load textures
	for (rapidjson::SizeType i = 0; i < textures.Size(); i++) {
		// is this texture already loaded?
		std::string texName = textures[i].GetString();
		Texture* tex = renderer->GetTexture(texName);
		if (tex == nullptr) {
			// Try loading texture
			tex = renderer->GetTexture(texName);
			if (tex == nullptr) {
				// if still null, use the default texture
				tex = renderer->GetTexture("Assets/Textures/Default.png");
			}
		}
		mTextures.emplace_back(tex);
	}

	// Load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1) {
		SDL_Log("Mesh %s has no vertices", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);
	mRadius = 0.f;
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++) {
		// For now assume 8 elements
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray() || vert.Size() != 8) {
			SDL_Log("Unexpected vertexx format for %s", fileName.c_str());
			return false;
		}

		// Extract vertex position
		Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
		mRadius = Math::Max(mRadius, pos.LengthSq());

		// Add vertices
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++) {
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
		}
	}
	// compute lenght squared
	mRadius = Math::Sqrt(mRadius);

	// Load in the indices
	const rapidjson::Value& indJson = doc["indices"];
	if (!indJson.IsArray() || indJson.Size() < 1) {
		SDL_Log("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3);
	for (rapidjson::SizeType i = 0; i < indJson.Size(); i++) {
		const rapidjson::Value& ind = indJson[i];
		if (!ind.IsArray() || ind.Size() != 3) {
			SDL_Log("Invalid indices for %s", fileName.c_str());
			return false;
		}

		indices.emplace_back(ind[0].GetUint());
		indices.emplace_back(ind[1].GetUint());
		indices.emplace_back(ind[2].GetUint());
	}

	// Now create a vertex array
	mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
		indices.data(), static_cast<unsigned>(indices.size()));
		
	return true;
}

void Mesh::Unload() {
	delete mVertexArray;
	mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index) {
	if (index < mTextures.size()) {
		return mTextures[index];
	}
	else {
		return nullptr;
	}
}

