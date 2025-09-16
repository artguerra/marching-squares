#ifndef __PROFILER_UI_H__
#define __PROFILER_UI_H__

#include <imgui.h>

#include "Profiler.h"

inline void DrawProfilerImGui(Profiler& prof) {
  ImGuiIO& io = ImGui::GetIO();
  const float PAD = 10.0f;

  ImGui::SetNextWindowBgAlpha(0.85f);
  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - PAD, PAD), ImGuiCond_Always, ImVec2(1, 0));

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

  if (ImGui::Begin("Profiler##overlay", nullptr, flags)) {
    float frametime = (float)prof.frametime;
    float fps = frametime > 0 ? 1000.0f / frametime : 0.0f;

    ImGui::Text("FPS: %.1f  (avg %.1f)", fps, (float)prof.avg_fps);
    ImGui::Text("Frametime: %.2f ms", frametime);
    ImGui::PlotLines(
        "Frametime (ms)", prof.frametime_history, Profiler::HISTORY, prof.history_idx, nullptr,
        0.0f, 50.0f, ImVec2(260, 60)
    );

    if (ImGui::CollapsingHeader("Sections", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (auto& kv : prof.scopes_stats) {
        const auto& name = kv.first;
        const auto& s = kv.second;
        ImGui::Text(
            "%s last: %6.2f ms  avg: %6.2f  (n=%d)", name.c_str(), s.last_ms, s.avg_ms, s.count
        );
      }
    }
  }
  ImGui::End();
}

#endif

