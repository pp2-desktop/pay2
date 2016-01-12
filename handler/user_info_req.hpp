#ifndef USER_INFO_HPP
#define USER_INFO_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool login_req(std::shared_ptr<cd_user>, Json);
bool logout_req(std::shared_ptr<cd_user>, Json);
bool update_alive_req(std::shared_ptr<cd_user> user_ptr, Json);

#endif
