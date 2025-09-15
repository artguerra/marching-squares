#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <vector>

#include "Shader.h"
#include "types.h"

class Application {
 private:
  i32 m_windowWidth, m_windowHeight;
  u32 VAO, VBO, EBO;
  u32 m_concentrationTex;

  i32 m_resolution, m_gridWidth, m_gridHeight;

  std::vector<f32> u_conc;
  std::vector<f32> v_conc;

  bool m_buffersInitializated{false};
  Shader m_mainShader;

 public:
  Application(i32 width, i32 height, i32 res)
      : m_windowWidth{width},
        m_windowHeight{height},
        m_resolution{res},
        m_mainShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) {
    resetConcentrations();
  }

  void computeConcentrations();

  void render();

  void setWindowSize(i32 w, i32 h) {
    m_windowWidth = w;
    m_windowHeight = h;
    resetConcentrations();
  }

  void setResolution(i32 res) {
    m_resolution = res;
    resetConcentrations();
  }

 private:
  void initBuffers();
  void updateConcentrationTexture();

  void resetConcentrations() {
    m_gridWidth = m_windowWidth / m_resolution;
    m_gridHeight = m_windowHeight / m_resolution;

    u_conc.resize(m_gridWidth * m_gridHeight);
    v_conc.resize(m_gridWidth * m_gridHeight);
  }

  static constexpr char VERTEX_SHADER_PATH[] = "shaders/grid.vert";
  static constexpr char FRAGMENT_SHADER_PATH[] = "shaders/grid.frag";
};

#endif  // __APPLICATION_H__
