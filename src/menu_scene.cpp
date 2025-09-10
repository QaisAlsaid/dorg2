#include "menu_scene.hpp"
#include "app.hpp"
#include "hungry_dorg_scene.hpp"
#include "log.hpp"
#include "raylib.h"
#include <memory>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void MenuScene::Update(float delta)
{
  LOG("update menu: delta: {}", delta);
  BeginDrawing();
  ClearBackground(RAYWHITE);
  if(GuiButton(Rectangle{App::base_window_width / 2.0f - 100, App::base_window_height / 2.0f - 100, 200, 200}, "play"))
  {
    App::Get()->SwitchScene(std::make_unique<HungryDorgScene>());
  }
  EndDrawing();
}