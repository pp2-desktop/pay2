#ifndef VS_PLAY_HPP
#define VS_PLAY_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool round_info_req(std::shared_ptr<cd_user>, Json);
bool start_round_req(std::shared_ptr<cd_user> user_ptr, Json);
bool find_spot_req(std::shared_ptr<cd_user> user_ptr, Json);
bool play_to_lobby_req(std::shared_ptr<cd_user> user_ptr, Json);
//bool start_vs_game_req(std::shared_ptr<cd_user> user_ptr, Json);

#endif
