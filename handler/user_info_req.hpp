#ifndef USER_INFO_HPP
#define USER_INFO_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool login_req(std::shared_ptr<cd_user>, Json payload);
bool logout_req(std::shared_ptr<cd_user>, Json payload);
bool update_alive_req(std::shared_ptr<cd_user> user_ptr, Json payload);
bool update_alive_noti(std::shared_ptr<cd_user> user_ptr, Json payload);
bool get_user_info_req(std::shared_ptr<cd_user> user_ptr, Json payload);
bool create_guest_account_req(std::shared_ptr<cd_user> user_ptr, Json payload);

bool update_game_info_noti(std::shared_ptr<cd_user> user_ptr, Json payload);
bool get_ranking_req(std::shared_ptr<cd_user> user_ptr, Json payload);

#endif
