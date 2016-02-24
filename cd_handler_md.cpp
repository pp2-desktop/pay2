#include <utility>
#include <iostream>
#include "cd_handler_md.hpp"
#include "handler/user_info_req.hpp"
#include "handler/lobby_req.hpp"
#include "handler/play_req.hpp"

cd_handler_md::cd_handler_md() {

}

cd_handler_md::~cd_handler_md() {

}

bool cd_handler_md::add_payload(std::string type, std::function<bool(user_ptr, Json)> handle_fn_ptr) {
  return m.insert(std::make_pair(type, handle_fn_ptr)).second;
}

bool cd_handler_md::init() {
  std::cout << "cd_handler_md init called" << std::endl;
  auto r = true;


  // user req
  r &= add_payload("login_req", login_req);
  r &= add_payload("create_guest_account_req", create_guest_account_req);
  r &= add_payload("update_alive_noti", update_alive_noti);

  r &= add_payload("update_game_info_noti", update_game_info_noti);
  r &= add_payload("get_ranking_req", get_ranking_req);

  // lobby_req
  r &= add_payload("join_lobby_req", join_lobby_req);
  r &= add_payload("leave_lobby_req", leave_lobby_req);
  r &= add_payload("send_chat_noti", send_chat_noti);

  r &= add_payload("room_list_req", room_list_req);
  r &= add_payload("chat_list_req", chat_list_req);

  // play_req
  r &= add_payload("create_room_req", create_room_req);
  r &= add_payload("quick_join_req", quick_join_req);
  r &= add_payload("join_room_req", join_room_req);
  r &= add_payload("leave_room_req", leave_room_req);
  r &= add_payload("start_game_req", start_game_req);
  r &= add_payload("ready_game_noti", ready_game_noti);

  r &= add_payload("ready_stage_noti", ready_stage_noti);
  r &= add_payload("check_point_req", check_point_req);

  r &= add_payload("master_info_req", master_info_req);
  r &= add_payload("opponent_info_req", opponent_info_req);
  r &= add_payload("kick_opponent_noti", kick_opponent_noti);
  r &= add_payload("check_ready_opponent_req", check_ready_opponent_req);

  return r;
}
 
