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

struct PointLight{
    // Point light position
    vec3 mPosition;
    // Diffuse color
    vec3 mDiffuseColor;
    // Specular color
    vec3 mSpecularColor;
    // Specular power
    float mSpecPower;
    // radius of influence
    float mRadius;
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
// Array of point lights
uniform PointLight uPointLights[4];

vec3 CalcDirLight(DirectionalLight dirLight, vec3 fragPos ,vec3 normal, vec3 cameraPos, vec3 ambientLight ,float specPower){
    // Surface normal
	vec3 N = normalize(fragNormal);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	vec3 Phong;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecularColor * pow(max(0.0, dot(R, V)), uSpecPower);
		Phong = Diffuse + Specular;
	}
    return Phong;
} 

vec3 CalcPointLight(PointLight ptLight, vec3 fragPos ,vec3 normal, vec3 cameraPos, vec3 ambientLight ,float specPower){
    // Surface normal
    vec3 N = normalize(normal);
    // Vector from surface to light
    vec3 L = normalize(ptLight.mPosition - fragPos);
    // Vector form surface to camera
    vec3 V = normalize(cameraPos - fragPos);
    // Reflection of -L about N
    vec3 R = normalize(reflect(-L, N));
    
    vec3 Phong;
    float NdotL = dot(N, L);
    if(NdotL > 0 && (length(fragPos - ptLight.mPosition) <= ptLight.mRadius)){
        vec3 Diffuse = ptLight.mDiffuseColor * NdotL;
        vec3 Specular = ptLight.mSpecularColor * pow(max(0.0, dot(R, V)), specPower);
        Phong = Diffuse + Specular;
    }
    return Phong;
} 

void main(){
    // Compute phong reflection
    
    vec3 Phong = uAmbientLight;

    Phong += CalcDirLight(uDirLight, fragWorldPos, fragNormal, uCameraPos, uAmbientLight, uSpecPower);

    for(int i = 0; i < 4; i++){
        Phong += CalcPointLight(uPointLights[i], fragWorldPos, fragNormal, uCameraPos, uAmbientLight, uSpecPower);
    }

    // Final color is texture color times phong light (alpha = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}