#ifndef LOBBY_REQ_HPP
#define LOBBY_REQ_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool join_lobby_req(std::shared_ptr<cd_user>, Json payload);
bool leave_lobby_req(std::shared_ptr<cd_user>, Json payload);
bool send_chat_noti(std::shared_ptr<cd_user> user_ptr, Json payload);

#endif
