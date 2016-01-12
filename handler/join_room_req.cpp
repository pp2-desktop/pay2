#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "join_room_req.hpp"
#include "../vs_room_md.hpp"
#include "../cd_user.hpp"

//static size_t cnt = 0;
bool join_room_req(std::shared_ptr<cd_user> user_ptr, Json payload) {

  std::cout << "join_room_req recv" << std::endl;

  if(user_ptr->get_is_seat()) {
    std::cout << "[error] user already in the room" << std::endl;
    return true;
  }

  /*
  std::cout << payload["msg"].string_value() << std::endl;
  std::cout << "받은 횟수: " << cnt++ << std::endl;


  Json my_json = Json::object {
    { "key1", "value1" },
    { "key2", false },
    { "key3", Json::array { 1, 2, 3 } },
  };
  std::string message_str = my_json.dump();

  for(auto a_user: vs_room_md::get().users) {
    auto send_stream=std::make_shared<WsServer::SendStream>();
    *send_stream << message_str;
    a_user->server_.send(a_user->connection_, send_stream);
  }
  */
  vs_room_md::get().join_room(user_ptr);

  return true;
}
