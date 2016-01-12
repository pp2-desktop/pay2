#ifndef CD_USER_MD_HPP
#define CD_USER_MD_HPP

#include <functional>
#include <map>
#include "json11.hpp"
#include "cd_user.hpp"

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
  bool add_user(size_t uid, user_ptr user);
  bool remove_user(size_t uid);
  bool get_users_size();

  std::atomic<bool> is_on_;
  std::mutex m;
};

#endif
