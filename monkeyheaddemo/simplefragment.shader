#version 330 core

// interpolated values from vertex shaders
in vec2 UV;

// output data
out vec3 color;

// Values that stay constant for whole mesh
uniform sampler2D myTextureSampler;

void main() {
  color = texture( myTextureSampler, UV ).rgb;
}
