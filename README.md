# Gray–Scott Reaction–Diffusion (CPU/GPU, real-time)

This project is an implementation of the **Gray–Scott reaction–diffusion** system in **OpenGL/GLSL** with a **CPU solver** and a **GPU solver**, designed to enable direct **performance and behavior comparison** between the two.

## Overview

* Simulates Gray–Scott dynamics on a 2D grid and visualizes patterns in real time.
* Parameters (feed **F**, kill **k**, diffusion **Du/Dv**, time step, steps-per-frame) are adjustable at runtime.
* Interactive seeding (mouse click); lightweight on-screen profiler.

## Implementation

* **CPU path (reference):** explicit finite-difference integration on a toroidal grid. Useful as a baseline for correctness and performance.
* **GPU path (fragment-shader compute with ping–pong):**
  * A single **RG floating-point texture** stores the state `(U,V)` (R=U, G=V).
  * A full-screen **fragment shader** computes the next state per texel (sampling neighbors via `texelFetch`).
  * The shader writes results to an **FBO-attached texture** (the "destination"). On the next step, source and destination textures are **swapped** ("ping–pong"), avoiding read–write hazards.
  * The current state texture is also sampled by a simple **display shader** to color pixels for visualization.
* **OpenGL details:** modern core profile, render-to-texture FBOs, nearest sampling, explicit control of viewport vs. simulation grid size, and fixed-Δt stepping with multiple simulation steps per frame.

## Purpose

* Provide a clear, minimal **reference implementation** of Gray–Scott in both CPU and GPU forms.
* Facilitate **head-to-head performance comparison** and inspection of numerical differences between CPU finite-difference and fragment-shader compute approaches.

**Technologies:** C++20, OpenGL (core), GLSL, ImGui.

## Some results

