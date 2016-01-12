#include <utility>
#include <iostream>
#include "cd_handler_md.hpp"
/*
#include "handler/join_room_req.hpp"
#include "handler/leave_room_req.hpp"
#include "handler/user_info_req.hpp"
#include "handler/vs_room_req.hpp"
#include "handler/vs_play_req.hpp"
*/

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
  /*
  r &= add_payload("join_room_req", join_room_req);
  r &= add_payload("leave_room_req", leave_room_req);
  r &= add_payload("login_req", login_req);
  r &= add_payload("logout_req", logout_req);
  r &= add_payload("update_alive_req", update_alive_req);
  r &= add_payload("opponent_ready_notify", opponent_ready_notify);
  r &= add_payload("start_vs_game_req", start_vs_game_req);
  r &= add_payload("round_info_req", round_info_req);
  r &= add_payload("start_round_req", start_round_req);
  r &= add_payload("find_spot_req", find_spot_req);
  r &= add_payload("play_to_lobby_req", play_to_lobby_req);
  */

  return r;
}
 
