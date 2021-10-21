#version 330

// Texture coordinates input from vertex shader
in vec2 fragTexCoord;
// specify the output of the fragment shader
// in this case, a color for each pixel inside the polygon (vector4 - RBGA)
out vec4 outColor;
// For texture sampling
// get the color from a texture given a UV coord
// we bind one texture at a time, so we don't need to pass uniform data from code. GLSL know which texture use
uniform sampler2D uTexture;

void main(){
    // sample color from texture
    outColor = texture(uTexture, fragTexCoord);
}