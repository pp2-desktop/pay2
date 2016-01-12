#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "user_info_req.hpp"
#include "../cd_user.hpp"
#include "../vs_room.hpp"

bool opponent_ready_notify(std::shared_ptr<cd_user> user_ptr, Json) {

  auto room_ptr = user_ptr->get_vs_room_ptr();
  
  if(room_ptr) {
    auto master_user_ptr = room_ptr->master_user_ptr;
    if(master_user_ptr) {
      master_user_ptr->send2(json11::Json::object {
	  { "type", "opponent_ready_notify" }
	});
    } 
  }
  return true;
}

bool start_vs_game_req(std::shared_ptr<cd_user> user_ptr, Json) {

  std::cout << "게임 시작 메시지 받음" << std::endl;

  auto room_ptr = user_ptr->get_vs_room_ptr();

  auto res = json11::Json::object{ 
    { "type", "start_vs_game_res" }
  };
  
  if(room_ptr) {
    auto master_user_ptr = room_ptr->master_user_ptr;
    if(master_user_ptr) {
      std::cout << "start_vs_game_res sent to master" << std::endl;
      master_user_ptr->send2(res);
    }
    auto opponent_user_ptr = room_ptr->opponent_user_ptr;
    if(opponent_user_ptr) {
      std::cout << "start_vs_game_res sent to opponent" << std::endl;
      opponent_user_ptr->send2(res);
    }
  }
  
  return true;
}
