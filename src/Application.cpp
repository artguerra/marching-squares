#include "Application.h"

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

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  ImGui::Begin("Hello, ImGui!");
  ImGui::Text("This is a window!");
  ImGui::End();
}

void Application::computeConcentrations() {
  u_conc[0] = 1.0f;

  updateConcentrationTexture();
}

void Application::updateConcentrationTexture() {
  std::vector<f32> normalized(u_conc.size());

  for (i32 i = 0; i < u_conc.size(); ++i) {
    f32 u = u_conc[i];
    f32 v = v_conc[i];

    if (u == 0 && v == 0)
      normalized[i] = 0;
    else
      normalized[i] = (u - v) / (u + v);
  }

  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glTexSubImage2D(
      GL_TEXTURE_2D, 0, 0, 0, m_gridWidth, m_gridHeight, GL_RED, GL_FLOAT, normalized.data()
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

  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_R32F, m_windowWidth, m_windowHeight, 0, GL_RED, GL_FLOAT, nullptr
  );

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);

  m_buffersInitializated = true;
}

