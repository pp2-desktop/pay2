#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "play_req.hpp"
#include "../cd_user.hpp"
#include "../lobby_md.hpp"
#include "../play_md.hpp"
#include "../game_info.hpp"

bool create_room_req(std::shared_ptr<cd_user> user, Json payload) {

  std::string title = payload["title"].string_value();
  std::string password = payload["password"].string_value();

  auto rid = play_md::get().create_room(user, title, password);
  
  json11::Json res = json11::Json::object {
    { "type", "create_room_res" },
    { "rid", rid }
  };
  user->send2(res);

  //lobby_md::get().leave_user(user);
  lobby_md::get().create_room_noti(rid, title, password);
  /*
  std::string uid = payload["uid"].string_value();
  auto r = lobby_md::get().join_user(user_ptr);
  json11::Json res = json11::Json::object {
    { "type", "join_lobby_res" },
    { "result", r }
  };
  user_ptr->send2(res);
  */
  return true;
}

bool quick_join_req(std::shared_ptr<cd_user> user, Json payload) {

  auto is_create = true;
  auto rid = play_md::get().find_quick_join_room();

  if(rid >= 0) {
    is_create = false;
  }

  json11::Json res = json11::Json::object {
    { "type", "quick_join_res" },
    { "is_create", is_create },
    { "rid", rid }
  };
  user->send2(res);

  return true;
}

bool join_room_req(std::shared_ptr<cd_user> user, Json payload) {

  auto rid = payload["rid"].int_value();
  auto r = play_md::get().join_user(rid, user);

  if(r) {
    json11::Json res = json11::Json::object {
      { "type", "join_room_res" },
      { "result", r }
    };
    user->send2(res);
  }

  lobby_md::get().full_room_noti(rid);
  // 전체에게 유저가 들어와서 꽛 찼다고 알려줌

  return true;
}

bool leave_room_req(std::shared_ptr<cd_user> user, Json payload) {
  play_md::get().leave_user(user);

  return true;
}

bool start_game_req(std::shared_ptr<cd_user> user, Json payload) {
  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->start_game();
  } else {
    json11::Json res = json11::Json::object {
      { "type", "start_game_res" },
      { "result", false }
    };
    user->send2(res);
  }

  return true;
}

bool ready_game_noti(std::shared_ptr<cd_user> user, Json payload) {
  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->ready_game();
  }
  return true;
}

bool ready_stage_noti(std::shared_ptr<cd_user> user, Json payload) {
 if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->ready_stage(user);
  }
  return true;
}

bool check_point_req(std::shared_ptr<cd_user> user, Json payload) {
  auto x = static_cast<float>(payload["x"].int_value());
  auto y = static_cast<float>(payload["y"].int_value());
  int stage_count = payload["stage_count"].int_value();

  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->check_point(user, stage_count, vec2(x, y));
  }

  return true;
}
