#ifndef PLAY_REQ_HPP
#define PLAY_REQ_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool create_room_req(std::shared_ptr<cd_user> user, Json payload);
bool quick_join_req(std::shared_ptr<cd_user> user, Json payload);
bool leave_room_req(std::shared_ptr<cd_user> user, Json payload);
bool join_room_req(std::shared_ptr<cd_user> user, Json payload);
bool start_game_req(std::shared_ptr<cd_user> user, Json payload);
bool ready_game_noti(std::shared_ptr<cd_user> user, Json payload);

bool ready_stage_noti(std::shared_ptr<cd_user> user, Json payload);
bool check_point_req(std::shared_ptr<cd_user> user, Json payload);


#endif
