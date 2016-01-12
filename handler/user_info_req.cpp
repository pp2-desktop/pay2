#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "user_info_req.hpp"
#include "../cd_user.hpp"

bool login_req(std::shared_ptr<cd_user> user_ptr, Json) {

  json11::Json res = json11::Json::object {
    { "type", "login_res" },
    { "uid", user_ptr->get_uid_to_string() }
  };

  user_ptr->send2(res);

  return true;
}

bool logout_req(std::shared_ptr<cd_user> user_ptr, Json) {

  json11::Json res = json11::Json::object {
    { "type", "logout_res" },
    { "result", true }
  };

  user_ptr->send2(res);
  return true;
}

bool update_alive_req(std::shared_ptr<cd_user> user_ptr, Json) {
  user_ptr->update_alive_t();
  return true;
}
