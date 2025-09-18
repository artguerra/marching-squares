#include "Application.h"

#include <algorithm>
#include <imgui.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Profiler.h"
#include "types.h"

constexpr f32 VERTICES[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,
};

constexpr u32 INDICES[] = {
    0, 1, 3, 1, 2, 3,
};

void Application::render(bool drawUI) {
  {
    Profiler::Scope _s(m_prof, "GUI");
    if (drawUI) renderUI();
  }

  if (!m_defaultBuffersInitializated) initDefaultBuffers();

  if (m_isRunningOnGPU) {
    if (!m_gpuCompTexturesInitialized) initBuffersGPUComp();
    renderGPUComp();
  } else {
    if (!m_cpuCompTexturesInitialized) initBuffersCPUComp();
    renderCPUComp();
  }
}

// pure CPU computation render call
void Application::renderCPUComp() {
  m_mainShader.use();
  m_mainShader.setInt("resolution", m_resolution);

  {
    Profiler::Scope _s(m_prof, "Texture upload");
    updateConcentrationTexture();
  }

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Application::renderGPUComp() {
  for (i32 i = 0; i < m_stepsPerFrame; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_destTex, 0);

    glViewport(0, 0, m_gridWidth, m_gridHeight);

    m_gpuComputeShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_srcTex);

    m_gpuComputeShader.setFloat("F", F);
    m_gpuComputeShader.setFloat("k", k);
    m_gpuComputeShader.setFloat("Du", Du);
    m_gpuComputeShader.setFloat("Dv", Dv);
    m_gpuComputeShader.setBool("isDraggingMouse", false);
    m_gpuComputeShader.setVec2("mousePos", m_mousePosX, m_mousePosY);

    if (m_isDraggingMouse && !ImGui::GetIO().WantCaptureMouse) {
      m_gpuComputeShader.setBool("isDraggingMouse", true);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    std::swap(m_srcTex, m_destTex);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, m_windowWidth, m_windowHeight);

  m_mainShader.use();
  m_mainShader.setInt("resolution", m_resolution);

  glBindTexture(GL_TEXTURE_2D, m_srcTex);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(0);
}

void Application::renderUI() {
  ImGui::Begin("Simulation controls");
  ImGui::Text("Press 'P' to show/hide the profiler");
  ImGui::Text("Press 'I' to show/hide this UI");
  ImGui::Text(
      "Press 'G' to toggle GPU computation. Currently processing on: %s",
      m_isRunningOnGPU ? "GPU" : "CPU"
  );

  ImGui::SliderInt("N. of steps per frame", &m_stepsPerFrame, 1, 24);

  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  if (ImGui::BeginCombo("Interesting presets", nullptr, ImGuiComboFlags_NoPreview)) {
    for (i32 i = 0; i < IM_ARRAYSIZE(PRESETS); ++i) {
      bool selected = m_currentPreset == i;

      if (ImGui::Selectable(PRESETS[i].name.c_str(), selected)) {
        m_currentPreset = i;
        setParams(PRESETS[i].F, PRESETS[i].k);
      }

      if (selected) ImGui::SetItemDefaultFocus();
    }

    ImGui::EndCombo();
  }

  ImGui::SliderFloat("F (\"feed rate\")", &F, 0.01f, 0.09f);

  ImGui::SliderFloat("k (\"kill rate\")", &k, 0.04f, 0.07f);

  if (ImGui::Button("Reset simulation (R)")) resetConcentrations();

  ImGui::End();
}

void Application::computeConcentrationsCPU(f32 delta_t) {
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

void Application::handleMouseAction() {
  // dont do anything if imgui is using the mouse
  if (ImGui::GetIO().WantCaptureMouse) return;

  if (m_isRunningOnGPU) return;  // on gpu we handle clicks in the shader

  if (m_mousePosX > 0 && m_mousePosX <= m_gridWidth && m_mousePosY > 0 &&
      m_mousePosY <= m_gridHeight) {
    v_conc[m_mousePosY * m_gridWidth + m_mousePosX] = 1.0f;
  }
}

void Application::updateConcentrationTexture() {
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glTexSubImage2D(
      GL_TEXTURE_2D, 0, 0, 0, m_gridWidth, m_gridHeight, GL_RED, GL_FLOAT, v_conc.data()
  );
}

void Application::initDefaultBuffers() {
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

  glBindVertexArray(0);

  m_defaultBuffersInitializated = true;
}

void Application::initBuffersCPUComp() {
  // concentration texture
  glGenTextures(1, &m_concentrationTex);
  glBindTexture(GL_TEXTURE_2D, m_concentrationTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_gridWidth, m_gridHeight, 0, GL_RED, GL_FLOAT, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);

  m_cpuCompTexturesInitialized = true;
}

void Application::initBuffersGPUComp() {
  // create framebuffer
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // src texture
  glGenTextures(1, &m_srcTex);
  glBindTexture(GL_TEXTURE_2D, m_srcTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  std::vector<float> data(m_gridWidth * m_gridHeight * 2);
  for (int i = 0; i < m_gridWidth * m_gridHeight; ++i) {
    data[2 * i] = 0.0f;      // initialize u with 1s
    data[2 * i + 1] = 1.0f;  // initialize v with 0s
  }

  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RG32F, m_gridWidth, m_gridHeight, 0, GL_RG, GL_FLOAT, data.data()
  );

  // dest texture
  glGenTextures(1, &m_destTex);
  glBindTexture(GL_TEXTURE_2D, m_destTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_gridWidth, m_gridHeight, 0, GL_RG, GL_FLOAT, nullptr);

  // setup framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_destTex, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_gpuCompTexturesInitialized = true;
}

