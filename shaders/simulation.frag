#version 460 core

layout(location = 0) out vec2 outUV;

uniform sampler2D concentrationTex;
uniform float F, k, Du, Dv;

uniform bool isDraggingMouse;
uniform vec2 mousePos;
uniform float brushRadius;

ivec2 wrap(ivec2 p, ivec2 sz) {
  return ivec2((p.x + sz.x) % sz.x, (p.y + sz.y) % sz.y);
}

float V(ivec2 p) { return texelFetch(concentrationTex, p, 0).r; }

float U(ivec2 p) { return texelFetch(concentrationTex, p, 0).g; }

void main() {
  ivec2 p = ivec2(gl_FragCoord.xy);
  ivec2 sz = textureSize(concentrationTex, 0);

  if (isDraggingMouse && distance(p, mousePos) <= brushRadius) {
    outUV = vec2(1.0, 0.0);
    return;
  }

  float u = U(p);
  float v = V(p);

  // neighbors
  ivec2 left = wrap(p + ivec2(-1, 0), sz);
  ivec2 right = wrap(p + ivec2(1, 0), sz);
  ivec2 down = wrap(p + ivec2(0, -1), sz);
  ivec2 up = wrap(p + ivec2(0, 1), sz);

  float u_lapl = U(left) + U(right) + U(down) + U(up) - 4 * u;
  float v_lapl = V(left) + V(right) + V(down) + V(up) - 4 * v;

  float du = -(u * v * v) + F * (1 - u) + Du * u_lapl;
  float dv = (u * v * v) - (F + k) * v + Dv * v_lapl;

  outUV = vec2(max(v + dv, 0.0), max(u + du, 0.0));
}

