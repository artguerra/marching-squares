#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <cstdint>

#include "Shader.h"

class Application {
 private:
  uint32_t m_windowWidth, m_windowHeight;
  uint32_t VAO, VBO, EBO;
  bool m_buffersInitializated{false};

  Shader m_mainShader;

 public:
  Application(uint32_t width, uint32_t height)
      : m_windowWidth{width},
        m_windowHeight{height},
        m_mainShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH)
  {}

  void render();

  void setWindowSize(uint32_t w, uint32_t h) {
    m_windowWidth = w;
    m_windowHeight = h;
  }

 private:
  void initBuffers();

  static constexpr char VERTEX_SHADER_PATH[] = "shaders/grid.vert";
  static constexpr char FRAGMENT_SHADER_PATH[] = "shaders/grid.frag";
};

#endif  // __APPLICATION_H__
