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
  u32 FBO;                  // GPU computation
  u32 m_concentrationTex;   // CPU method
  u32 m_srcTex, m_destTex;  // GPU method

  // screen parameters
  i32 m_windowWidth, m_windowHeight;
  i32 m_resolution, m_gridWidth, m_gridHeight;

  // simulation parameters
  i32 m_stepsPerFrame{8};
  f32 F{0.037f}, k{0.06f};
  const f32 Du = 0.16f, Dv = 0.08f;

  // ui controls
  float m_brushRadius;
  bool m_isRunningOnGPU{true};
  i32 m_currentPreset;

  bool m_isDraggingMouse{false};
  i32 m_mousePosX, m_mousePosY;

  // core gray-scott model vars
  std::vector<f32> u_conc;
  std::vector<f32> v_conc;

  // shaders
  bool m_defaultBuffersInitializated{false};
  bool m_cpuCompTexturesInitialized{false};
  bool m_gpuCompTexturesInitialized{false};
  Shader m_mainShader;
  Shader m_gpuComputeShader;

 public:
  Application(i32 width, i32 height, i32 res, Profiler& _profiler)
      : m_prof(_profiler),
        m_windowWidth{width},
        m_windowHeight{height},
        m_resolution{res},
        m_brushRadius{std::min(1.0f, 10.0f / res)},
        m_mainShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH),
        m_gpuComputeShader(VERTEX_SHADER_PATH, SIM_SHADER_PATH) {
    recalculateGrid();
    resetConcentrations();
  }

  void computeConcentrationsCPU(f32 delta_t);
  void render(bool drawUI);

  void resetConcentrations() {
    m_prof.restart();

    // CPU computation
    u_conc.assign(m_gridWidth * m_gridHeight, 1.0f);
    v_conc.assign(m_gridWidth * m_gridHeight, 0.0f);

    // GPU computation
    if (m_gpuCompTexturesInitialized) {
      std::vector<float> data(m_gridWidth * m_gridHeight * 2);
      for (int i = 0; i < m_gridWidth * m_gridHeight; ++i) {
        data[2 * i] = 0.0f;      // initialize u with 1s
        data[2 * i + 1] = 1.0f;  // initialize v with 0s
      }

      glBindTexture(GL_TEXTURE_2D, m_srcTex);
      glTexSubImage2D(
          GL_TEXTURE_2D, 0, 0, 0, m_gridWidth, m_gridHeight, GL_RG, GL_FLOAT, data.data()
      );
    }
  }

  void recalculateGrid() {
    m_gridWidth = m_windowWidth / m_resolution;
    m_gridHeight = m_windowHeight / m_resolution;
  }

  void setWindowSize(i32 w, i32 h) {
    m_windowWidth = w;
    m_windowHeight = h;
    recalculateGrid();
    resetConcentrations();
  }

  void setResolution(i32 res) {
    m_resolution = res;
    recalculateGrid();

    initBuffersCPUComp();
    initBuffersGPUComp();
    resetConcentrations();
  }

  void setParams(f32 _f, f32 _k) {
    F = _f;
    k = _k;
  }

  // ui controls
  i32 getStepsPerFrame() { return m_stepsPerFrame; }

  bool isRunningOnGPU() { return m_isRunningOnGPU; }
  void toggleGPUComputation() { m_isRunningOnGPU = !m_isRunningOnGPU; }

  void handleMouseAction();
  bool isDraggingMouse() { return m_isDraggingMouse; }
  void setDraggingMouse(bool dragging) { m_isDraggingMouse = dragging; }
  void setMousePos(f64 x, f64 y) {
    m_mousePosX = x / m_resolution;
    m_mousePosY = (m_windowHeight - y) / m_resolution;
  }

 private:
  // gpu buffers initialization
  void initDefaultBuffers();
  void initBuffersCPUComp();
  void initBuffersGPUComp();

  void renderCPUComp();
  void renderGPUComp();
  void renderUI();

  void updateConcentrationTexture();

  static constexpr char VERTEX_SHADER_PATH[] = "shaders/passthrough.vert";
  static constexpr char FRAGMENT_SHADER_PATH[] = "shaders/grid.frag";
  static constexpr char SIM_SHADER_PATH[] = "shaders/simulation.frag";

  const Preset PRESETS[6] = {Preset{"Mazes", 0.037f, 0.060f},  Preset{"Worms", 0.078f, 0.061f},
                             Preset{"Flower", 0.055f, 0.062f}, Preset{"Waves", 0.014f, 0.045f},
                             Preset{"Pulses", 0.025f, 0.060f}, Preset{"Holes", 0.039f, 0.058f}};
};

#endif  // __APPLICATION_H__
