#include "asset_manager.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "raylib.h"

AssetManager::~AssetManager() {
  for (auto& texture_pair : textures) UnloadTexture(texture_pair.second);
  textures.clear();

  for (auto& font_pair : fonts) UnloadFont(font_pair.second);
  fonts.clear();
}

std::vector<Texture2D*> AssetManager::texture_list(
        const char* file_name_format) const {
  std::vector<Texture2D*> out{};

  for (int i{0};; i++) {
    char file_name[256];
    assert(strlen(file_name_format) < 240);

    sprintf(file_name, file_name_format, i);

    if (textures.count(file_name) == 0) break;

    out.emplace_back((Texture*) (&textures.at(file_name)));
  }

  return out;
}

void AssetManager::preload_textures(std::string folder) {
  // // std::cout << "!!! preload_textures: " << folder << std::endl;
  // TraceLog(LOG_ERROR, "FILEIO: preload_textures %s", folder.c_str());
  // FilePathList files = LoadDirectoryFiles("./resources/images/default");

  // for (int i = 0; i < (int) files.count; i++) {
  //   textures.insert(
  //           {GetFileName(files.paths[i]),
  //            LoadTexture(files.paths[i])});
  // }
  textures.insert({"background.png", LoadTexture("resources/images/default/background.png")});
  textures.insert({"coin.png", LoadTexture("resources/images/default/coin.png")});
  textures.insert({"decoration_0.png", LoadTexture("resources/images/default/decoration_0.png")});
  textures.insert({"decoration_1.png", LoadTexture("resources/images/default/decoration_1.png")});
  textures.insert({"decoration_2.png", LoadTexture("resources/images/default/decoration_2.png")});
  textures.insert({"door_close_0.png", LoadTexture("resources/images/default/door_close_0.png")});
  textures.insert({"door_close_1.png", LoadTexture("resources/images/default/door_close_1.png")});
  textures.insert({"door_close_2.png", LoadTexture("resources/images/default/door_close_2.png")});
  textures.insert({"door_close_3.png", LoadTexture("resources/images/default/door_close_3.png")});
  textures.insert({"door_close_4.png", LoadTexture("resources/images/default/door_close_4.png")});
  textures.insert({"door_close_5.png", LoadTexture("resources/images/default/door_close_5.png")});
  textures.insert({"door_close_6.png", LoadTexture("resources/images/default/door_close_6.png")});
  textures.insert({"door_close_7.png", LoadTexture("resources/images/default/door_close_7.png")});
  textures.insert({"door_close.png", LoadTexture("resources/images/default/door_close.png")});
  textures.insert({"door_open.png", LoadTexture("resources/images/default/door_open.png")});
  textures.insert({"end.png", LoadTexture("resources/images/default/end.png")});
  textures.insert({"enemy_chaser_0.png", LoadTexture("resources/images/default/enemy_chaser_0.png")});
  textures.insert({"enemy_chaser_1.png", LoadTexture("resources/images/default/enemy_chaser_1.png")});
  textures.insert({"enemy_chaser_2.png", LoadTexture("resources/images/default/enemy_chaser_2.png")});
  textures.insert({"enemy_random_0.png", LoadTexture("resources/images/default/enemy_random_0.png")});
  textures.insert({"ground.png", LoadTexture("resources/images/default/ground.png")});
  textures.insert({"ladybug_fly_0.png", LoadTexture("resources/images/default/ladybug_fly_0.png")});
  textures.insert({"ladybug_fly_1.png", LoadTexture("resources/images/default/ladybug_fly_1.png")});
  textures.insert({"ladybug_move_0.png", LoadTexture("resources/images/default/ladybug_move_0.png")});
  textures.insert({"ladybug_move_1.png", LoadTexture("resources/images/default/ladybug_move_1.png")});
  textures.insert({"ladybug_move_2.png", LoadTexture("resources/images/default/ladybug_move_2.png")});
  textures.insert({"ladybug_move_3.png", LoadTexture("resources/images/default/ladybug_move_3.png")});
  textures.insert({"ladybug.png", LoadTexture("resources/images/default/ladybug.png")});
  textures.insert({"ladybug_stand_0.png", LoadTexture("resources/images/default/ladybug_stand_0.png")});
  textures.insert({"ladybug_stand_1.png", LoadTexture("resources/images/default/ladybug_stand_1.png")});
  textures.insert({"ladybug_stand_2.png", LoadTexture("resources/images/default/ladybug_stand_2.png")});
  textures.insert({"ladybug_stand_3.png", LoadTexture("resources/images/default/ladybug_stand_3.png")});
  textures.insert({"poop.png", LoadTexture("resources/images/default/poop.png")});
  textures.insert({"regex.png", LoadTexture("resources/images/default/regex.png")});
  textures.insert({"regex_prepend.png", LoadTexture("resources/images/default/regex_prepend.png")});
  textures.insert({"shield.png", LoadTexture("resources/images/default/shield.png")});
  textures.insert({"spike.png", LoadTexture("resources/images/default/spike.png")});
}

AssetManager asset_manager = AssetManager();
