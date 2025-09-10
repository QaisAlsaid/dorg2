#pragma once

#include "raylib.h"
#include "scene.hpp"
#include <cstdint>

class HungryDorgScene : public Scene
{
public:
  void Init() override;
  void Shutdown() override;
  void Update(float delta) override;

private:
  void _Update(float delta);
  void _Draw();
  void _DrawUI();

  void HandleFoodSpawn(float delta);
  void CheckFoodAgainstBoundary();
  void CheckFoodAgainstPlayer();
  void NthFood();
  void GameOver();
  void TransitionScene(float delta);
  void InitAudio();
  void UpdateAudio(float delta);

private:
  struct Data;
  Data* m_data;

private:
  class Player
  {
  public:
    Rectangle coords;
    Rectangle src;
    Texture2D texture;

    Player();
    void Update(float delta);
    void Draw();

    static constexpr Vector2 s_player_sprite_size{100, 100};
    Vector2 last_pointer{0, 0};
    bool dragging = false;
    int lives = 3;
    uint32_t score = 0;
  };

  class Food
  {
  public:
    enum Type
    {
      Discord = 0,
      Shoe,
      Bone,
      Pizza,
      Max
    };

    Rectangle coords;
    float speed_y = 100;
    Type type = Type::Pizza;

    Food(Vector2 position, int tp, float sy);

    void Update(float delta);
    void Draw();
    bool IsEadble();

    static Texture2D s_food_textures;
    static constexpr Vector2 s_food_sprite_size = {64, 64};
    static constexpr float s_base_speed = 100.0f;
    static void Init();
  };
};