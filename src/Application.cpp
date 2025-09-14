#include "Application.h"

#include <imgui.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

constexpr float VERTICES[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,
};

constexpr uint32_t INDICES[] = {
    0, 1, 3, 1, 2, 3,
};

void Application::render() {
  if (!m_buffersInitializated) initBuffers();

  m_mainShader.use();
  m_mainShader.setVec2("viewportDimensions", m_windowWidth, m_windowHeight);
  m_mainShader.setVec2("center", glm::vec2(0.0f));
  m_mainShader.setFloat("viewSpan", 2.0f);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  ImGui::Begin("Hello, ImGui!");
  ImGui::Text("This is a window!");
  ImGui::End();
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

  m_buffersInitializated = true;
}

