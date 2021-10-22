#version 330

// Texture coordinates input from vertex shader
in vec2 fragTexCoord;
// Normal position
in vec3 fragNormal;
// Position in world space
in vec3 fragWorldPos;

// specify the output of the fragment shader
// in this case, a color for each pixel inside the polygon (vector4 - RBGA)
out vec4 outColor;
// For texture sampling
// get the color from a texture given a UV coord
// we bind one texture at a time, so we don't need to pass uniform data from code. GLSL know which texture use
uniform sampler2D uTexture;

// Struct for directional light
struct DirectionalLight{
    // Direction of light
    vec3 mDirection;
    // Diffuse color
    vec3 mDiffuseColor;
    // Specular color
    vec3 mSpecularColor;
};

// uniforms for lighting
// Camera position in world space
uniform vec3 uCameraPos;
// Ambient light level
uniform vec3 uAmbientLight;
// Specular power of this surface
uniform float uSpecPower;
// Directional light (only one for now)
uniform DirectionalLight uDirLight;

void main(){
    // Surface normal
    vec3 N = normalize(fragNormal);
    // Vector from surface to light
    vec3 L = normalize(-uDirLight.mDirection);
    // Vector form surface to camera
    vec3 V = normalize(uCameraPos - fragWorldPos);
    // Reflection of -L about N
    vec3 R = normalize(reflect(-L, N));

    // Compute phong reflection
    vec3 Phong = uAmbientLight;
    float NdotL = dot(N, L);
    if(NdotL > 0){
        vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
        vec3 Specular = uDirLight.mSpecularColor * pow(max(0.0, dot(R, V)), uSpecPower);
        Phong += Diffuse + Specular;
    }

    // Final color is texture color times phong light (alpha = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}