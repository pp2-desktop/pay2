#ifndef CD_USER_HPP
#define CD_USER_HPP

#include <memory>
#include "server_ws.hpp"
#include "json11.hpp"
#include "play_md.hpp"

class room;
typedef SocketServer<WS> WsServer;

class cd_user: public std::enable_shared_from_this<cd_user> { 

  //std::string uid_;
  size_t uid_;
  size_t alive_t_;
  
public:
  cd_user(WsServer& server, std::shared_ptr<WsServer::Connection> connection_ptr);
  ~cd_user();

  virtual void destory();

  void send(std::string payload);
  void send2(json11::Json payload);


  void set_uid(const size_t& uid) { uid_ = uid; }  
  size_t get_uid() { return uid_; }
  std::string get_uid_to_string() { return std::to_string(uid_); }

  void start_alive_time();
  void update_alive_t() { alive_t_ = time(NULL); }
  time_t get_alive_t() { return alive_t_; }



  WsServer& server_;
  std::shared_ptr<WsServer::Connection> connection_;

  std::weak_ptr<room> room_ptr;
};

#endif
