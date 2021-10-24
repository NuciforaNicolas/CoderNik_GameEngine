#version 330

// Specify the vertex attributes for each vertex
// These attributes must match the attributes in the vertex array object created
// These are the input of the vertex shader

// the output of the shader are marked with "out" keyword. Some built-in variable can be used without declare them with out keyword
// Es. out vec4 color;

// Uniform variable. A uniform is a global variable that stays the same between different invocation of the shader program
// Define two uniform for each matrix: a world transform matrix e a view-projection matrix that convert world space to clip space
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// input of shaders are marked with "in" keyword
// Specify attribute position with layout(location=n))
// the position of the vertex (a vector3). Attribute 0
// we can access vec3 attributes with dot notation: pos.x; pos.y; pos.z
layout(location=0) in vec3 inPosition;
// Normal position
layout(location=1) in vec3 inNormal;
// UV coordinates. Attribute 1
layout(location=2) in vec2 inTexCoord;

// To pass information to fragment shader, use global out variable. The name must match to both vertex and fragment shaders
out vec2 fragTexCoord;
// Normal position in world space
out vec3 fragNormal;
// Position in world space
out vec3 fragWorldPos;

// A shader is a program, so it has a main function
void main(){
    // built-in position output
    // store the inPosition of the vertex in the output of the shader
    // it expects a vector4 (x, y, z, w). So, we convert inPosition from vec3 to vec4
    vec4 pos = vec4(inPosition, 1.0); // Position  in OBJECT SPACE
    pos = pos * uWorldTransform; // Convert vertex position from Object space to WORLD SPACE
    // Save world position
    fragWorldPos = pos.xyz;
    //vec4 clipPos = worldPos * uViewProj; // Convert World space to CLIP SPACE
    gl_Position = pos * uViewProj;

    // Transform normal to world space
    fragNormal = (vec4(inNormal, 0.0) * uWorldTransform).xyz;

    // Pass Texture coordinate to fragment shader
    fragTexCoord = inTexCoord;
}