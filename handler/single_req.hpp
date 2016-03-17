#ifndef SINGLE_REQ_HPP
#define SINGLE_REQ_HPP

#include <memory>
#include "../json11.hpp"

using namespace json11;

class cd_user;

bool single_img_info_req(std::shared_ptr<cd_user> user, Json payload);
bool max_stage_req(std::shared_ptr<cd_user> user, Json payload);

#endif
