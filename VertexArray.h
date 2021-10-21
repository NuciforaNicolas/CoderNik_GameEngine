#pragma once

class VertexArray {
public:
	VertexArray(const float* verts, const unsigned int numVerts,
		const unsigned int* indices, const unsigned int numIndices);
	~VertexArray();

	// Activate this Vertex Array so we can draw it
	void SetActive();

	// Getters
	unsigned int GetNumIndices() const { return mNumIndices; }
	unsigned int GetNumVerts() const { return mNumVerts; }

private:
	// How many vertex in the vertex buffer?
	unsigned int mNumVerts;
	// How many indeces in the index buffer?
	unsigned int mNumIndices;
	// OpenGL ID of the vertex buffer (used by OpenGL to reference to vertex buffer)
	unsigned int mVertexBuffer;
	// OpenGL ID of the indices buffer (used by OpenGL to reference to indices buffer)
	unsigned int mIndexBuffer;
	// OpenGL ID of the vertex array object (used by OpenGL to create, bind, etc..)
	unsigned int mVertexArray;
};