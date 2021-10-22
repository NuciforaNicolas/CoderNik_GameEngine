#pragma once
#include <string>
#include <glew.h>
#include "Math.h"
#include <map>
#include <string>

class Shader {
public:
	Shader();
	~Shader();

	// Load the vertex/fragment shaders with the given name
	bool Load(const std::string& vertName, const std::string& fragName);
	// Unload the shader program by deleting vertex/fragment shaders and shader program
	void Unload();
	// Set this shader as the active shader program
	void SetActive();
	// Set the uniform matrix
	void SetMatrixUniform(const std::string matrixName, const Matrix4& matrix);
	void SetVectorUniform(const std::string vectorName, const Vector3& vec);
	void SetFloatUniform(const std::string floatName, const float& flt);

private:
	// support method. Tries to compile the specified shader program
	bool CompileShader(const std::string& shaderName, GLenum shaderType, GLuint& outShader);
	// support method. Tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// support method. Tests wheter vertex/fragment programs link
	bool IsValidProgram();
	// Store the shader object IDs
	GLuint mVertexShader;
	GLuint mFragShader;
	// OpenGL use the current active shader program to render polygons
	GLuint mShaderProgram;
	// Map of uniform of the active shader
	std::map<std::string, GLuint> mUniforms;
};