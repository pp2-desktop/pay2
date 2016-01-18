#ifndef LOBBY_MD_HPP
#define LOBBY_MD_HPP

#include <functional>
#include <map>
#include "json11.hpp"
#include "cd_user.hpp"

typedef std::shared_ptr<cd_user> user_ptr;

using namespace json11;

class lobby_md {

public:  

  std::map<size_t, user_ptr> users_;

  lobby_md();
  ~lobby_md();
  
  bool init();
  bool join_user(user_ptr user);
  bool leave_user(user_ptr user);
  void send_chat(Json payload);

  static lobby_md& get() {
    static lobby_md obj;
    return obj;
  }

  std::mutex m;
};

#endif
