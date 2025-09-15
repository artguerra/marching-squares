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

  void computeConcentrations(f32 delta_t);

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
    m_gridWidth  = m_windowWidth  / m_resolution;
    m_gridHeight = m_windowHeight / m_resolution;

    u_conc.assign(m_gridWidth * m_gridHeight, 1.0f);
    v_conc.assign(m_gridWidth * m_gridHeight, 0.0f);

    auto idx = [&](int x,int y){ return y*m_gridWidth + x; };

    // seed a small square in the center
    int cx = m_gridWidth  / 2;
    int cy = m_gridHeight / 2;
    for (int y = cy-4; y <= cy+4; ++y)
      for (int x = cx-4; x <= cx+4; ++x) {
        v_conc[idx(x, y)] = 1.2f;   // 0.2–0.5 works well
        u_conc[idx(x, y)] = 0.50f;   // optional dip in u speeds things up
      }
  }

  static constexpr char VERTEX_SHADER_PATH[] = "shaders/grid.vert";
  static constexpr char FRAGMENT_SHADER_PATH[] = "shaders/grid.frag";
};

#endif  // __APPLICATION_H__
