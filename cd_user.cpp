#include "cd_user.hpp"
#include <iostream>


typedef std::shared_ptr<cd_user> user_ptr;  

cd_user::cd_user(WsServer& server, std::shared_ptr<WsServer::Connection> connection_ptr) : server_(server), connection_(connection_ptr) {
  std::cout << "유저 생성자 called" << std::endl;
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

}
