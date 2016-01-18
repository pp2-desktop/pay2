#include "lobby_md.hpp"

lobby_md::lobby_md() {

}

lobby_md::~lobby_md() {

}

bool lobby_md::join_user(user_ptr user) {

  std::lock_guard<std::mutex> lock(m);

  if(users_.find( user->get_uid() ) != users_.end()) {
    std::cout << "유저가 이미 존재함" << std::endl;
    return false;
  }

  users_[user->get_uid()] = user;
  return true;
}

bool lobby_md::leave_user(user_ptr user) {

  std::lock_guard<std::mutex> lock(m);

  if(users_.find( user->get_uid() ) == users_.end()) {
    std::cout << "유저가 이미 존재함" << std::endl;
    return false;
  }

  users_.erase(user->get_uid());

  return true;
}

void lobby_md::send_chat(Json payload) {

  std::lock_guard<std::mutex> lock(m);

  std::string nickname = payload["nickname"].string_value();
  std::string msg = payload["msg"].string_value();

  json11::Json chat_msg = json11::Json::object {
    { "type", "send_chat_noti" },
    { "nickname", nickname },
    { "msg", msg }
  };

  for(auto& it : users_) {
    it.second->send2(chat_msg);
  }

}
