#version 330

// Specify the vertex attributes for each vertex
// These attributes must match the attributes in the vertex array object created
// These are the input of the vertex shader

// input of shaders are marked with "in" keyword
// the position of the vertex (a vector3) (attribute 0). Because there is only one input, we don't specify the attribute position
// we can access vec3 attributes with dot notation: pos.x; pos.y; pos.z
in vec3 inPosition;

// the output of the shader are marked with "out" keyword. Some built-in variable can be used without declare them with out keyword
// Es. out vec4 color;

// A shader is a program, so it has a main function
void main(){
    // built-in position output
    // store the inPosition of the vertex in the output of the shader
    // it expects a vector4 (x, y, z, w). So, we convert inPosition from vec3 to vec4
    gl_Position = vec4(inPosition, 1.0); // first parameter: vec3 to convert; second parameter: the value of w componentg
}