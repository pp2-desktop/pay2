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

  std::cout << "join user" << std::endl;
  send_join_noti(user->get_name());
  users_[user->get_uid()] = user;
  return true;
}

bool lobby_md::leave_user(user_ptr user) {
  std::cout << "leave user" << std::endl;

  std::lock_guard<std::mutex> lock(m);

  if(users_.find( user->get_uid() ) == users_.end()) {
    std::cout << "유저가 존재하지 않음" << std::endl;
    return false;
  }

  users_.erase(user->get_uid());
  send_leave_noti(user->get_name());

  return true;
}

void lobby_md::send_chat(Json payload) {

  std::lock_guard<std::mutex> lock(m);

  std::string name = payload["name"].string_value();
  std::string msg = payload["msg"].string_value();

  json11::Json chat_msg = json11::Json::object {
    { "type", "send_chat_noti" },
    { "name", name },
    { "msg", msg }
  };

  std::cout << "1111111111111111" << std::endl;
  for(auto& it : users_) {
  std::cout << "222222222222222222" << std::endl;
    it.second->send2(chat_msg);
  }

}

void lobby_md::create_room_noti(int rid, std::string title, std::string password) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json noti = json11::Json::object {
    { "type", "create_room_noti" },
    { "rid", rid },
    { "title", title },
    { "password", password }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::destroy_room_noti(int rid) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json noti = json11::Json::object {
    { "type", "destroy_room_noti" },
    { "rid", rid }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::full_room_noti(int rid) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json noti = json11::Json::object {
    { "type", "full_room_noti" },
    { "rid", rid }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::available_room_noti(int rid) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json noti = json11::Json::object {
    { "type", "available_room_noti" },
    { "rid", rid }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::start_game_noti(int rid) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json noti = json11::Json::object {
    { "type", "start_game_noti" },
    { "rid", rid }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::send_chat_list(user_ptr user) {
  std::cout << "send_chat_list in lobby_md" << std::endl;

}

void lobby_md::send_join_noti(std::string name) {

  json11::Json noti = json11::Json::object {
    { "type", "join_lobby_noti" },
    { "name", name }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}

void lobby_md::send_leave_noti(std::string name) {

  json11::Json noti = json11::Json::object {
    { "type", "leave_lobby_noti" },
    { "name", name }
  };

  for(auto& it : users_) {
    it.second->send2(noti);
  }
}


/*
void lobby_md::update_room_info_noti(std::string type, int rid) {
  std::lock_guard<std::mutex> lock(m);

  json11::Json chat_msg = json11::Json::object {
    { "type", type },
    { "rid", rid }
  };

  for(auto& it : users_) {
    it.second->send2(chat_msg);
  }
}
*/
