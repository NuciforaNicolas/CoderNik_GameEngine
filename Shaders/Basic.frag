#version 330

// specify the output of the fragment shader
// in this case, a color for each pixel inside the polygon (vector4 - RBGA)
out vec4 outColor;

void main(){
    // set the output variable as a red color
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}