#include "hungry_dorg_scene.hpp"
#include "app.hpp"
#include "input.hpp"
#include "log.hpp"
#include "menu_scene.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
struct HungryDorgScene::Data
{
  constexpr static auto s_max_food_count = 25;
  bool is_playable_state = true;
  bool is_advert_on = false;
  float food_spawn_timer = 0;
  float food_spawn_time = 1;
  float transition_timer = 0;
  float transition_time = 3;
  int food_spawn_point_y = -Food::s_food_sprite_size.y;
  int divider = 10;
  float speed_mult = 1;
  float bark_timer = 0;
  float bark_time = 1;
  Rectangle lower_boundary{0, App::base_window_height + 200, App::base_window_width, 50};
  Player player;
  std::vector<Food> food;
  Sound music;
  Sound bark;
};

void HungryDorgScene::Init()
{
  m_data = new Data();
  Food::Init();
  m_data->player.coords.x = App::base_window_width / 2.0f;
  InitAudio();
  PlaySound(m_data->music);
}

void HungryDorgScene::Shutdown()
{
  UnloadSound(m_data->bark);
  UnloadSound(m_data->music);
  UnloadTexture(m_data->player.texture);
  delete m_data;
  m_data = nullptr;
}

void HungryDorgScene::Update(float delta)
{
  _Update(delta);
  BeginDrawing();

  if(m_data->is_playable_state)
    ClearBackground(RAYWHITE);
  else
    ClearBackground(BLACK);

  BeginMode2D(Camera2D{
      .offset = {0, 0}, .target = {0, 0}, .rotation = 0, .zoom = (float)GetScreenWidth() / App::base_window_width});

  _Draw();

  EndMode2D();

  _DrawUI();

  LOG("ending drawing");
  EndDrawing();
  LOG("ended drawing");
}

void HungryDorgScene::InitAudio()
{
  auto music_file = 1;
  m_data->music = LoadSound(("assets/song" + std::to_string(music_file) + ".wav").c_str());
  m_data->bark = LoadSound("assets/bark.wav");
}

void HungryDorgScene::_Update(float delta)
{
  LOG("game update: delta: {}", delta);
  if(!m_data->is_playable_state)
  {
    TransitionScene(delta);
    return;
  }
  UpdateAudio(delta);
  HandleFoodSpawn(delta);
  m_data->player.Update(delta);
  for(auto& f : m_data->food)
    f.Update(delta);

  CheckFoodAgainstBoundary();
  CheckFoodAgainstPlayer();
}

void HungryDorgScene::UpdateAudio(float delta)
{
  if(!m_data->is_playable_state)
    return;
  if(!IsSoundPlaying(m_data->music))
    PlaySound(m_data->music);
  if(IsSoundPlaying(m_data->bark))
  {
    m_data->bark_timer += delta;
    if(m_data->bark_timer > m_data->bark_time)
    {
      StopSound(m_data->bark);
      m_data->bark_timer = 0;
    }
  }
}

void HungryDorgScene::TransitionScene(float delta)
{
  if(m_data->is_playable_state)
    return;
  m_data->transition_timer += delta;
  if(m_data->transition_timer > m_data->transition_time)
  {
    App::Get()->SwitchScene(std::make_unique<MenuScene>());
  }
}

void HungryDorgScene::_Draw()
{
  LOG("game draw");
  if(!m_data->is_playable_state)
    return;
  for(auto& f : m_data->food)
    f.Draw();
  m_data->player.Draw();
}

void HungryDorgScene::_DrawUI()
{
  if(m_data->is_playable_state)
  {
    std::stringstream ss;
    ss << "score: " << std::to_string(m_data->player.score);
    DrawText(ss.str().c_str(), 0, 0, 22, BLACK);
    std::stringstream ss2;
    ss2 << "remaining lives: " << std::to_string(m_data->player.lives);
    auto str = ss2.str();
    auto ts = MeasureText(str.c_str(), 22) + 22;
    DrawText(str.c_str(), App::base_window_width - ts, 0, 22, BLACK);
  }
  else
  {
    auto text = "YOU LOST hehehe";
    auto ts = MeasureText(text, 32);
    auto a = (uint8_t)(uint32_t(m_data->transition_time / m_data->transition_timer * 255));
    LOG("a : {}", a);
    Color c{255, 255, 255, a};
    DrawText(text, (App::base_window_width / 2.0f) - ts / 2.0f, App::base_window_height / 2.0f, 32, c);
  }
}

Texture2D HungryDorgScene::Food::s_food_textures;

void HungryDorgScene::HandleFoodSpawn(float delta)
{
  m_data->food_spawn_timer += delta;
  if(m_data->food_spawn_timer > m_data->food_spawn_time)
  {
    auto pos_x = (float)GetRandomValue(0, App::base_window_width);
    auto tp = GetRandomValue(0, 4);
    LOG("spawn food: positionX: {}", pos_x);
    m_data->food.emplace_back(
        Vector2{pos_x, (float)m_data->food_spawn_point_y}, tp, m_data->speed_mult * Food::s_base_speed);
    m_data->food_spawn_timer = 0;
  }
}

void HungryDorgScene::CheckFoodAgainstBoundary()
{
  for(auto it = m_data->food.begin(); it != m_data->food.end();)
  {
    auto& f = *it;
    if(Physics::AABBCheck(f.coords, m_data->lower_boundary))
      it = m_data->food.erase(it);
    else
      it++;
  }
}

void HungryDorgScene::CheckFoodAgainstPlayer()
{
  for(auto it = m_data->food.begin(); it != m_data->food.end();)
  {
    auto& f = *it;
    if(Physics::AABBCheck(f.coords, m_data->player.coords))
    {
      if(f.IsEadble())
        m_data->player.score++;
      else
        m_data->player.lives--;
      it = m_data->food.erase(it);
    }
    else
      it++;
  }
  if(m_data->player.lives < 1)
  {
    GameOver();
    return;
  }
  if(m_data->player.score % m_data->divider == m_data->divider)
  {
    NthFood();
  }
}

void HungryDorgScene::NthFood()
{
  m_data->divider++;
  m_data->speed_mult += m_data->divider * 0.1;
  PlaySound(m_data->bark);
  auto r = GetRandomValue(1, 100);
  SetSoundPitch(m_data->bark, 1.0f / (float)r);
}

void HungryDorgScene::GameOver()
{
  m_data->is_playable_state = false;
}

HungryDorgScene::Player::Player()
    : coords({0, App::base_window_height - 200, s_player_sprite_size.x, s_player_sprite_size.y}),
      src({0, 0, s_player_sprite_size.x, s_player_sprite_size.y})
{
  texture = LoadTexture("assets/player.png");
}

void HungryDorgScene::Player::Update(float delta)
{
  LOG("player update: delta: {}, position: [{}, {}], size: [{}, {}]",
      delta,
      coords.x,
      coords.y,
      coords.width,
      coords.height);
  if(Input::IsPointerDown())
  {
    auto pointer_pos = Input::GetPointerPosition();
    if(!dragging)
    {
      dragging = true;
      last_pointer = pointer_pos;
    }
    float dx = pointer_pos.x - last_pointer.x;
    coords.x += dx;
    coords.x = std::clamp(coords.x, coords.width / 2.0f, (float)App::base_window_width + coords.width / 2.0f);
    last_pointer = pointer_pos;
  }
  else
    dragging = false;
}

void HungryDorgScene::Player::Draw()
{
  LOG("player draw");
  DrawTexturePro(texture, src, coords, s_player_sprite_size, 0, WHITE);
}

HungryDorgScene::Food::Food(Vector2 position, int tp, float s)
{
  coords.x = position.x;
  coords.y = position.y;
  coords.width = s_food_sprite_size.x;
  coords.height = s_food_sprite_size.y;
  type = (Type)(tp % 4);
  speed_y = s;
}

void HungryDorgScene::Food::Update(float delta)
{
  LOG("food update: delta: {}, position: [{}, {}], size: [{}, {}], type: {}",
      delta,
      coords.x,
      coords.y,
      coords.width,
      coords.height,
      (int)type);
  coords.y += speed_y * delta;
}

void HungryDorgScene::Food::Draw()
{
  assert(type < Type::Max);
  LOG("food draw type: {}", (int)type);
  Rectangle src{(float)type * s_food_sprite_size.x, 0, s_food_sprite_size.x, s_food_sprite_size.y};
  DrawTexturePro(s_food_textures, src, coords, s_food_sprite_size, 0, WHITE);
}

bool HungryDorgScene::Food::IsEadble()
{
  return type == Type::Bone || type == Type::Pizza;
}

void HungryDorgScene::Food::Init()
{
  s_food_textures = LoadTexture("assets/food.png");
}