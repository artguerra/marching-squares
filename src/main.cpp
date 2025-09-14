#include <cstdlib>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Application.h"

constexpr float ASPECT_RATIO = 16.f / 9;
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = WINDOW_WIDTH / ASPECT_RATIO;

// global contexts
GLFWwindow* g_window;
Application* g_app;

// global state variables
bool g_wireframeActive = false;

void windowSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

  if (g_app) g_app->setWindowSize(width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS && key == GLFW_KEY_X) {
    if (g_wireframeActive)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    g_wireframeActive = !g_wireframeActive;
  } else if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true);
  }
}

void initGLFW() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  g_window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Depixelization", nullptr, nullptr);
  if (!g_window) {
    std::cerr << "Failed to create GLFW window" << '\n';

    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(g_window);
  glfwSetFramebufferSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
}

void initOpenGL() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << '\n';
    std::exit(EXIT_FAILURE);
  }

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
}

void initImgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // default font size is 18
  io.Fonts->AddFontFromFileTTF("external/imgui/misc/fonts/Karla-Regular.ttf", 36.0f);
  ImGui::GetStyle().ScaleAllSizes(2.0f);

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
}

int main() {
  initGLFW();
  initOpenGL();
  initImgui();

  int w, h;
  glfwGetWindowSize(g_window, &w, &h);
  g_app = new Application(w, h);

  while (!glfwWindowShouldClose(g_window)) {
    glfwSwapBuffers(g_window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // imgui rendering
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // render
    g_app->render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwPollEvents();
  }

  delete g_app;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
