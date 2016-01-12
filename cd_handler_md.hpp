#ifndef CD_HANDLER_MD_HPP
#define CD_HANDLER_MD_HPP

#include <functional>
#include <map>
#include "json11.hpp"
#include "cd_user.hpp"

typedef std::shared_ptr<cd_user> user_ptr;  

using namespace json11;

class cd_handler_md {

public:  

  std::map<std::string, std::function<bool(user_ptr, Json)>> m;

  cd_handler_md();
  ~cd_handler_md();

  static cd_handler_md& get() {
    static cd_handler_md obj;
    return obj;
  }

  bool init();
  bool add_payload(std::string, std::function<bool(user_ptr, Json)>);
};

#endif
