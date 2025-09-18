#version 460 core

out vec4 FragColor;

uniform int resolution;  // size (px) of each square
uniform sampler2D concentration;

void main() {
  ivec2 gridSize = textureSize(concentration, 0);
  ivec2 cell = ivec2(gl_FragCoord.xy / float(resolution));
  cell = clamp(cell, ivec2(0), gridSize - ivec2(1));

  float conc = texelFetch(concentration, cell, 0).r;

  float t = smoothstep(0.02, 0.6, conc);
  float glow = pow(t, 0.75);

  vec3 c1 = vec3(0.00, 0.90, 1.00);
  vec3 c2 = vec3(1.00, 0.20, 0.90);
  vec3 albedo = mix(c1, c2, t);

  vec3 color = vec3(clamp(albedo * glow, 0.0, 1.0));

  FragColor = vec4(color, 1.0);
  // FragColor = vec4(vec3(conc), 1.0);
}
