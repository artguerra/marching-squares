
#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <chrono>
#include <string>
#include <unordered_map>

struct Profiler {
  using clock = std::chrono::high_resolution_clock;
  static constexpr int HISTORY = 240;  // past 4s at 60 fps

  // frame stats
  float frametime_history[HISTORY] = {};
  int history_idx = 0;

  double frametime = 0.0;
  double avg_ms_accum = 0.0;
  int frame_count = 0;
  double avg_fps = 0.0;

  // per-frame begin/end
  clock::time_point frame_start{};

  void beginFrame() { frame_start = clock::now(); }

  void endFrame() {
    auto end = clock::now();
    frametime = std::chrono::duration<double, std::milli>(end - frame_start).count();
    frametime_history[history_idx] = (float)frametime;
    history_idx = (history_idx + 1) % HISTORY;

    avg_ms_accum += frametime;
    frame_count++;
    avg_fps = frame_count / (avg_ms_accum / 1000.0);
  }

  void restart() {
    frame_start = clock::now();

    std::fill_n(frametime_history, HISTORY, 0.0f);

    frametime = 0.0f;
    avg_ms_accum = 0.0f;
    frame_count = 0.0f;
    avg_fps = 0.0f;

    history_idx = 0;

    scopes_stats.clear();
  }

  // scopes
  struct Stat {
    double last_ms = 0, avg_ms = 0;
    int count = 0;
  };

  std::unordered_map<std::string, Stat> scopes_stats;

  struct Scope {
    Profiler& p;
    const char* name;
    clock::time_point t0;

    Scope(Profiler& _p, const char* n) : p(_p), name(n), t0(clock::now()) {}

    ~Scope() {
      auto dt = std::chrono::duration<double, std::milli>(clock::now() - t0).count();
      auto& s = p.scopes_stats[name];

      s.last_ms = dt;
      s.count++;
      s.avg_ms = (s.avg_ms * (s.count - 1) + dt) / s.count;
    }
  };
};

#endif

