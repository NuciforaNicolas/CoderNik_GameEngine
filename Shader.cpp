#include <fstream>
#include <sstream>
#include "Shader.h"
#include "Texture.h"
#include <SDL.h>
#include <iostream>

Shader::Shader() :
	mVertexShader(0),
	mFragShader(0),
	mShaderProgram(0)
{}

Shader::~Shader(){}

bool Shader::CompileShader(const std::string& shaderName, GLenum shaderType, GLuint& outShader) {
	// Open the shader file
	std::ifstream shaderFile(shaderName);
	if (shaderFile.is_open()) {
		// Insert all the code inside the shader file into a string
		std::stringstream sstream;
		// read the file buffer
		sstream << shaderFile.rdbuf();
		// insert the sstream content inside a string
		std::string contents = sstream.str();
		// convert string to a pointer to char
		const char* contentsChar = contents.c_str();

		// create a shader of the specified type
		outShader = glCreateShader(shaderType);
		// puts the entire program inside the char array into the shader source and compile it
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		// check if compiled successfully
		if (!IsCompiled(outShader)) {
			SDL_Log("Failed to compile shader program %s", shaderName.c_str());
			return false;
		}
	}
	else {
		SDL_Log("Failed to open the shader file %s", shaderName.c_str());
		return false;
	}
	// Shader compiled successfully
	return true;
}

bool Shader::IsCompiled(GLuint shaderID) {
	// store the compilation status
	GLint status;
	// Query the compilation status of the specified shader and store the status
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char errMsg[512];
		memset(errMsg, 0, 512);
		glGetShaderInfoLog(shaderID, 511, nullptr, errMsg);
		SDL_Log("GLSL Compile failed: \n%s", errMsg);
		return false;
	}
	// Shader compiled successfully
	return true;
}

bool Shader::Load(const std::string& vertName, const std::string& fragName) {
	// Compile vertex and fragment shaders, stores theirs ID and check if compiled
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
		!CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader)) {
		return false;
	}

	// Create a program that link vertex and fragment shaders, and store the shader program ID
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// verify is program linked successfully
	if (!IsValidProgram()) return false;

	// Shader program created successfully
	return true;
}

void Shader::Unload() {
	// delete program and shaders
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

bool Shader::IsValidProgram() {
	// Store the linking status
	GLint status;
	// Query the linking status of the specified program
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char errMsg[512];
		memset(errMsg, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, errMsg);
		SDL_Log("GLSL Link Failed:\n%s", errMsg);
		return false;
	}
	// program linked successfully
	return true;
}

void Shader::SetActive() {
	// Use the specified shader program to draw polygons
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const std::string matrixName, const Matrix4& matrix) {
	// Find the uniform variable by its name
	GLuint uniformId;
	// Check if the ID was already obtained. Otherwise, create new ID and cache it
	if (mUniforms.find(matrixName) != mUniforms.end())
		uniformId = mUniforms.at(matrixName);
	else {
		uniformId = glGetUniformLocation(mShaderProgram, matrixName.c_str());
		mUniforms[matrixName] = uniformId;
	}

	// Send the matrix data to the uniform
	glUniformMatrix4fv(
		uniformId,				// Uniform ID
		1,						// Number of matrices
		GL_TRUE,				// Set to true if use row vector for multiplication
		matrix.GetAsFloatPtr()	// Pointer to the matrix data
	);
}

void Shader::SetVectorUniform(const std::string vectorName, const Vector3& vec) {
	// Find the uniform variable by its name
	GLuint uniformId;
	// Check if the ID was already obtained. Otherwise, create new ID and cache it
	if (mUniforms.find(vectorName) != mUniforms.end()) {
		uniformId = mUniforms.at(vectorName);
	}
	else {
		uniformId = glGetUniformLocation(mShaderProgram, vectorName.c_str());
		mUniforms[vectorName] = uniformId;
	}

	// Send the vector data to the uniform
	glUniform3fv(uniformId, 1, vec.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const std::string floatName, const float& flt) {
	// Find the uniform location by its name
	GLuint uniformId;
	if (mUniforms.find(floatName) != mUniforms.end()) {
		uniformId = mUniforms.at(floatName);
	}
	else {
		uniformId = glGetUniformLocation(mShaderProgram, floatName.c_str());
		mUniforms[floatName] = uniformId;
	}

	// Send the float data to the uniform
	glUniform1f(uniformId, flt);
}