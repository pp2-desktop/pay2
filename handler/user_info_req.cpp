#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "user_info_req.hpp"
#include "../cd_user.hpp"

bool login_req(std::shared_ptr<cd_user> user_ptr, Json payload) {

  std::string uid = payload["uid"].string_value();

  json11::Json res = json11::Json::object {
    { "type", "login_res" },
    { "result", true },
    { "uid", user_ptr->get_uid_to_string() },
    { "score", 1050 },
    { "win_count", 50 },
    { "lose_count", 20 }
  };

  user_ptr->send2(res);

  return true;
}

bool logout_req(std::shared_ptr<cd_user> user_ptr, Json payload) {

  json11::Json res = json11::Json::object {
    { "type", "logout_res" },
    { "result", true }
  };

  user_ptr->send2(res);
  return true;
}

bool update_alive_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  user_ptr->update_alive_t();
  return true;
}

bool get_user_info_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  std::string uid = payload["uid"].string_value();
  // uid 기준으로 유저 정보를 가져옴

  return true;
}

bool create_guest_account_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  static int guest_id = 100;
  std::string name = "guest" + std::to_string(guest_id++);

  json11::Json res = json11::Json::object {
    { "type", "create_guest_account_res" },
    { "result", true },
    { "name", name },
    { "password", name }
  };

  user_ptr->send2(res);

  return true;
}
