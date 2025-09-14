#version 460 core

out vec4 FragColor;

uniform vec2 viewportDimensions;

uniform vec2 center;
uniform float viewSpan;

const float lineWidth = 0.01;

void main() {
  vec2 uv = (gl_FragCoord.xy / viewportDimensions.xy) * viewSpan + center;
  uv.y *= viewportDimensions.y / viewportDimensions.x;

  vec2 lineAA = fwidth(uv);
  vec2 gridUV = 1.0 - abs(fract(uv * 10) * 2 - 1.0);
  vec2 grid2 = smoothstep(lineWidth + lineAA, lineWidth - lineAA, gridUV);
  float grid = max(grid2.x, grid2.y);

  FragColor = vec4(vec3(grid) * 0.8, 1.0);
}
