#ifndef CD_USER_MD_HPP
#define CD_USER_MD_HPP

#include <functional>
#include <map>
#include "json11.hpp"
#include "cd_user.hpp"
#include "server_ws.hpp"

typedef std::shared_ptr<cd_user> user_ptr;  

using namespace json11;

class cd_user_md {

public:  

  std::map<size_t, user_ptr> users_;

  cd_user_md();
  ~cd_user_md();

  static cd_user_md& get() {
    static cd_user_md obj;
    return obj;
  }

  //bool init();
  void start_check_alive();
  void stop_check_alive();
  bool add_user(long long uid, user_ptr user);
  bool remove_user(long long uid);
  bool get_users_size();

  user_ptr get_user(long long uid);
  void kick_user(long long uid);
  void kick_user_without_lock(long long uid);
  void update_game_info(long long uid);
  void update_game_info_without_lock(long long uid);
  
  void destory() { server_ = nullptr; }

  std::atomic<bool> is_on_;
  std::mutex m;

  SocketServer<WS>* server_;
};

#endif
