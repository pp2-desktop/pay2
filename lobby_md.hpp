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

  void create_room_noti(int rid, std::string title, std::string password);
  void destroy_room_noti(int rid);

  void full_room_noti(int rid);
  void start_game_noti(int rid);
  //void update_room_info_noti(std::string type, int rid);
  void send_chat_list(user_ptr user);

  static lobby_md& get() {
    static lobby_md obj;
    return obj;
  }

  std::mutex m;
};

#endif
