#version 460 core

out vec4 FragColor;

uniform int resolution;  // size (px) of each square
uniform sampler2D concentration;

void main() {
  ivec2 gridSize = textureSize(concentration, 0);
  ivec2 cell = ivec2(gl_FragCoord.xy / float(resolution));
  cell = clamp(cell, ivec2(0), gridSize - ivec2(1));

  float conc = texelFetch(concentration, cell, 0).r;

  // vec3 color = vec3(0.0);
  // if (square_y % 2 == 0) {
  //   color = square_x % 2 == 0 ? vec3(0.0) : vec3(1.0);
  // } else {
  //   color = square_x % 2 == 0 ? vec3(1.0) : vec3(0.0);
  // }

  FragColor = vec4(vec3(conc), 1.0);
}
