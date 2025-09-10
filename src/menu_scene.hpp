#pragma once

#include "raylib.h"
#include "scene.hpp"
class MenuScene : public Scene
{
public:
  void Init() override {};
  void Shutdown() override {};
  void Update(float delta) override;

private:
};