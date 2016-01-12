#ifndef LEAVE_ROOM_HPP
#define LEAVE_ROOM_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool leave_room_req(std::shared_ptr<cd_user>, Json);

#endif
