#version 460 core

out vec4 FragColor;

uniform int resolution;  // size (px) of each square
uniform sampler2D concentration;

void main() {
  ivec2 gridSize = textureSize(concentration, 0);
  ivec2 cell = ivec2(gl_FragCoord.xy / float(resolution));
  cell = clamp(cell, ivec2(0), gridSize - ivec2(1));

  float conc = texelFetch(concentration, cell, 0).r;

  vec3 color = conc * vec3(0.0, 0.99, 0.98);

  FragColor = vec4(color, 1.0);
}
