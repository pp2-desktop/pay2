#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "lobby_req.hpp"
#include "../cd_user.hpp"
#include "../lobby_md.hpp"

bool join_lobby_req(std::shared_ptr<cd_user> user, Json payload) {

  std::string uid = payload["uid"].string_value();

  auto r = lobby_md::get().join_user(user);

  json11::Json res = json11::Json::object {
    { "type", "join_lobby_res" },
    { "result", r }
  };

  user->send2(res);
  return true;
}

bool leave_lobby_req(std::shared_ptr<cd_user> user, Json payload) {

  auto r = lobby_md::get().leave_user(user);

  json11::Json res = json11::Json::object {
    { "type", "leave_lobby_res" },
    { "result", r }
  };

  user->send2(res);
  return true;
}

bool send_chat_noti(std::shared_ptr<cd_user> user, Json payload) {
  lobby_md::get().send_chat(payload);
  return true;
}

bool room_list_req(std::shared_ptr<cd_user> user, Json payload) {
  play_md::get().send_room_list(user);
  return true;
}

bool chat_list_req(std::shared_ptr<cd_user> user, Json payload) {
  lobby_md::get().send_chat_list(user);
  return true;
}

bool update_user_info_req(std::shared_ptr<cd_user> user, Json payload) {

  return true;
}
