// #include "physics.hpp"
// #include "raylib.h"
// #include "raymath.h"
// #include <assert.h>
// #include <cstdint>
// #include <print>
// #include <random>
// #include <vector>
// #if defined(PLATFORM_WEB)
// #include <emscripten/emscripten.h>
// #endif

// #define LOG(...) std::println(__VA_ARGS__);

// constexpr int base_width = 1280;
// constexpr int base_height = 720;
// constexpr const char *title = "catch cool game";

// void Init();
// void Die();
// void UpdateDrawFrame();
// void DrawInVirtualResolution();
// void DrawUI();
// void Update();

// int main(void) {
//   InitWindow(base_width, base_height, title);
//   SetWindowState(FLAG_WINDOW_RESIZABLE);
//   Init();

// #if defined(PLATFORM_WEB)
//   emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
// #else
//   SetTargetFPS(60);

//   while (!WindowShouldClose()) {
//     UpdateDrawFrame();
//   }
// #endif

//   Die();
//   CloseWindow();

//   return 0;
// }

// void UpdateDrawFrame(void) {

//   Update();
//   BeginDrawing();

//   ClearBackground(RAYWHITE);

//   BeginMode2D(Camera2D{.offset = {0, 0},
//                        .target = {0, 0},
//                        .rotation = 0,
//                        .zoom = (float)GetScreenWidth() / base_width});

//   DrawInVirtualResolution();

//   EndMode2D();

//   DrawUI();

//   EndDrawing();
// }

// class Player {
// public:
//   Rectangle coords{0, base_height - 200, s_player_sprite_size.x,
//                    s_player_sprite_size.y};
//   Rectangle src{0, 0, s_player_sprite_size.x, s_player_sprite_size.y};
//   Texture2D texture;

//   Player() { texture = LoadTexture("assets/player.png"); }

//   void Update(float delta) {
//     LOG("player update: delta: {}, position: [{}, {}], size: [{}, {}]", delta,
//         coords.x, coords.y, coords.width, coords.height);
//     if (IsPointerDown()) {
//       auto pointer_pos = GetPointerPosition();
//       coords.x = pointer_pos.x - coords.width / 2.0f;
//       coords.x = Clamp(coords.x, coords.width, base_width);
//     }
//   }

//   void Draw() {
//     LOG("player draw");
//     DrawTexturePro(texture, src, coords, s_player_sprite_size, 0, WHITE);
//   }

//   static constexpr Vector2 s_player_sprite_size{100, 100};
// };

// class Food {
// public:
//   enum Type { Discord = 0, Shoe, Bone, Pizza, Max };

//   Rectangle coords{0, 0};
//   float speed_y = 10;
//   Type type = Type::Bone;

//   Food(Vector2 position) {
//     coords.x = position.x;
//     coords.y = position.y;
//   }

//   void Update(float delta) {
//     LOG("food update: delta: {}, position: [{}, {}], size: [{}, {}], type: {}",
//         delta, coords.x, coords.y, coords.width, coords.height, (int)type);
//     coords.y += speed_y * delta;
//   }

//   void Draw() {
//     assert(type < Type::Max);
//     LOG("food draw");
//     Rectangle src{(float)type, 0, s_food_sprite_size.x, s_food_sprite_size.y};
//     DrawTexturePro(s_food_textures, src, coords, s_food_sprite_size, 0, WHITE);
//   }

//   bool IsEadble() { return type == Type::Bone || type == Type::Pizza; }

//   static Texture2D s_food_textures;
//   static constexpr Vector2 s_food_sprite_size = {64, 64};
//   static void Init() { s_food_textures = LoadTexture("assets/food.png"); }
// };

// Texture2D Food::s_food_textures;

// class Game {
// public:
//   static void Set(Game *game) { s_instance = game; }
//   static Game *Get() { return s_instance; }

//   void Update(float delta) {
//     LOG("game update: delta: {}", delta);
//     player.Update(delta);
//     for (auto &f : food)
//       f.Update(delta);

//     if (!is_playable_state)
//       return;
//     HandleFoodSpawn(delta);
//     CheckFoodAgainstBoundary();
//     CheckFoodAgainstPlayer();
//   }

//   void Draw() {
//     LOG("game draw");
//     if (!is_playable_state)
//       return;
//     for (auto &f : food)
//       f.Draw();
//     player.Draw();
//   }

// private:
//   void HandleFoodSpawn(float delta) {
//     food_spawn_timer += delta;
//     if (food_spawn_timer > food_spawn_time) {
//       food.emplace_back(Vector2{(float)GetRandomValue(0, base_width),
//                                 (float)food_spawn_point_y});
//       food_spawn_timer = 0;
//     }
//   }

//   void CheckFoodAgainstBoundary() {
//     std::vector<int> del;
//     for (auto i = 0; const auto &f : food) {
//       if (Physics::AABBCheck(f.coords, lower_boundary))
//         del.push_back(i++);
//     }
//   }

//   void CheckFoodAgainstPlayer() {
//     std::vector<int> del;
//     for (auto i = 0; const auto &f : food) {
//       if (Physics::AABBCheck(f.coords, player.coords)) {
//         del.push_back(i++);
//         auto &pice = food[i];
//         if (pice.IsEadble())
//           score++;
//         else
//           lives--;
//       }
//     }
//     for (auto e : del) {
//       food.erase(food.begin() + e);
//     }
//     if (lives < 1) {
//       GameOver();
//       return;
//     }
//     if (score % divider == 10) {
//       NthFood();
//     }
//   }

//   void NthFood() {}

//   void GameOver() {}

// private:
//   constexpr static auto s_max_food_count = 25;
//   bool is_playable_state = true;
//   bool is_advert_on = false;
//   float food_spawn_timer = 0;
//   float food_spawn_time = 1000;
//   int food_spawn_point_y = -Food::s_food_sprite_size.y;
//   int divider = 10;
//   Rectangle lower_boundary{740, 0, base_width, 1};
//   Player player;
//   std::vector<Food> food;
//   uint32_t lives = 3;
//   uint32_t score = 0;
//   static Game *s_instance;
// };

// Game *Game::s_instance = nullptr;

// void Init() {
//   Game::Set(new Game());
//   Food::Init();
// }

// void Die() {}

// void Update() { Game::Get()->Update(GetFrameTime()); }

// void DrawInVirtualResolution() { Game::Get()->Draw(); }

// void DrawUI() {}

#include "app.hpp"
#include "hungry_dorg_scene.hpp"
#include <memory>
int main()
{
  App app(std::make_unique<HungryDorgScene>());
  app.Run();
}