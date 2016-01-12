#ifndef VS_ROOM_HPP
#define VS_ROOM_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool opponent_ready_notify(std::shared_ptr<cd_user>, Json);
bool start_vs_game_req(std::shared_ptr<cd_user> user_ptr, Json);

#endif
