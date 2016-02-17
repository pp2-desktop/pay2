#include "cd_user.hpp"
#include <iostream>
#include "lobby_md.hpp"

typedef std::shared_ptr<cd_user> user_ptr;  

cd_user::cd_user(WsServer& server, std::shared_ptr<WsServer::Connection> connection_ptr) : server_(server), connection_(connection_ptr) {
  std::cout << "유저 생성자 called" << std::endl;
  start_alive_time();
  score_ = 0;
  win_count_ = 0;
  lose_count_ = 0;
  is_facebook_login_ = false;
  facebookid_ = "";
}

cd_user::~cd_user() {
  std::cout << "유저 소멸자 called" << std::endl;
}

void cd_user::start_alive_time() {
  alive_t_ = time(NULL);
}

void cd_user::send(std::string payload) {
  auto send_stream=std::make_shared<WsServer::SendStream>();
  *send_stream << payload;
  server_.send(connection_, send_stream);
}

void cd_user::send2(json11::Json payload) {
  send(payload.dump());
}

void cd_user::destory() {
  std::cout << "user destroy called" << std::endl;
  auto sp = std::shared_ptr<cd_user>(shared_from_this());

  if(sp) {
    if(auto room = room_ptr.lock()) {
      std::cout << "destroy step 1" << std::endl;
      play_md::get().leave_user(sp);
    }

    std::cout << "destroy step 2" << std::endl;
    lobby_md::get().leave_user(sp);
    std::cout << "destroy step 3" << std::endl;
  } else {
    std::cout << "sp is nullptr" << std::endl;
  }
}

void cd_user::set_score(int score) {
  score_ = score;
}

int cd_user::get_score() {
  return score_;
}

void cd_user::set_win_count(int win_count) {
  win_count_ = win_count;
}

int cd_user::get_win_count() {
  return win_count_;
}

void cd_user::set_lose_count(int lose_count) {
  lose_count_ = lose_count;
}

int cd_user::get_lose_count() {
  return lose_count_;
}

void cd_user::set_ranking(int ranking) {
  ranking_ = ranking;
}

int cd_user::get_ranking() {
  return ranking_;
}
