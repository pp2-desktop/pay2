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


  void set_uid(const long long& uid) { uid_ = uid; }  
  long long get_uid() { return uid_; }
  std::string get_uid_to_string() { return std::to_string(uid_); }

  void set_name(std::string name) { name_ = name; }  
  std::string& get_name() { return name_; }

  void set_is_facebook_login(bool is_facebook_login) { is_facebook_login_ = is_facebook_login; }  
  bool get_is_facebook_login() { return is_facebook_login_; }
  void set_facebookid(std::string facebookid) { facebookid_ = facebookid; }  
  std::string& get_facebookid() { return facebookid_; }

  void set_score(int score);
  int get_score();
  void set_win_count(int win_count);
  int get_win_count();
  void set_lose_count(int lose_count);
  int get_lose_count();
  void set_ranking(int ranking);
  int get_ranking();

  void start_alive_time();
  void update_alive_t() { alive_t_ = time(NULL); }
  time_t get_alive_t() { return alive_t_; }

  WsServer& server_;
  std::shared_ptr<WsServer::Connection> connection_;

  std::weak_ptr<room> room_ptr;

  std::string name_;
  int score_;
  int win_count_;
  int lose_count_;
  int ranking_;
  bool is_facebook_login_;
  std::string facebookid_;
};

#endif
