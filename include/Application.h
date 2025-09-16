#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <vector>

#include "Profiler.h"
#include "Shader.h"
#include "types.h"

struct Preset {
  std::string name;
  f32 F, k;
};

class Application {
 private:
  // profiling
  Profiler& m_prof;

  // opengl variables
  u32 VAO, VBO, EBO;
  u32 m_concentrationTex;

  // screen parameters
  i32 m_windowWidth, m_windowHeight;
  i32 m_resolution, m_gridWidth, m_gridHeight;

  // simulation parameters
  i32 m_stepsPerFrame{8};
  f32 F{0.037f}, k{0.06f};

  // ui controls
  i32 m_currentPreset;

  // core gray-scott model vars
  std::vector<f32> u_conc;
  std::vector<f32> v_conc;

  // shaders
  bool m_buffersInitializated{false};
  Shader m_mainShader;

 public:
  Application(i32 width, i32 height, i32 res, Profiler& _profiler)
      : m_prof(_profiler),
        m_windowWidth{width},
        m_windowHeight{height},
        m_resolution{res},
        m_mainShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) {
    resetConcentrations();
  }

  void computeConcentrations(f32 delta_t);
  void render(bool drawUI);

  void handleMouseAction(f64 xpos, f64 ypos);

  void resetConcentrations() {
    m_prof.restart();

    m_gridWidth = m_windowWidth / m_resolution;
    m_gridHeight = m_windowHeight / m_resolution;

    u_conc.assign(m_gridWidth * m_gridHeight, 1.0f);
    v_conc.assign(m_gridWidth * m_gridHeight, 0.0f);
  }

  void setWindowSize(i32 w, i32 h) {
    m_windowWidth = w;
    m_windowHeight = h;
    resetConcentrations();
  }

  void setResolution(i32 res) {
    m_resolution = res;
    resetConcentrations();
  }

  void setParams(f32 _f, f32 _k) {
    F = _f;
    k = _k;
  }

  i32 getStepsPerFrame() { return m_stepsPerFrame; }

 private:
  void initBuffers();
  void updateConcentrationTexture();

  void renderUI();

  void presetFillCenter() {
    resetConcentrations();

    i32 cx = m_gridWidth / 2;
    i32 cy = m_gridHeight / 2;
    for (i32 y = cy - 4; y <= cy + 4; ++y)
      for (i32 x = cx - 4; x <= cx + 4; ++x) {
        v_conc[y * m_gridWidth + x] = 1.0f;
        u_conc[y * m_gridWidth + x] = 0.50f;
      }
  }

  static constexpr char VERTEX_SHADER_PATH[] = "shaders/grid.vert";
  static constexpr char FRAGMENT_SHADER_PATH[] = "shaders/grid.frag";
  const Preset PRESETS[6] = {
    Preset{"Mazes", 0.037f, 0.060f},
    Preset{"Worms", 0.078f, 0.061f},
    Preset{"Flower", 0.055f, 0.062f},
    Preset{"Waves", 0.014f, 0.045f},
    Preset{"Pulses", 0.025f, 0.060f},
    Preset{"Holes", 0.039f, 0.058f}
  };
};

#endif  // __APPLICATION_H__
