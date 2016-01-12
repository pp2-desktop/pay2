#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "leave_room_req.hpp"
#include "../vs_room_md.hpp"
#include "../cd_user.hpp"

//static size_t cnt = 0;
bool leave_room_req(std::shared_ptr<cd_user> user_ptr, Json payload) {

  std::cout << "leave_room_req recv" << std::endl;

  if(!user_ptr->get_is_seat()) {
    std::cout << "[error] user already leave the room" << std::endl;
    return true;
  }
  
  vs_room_md::get().leave_room(user_ptr);

  json11::Json res = json11::Json::object {
    { "type", "leave_room_res" },
    { "result", true }
  };
  user_ptr->send(res.dump());

  return true;
}
