#include "VertexArray.h"
#include <glew.h>
VertexArray::VertexArray(const float* verts, unsigned int numVerts,
	const unsigned int* indices, unsigned int numIndices) :
	mNumIndices(numIndices),
	mNumVerts(numVerts)
{
	// First create the vertex array object and store its ID
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// Create vertex buffer and store its ID
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexArray); // GL_ARRAY_BUFFER specify that we want to use the buffer as a vertex buffer

	// Copy the vertex data passed into the VertexArray constructor into this vertex buffer
	glBufferData(
		GL_ARRAY_BUFFER,				// The Active buffer type to write to. It specify the buffer just created, so we don't need to specify buffer ID
		numVerts * 8 * sizeof(float),	// Number of bytes to copy. 8 bytes: 3 float (vertex position) + 3 float (normal position) + 2 float (UV position)
		verts,							// Source of vertex to copy
		GL_STATIC_DRAW					// Load the data once and use it frequently for drawing
	);

	// Create indices buffer and store its ID
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

	// Copy the index data passed into the VertexArray constructor into this index buffer
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,			// The active index buffer just created
		numIndices * sizeof(unsigned int),	// size of data
		indices,							// source of index to copy
		GL_STATIC_DRAW						// Load the data once and use it frequently for drawing
	);

	// Specify vertex layout (vertex attributes). Each vertex contains information (attributes) as the vertex position, vertex color, etc...
	// Enable first vertex attribute: vertex position
	glEnableVertexAttribArray(0); // 0 = first attribute
	// Specify size, tyoe and format of the attrribute
	glVertexAttribPointer(
		0,					// First attribute
		3,					// Number of components. 3 in this case (3 float for each vertex)
		GL_FLOAT,			//The type of the components. Each vertex has 3 float
		GL_FALSE,			// Only used for integer types
		sizeof(float) * 8,	// Size of each vertex (3 float for vertex position and 2 float for UV position)
		0					// Offset from start of vertex to this attribute (in this case, position of the vertex is the first attribute and don't need offset)
	);

	// Enable second vertex attribute: normal position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, // First attribute
		3, // Number of components. Normal has 3 float (x, y, z)
		GL_FLOAT, // Type of components
		GL_FALSE,
		sizeof(float) * 8, // Size of each vertex. Now vertex has 8 components. (x, y, z) (x, y, z) (u, v)
		reinterpret_cast<void*>(sizeof(float) * 3) // Offset to reach normal components: normal are after vertex position -> 3 float
	);

	// Enable second vertex attribute: UV coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // Second attribute
		2, // Number of components. 2 float for UV coordinates
		GL_FLOAT, // Type of the conponents
		GL_FALSE, // Used only for integers types
		sizeof(float) * 8, // Size of each vertex (3 floats for vertex position, 2 float for UV coordinates)
		reinterpret_cast<void*>(sizeof(float) * 6) // Offset to reach UV components. UV are after vertex + normal = 3 + 3 = 6 floats
	);
}

VertexArray::~VertexArray() {
	// Destroy vertex buffer, index buffer and vertex array object
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() {
	glBindVertexArray(mVertexArray);
}