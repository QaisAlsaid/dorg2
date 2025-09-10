#pragma once

#include "scene.hpp"
#include <memory>
class App
{
public:
  App(std::unique_ptr<Scene> main_scene);
  ~App();
  void Run();
  void Update();

  void SwitchScene(std::unique_ptr<Scene> new_scene);

  static App* Get();

public:
  static constexpr auto base_window_width = 1280;
  static constexpr auto base_window_height = 720;
  static constexpr auto base_window_title = "Dorg 2";

private:
  void _SwitchScene();

private:
  std::unique_ptr<Scene> m_main_scene = nullptr;
  std::unique_ptr<Scene> m_scene_to_transition_to = nullptr;
  bool needs_switch = false;
  static App* s_app;
};