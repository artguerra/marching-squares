#include "Application.h"

#include <algorithm>
#include <imgui.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "types.h"

constexpr f32 VERTICES[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,
};

constexpr u32 INDICES[] = {
    0, 1, 3, 1, 2, 3,
};

void Application::render() {
  if (!m_buffersInitializated) initBuffers();

  m_mainShader.use();
  m_mainShader.setInt("resolution", m_resolution);

  updateConcentrationTexture();

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // ImGui::Begin("Hello, ImGui!");
  // ImGui::Text("This is a window!");
  // ImGui::End();
}

void Application::computeConcentrations(f32 delta_t) {
  const f32 F = 0.037f;
  const f32 k = 0.06f;
  const f32 Du = 0.16f, Dv = 0.08f;

  std::vector<f32> new_u(u_conc.size());
  std::vector<f32> new_v(v_conc.size());

  i32 w = m_gridWidth, h = m_gridHeight;

  auto idx = [&](i32 x, i32 y) { return y * w + x; };

  for (i32 i = 0; i < w; ++i) {
    for (i32 j = 0; j < h; ++j) {
      i32 cur_idx = idx(i, j);
      f32 u = u_conc[cur_idx];
      f32 v = v_conc[cur_idx];

      auto U = [&](i32 x, i32 y) { return u_conc[idx((x + w) % w, (y + h) % h)]; };
      auto V = [&](i32 x, i32 y) { return v_conc[idx((x + w) % w, (y + h) % h)]; };

      f32 u_lapl = U(i - 1, j) + U(i + 1, j) + U(i, j - 1) + U(i, j + 1) - 4 * U(i, j);
      f32 v_lapl = V(i - 1, j) + V(i + 1, j) + V(i, j - 1) + V(i, j + 1) - 4 * V(i, j);

      f32 du = -(u * v * v) + F * (1 - u) + Du * u_lapl;
      f32 dv = (u * v * v) - (F + k) * v + Dv * v_lapl;

      new_u[cur_idx] = std::max(u + du * delta_t, 0.0f);
      new_v[cur_idx] = std::max(v + dv * delta_t, 0.0f);
    }
  }

  u_conc = std::move(new_u);
  v_conc = std::move(new_v);
}

void Application::updateConcentrationTexture() {
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glTexSubImage2D(
      GL_TEXTURE_2D, 0, 0, 0, m_gridWidth, m_gridHeight, GL_RED, GL_FLOAT, v_conc.data()
  );
}

void Application::initBuffers() {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // concentration texture
  glGenTextures(1, &m_concentrationTex);
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_gridWidth, m_gridHeight, 0, GL_RED, GL_FLOAT, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);

  m_buffersInitializated = true;
}

