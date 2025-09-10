#include "raylib.h"

#include "app.hpp"

#include <cassert>
#include <memory>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame();

App* App::s_app = nullptr;

App::App(std::unique_ptr<Scene> main_scene) : m_main_scene(std::move(main_scene))
{
  assert(s_app == nullptr);
  s_app = this;
  InitAudioDevice();
}

App::~App()
{
  CloseAudioDevice();
  s_app = nullptr;
}

App* App::Get()
{
  return s_app;
}

void App::Run()
{
  InitWindow(base_window_width, base_window_height, base_window_title);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  m_main_scene->Init();
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);

  while(!WindowShouldClose())
  {
    UpdateDrawFrame();
  }
  m_main_scene->Shutdown();
#endif
}

void App::Update()
{
  if(needs_switch && m_scene_to_transition_to != nullptr)
  {
    _SwitchScene();
    return; // skip frame
  }
  m_main_scene->Update(GetFrameTime());
}

void App::_SwitchScene()
{
  m_main_scene->Shutdown();
  m_main_scene = std::move(m_scene_to_transition_to);
  m_main_scene->Init();
  m_scene_to_transition_to = nullptr; // redundant
  needs_switch = false;
}

void App::SwitchScene(std::unique_ptr<Scene> new_scene)
{
  m_scene_to_transition_to = std::move(new_scene);
  needs_switch = true;
}

void UpdateDrawFrame()
{
  App::Get()->Update();
}