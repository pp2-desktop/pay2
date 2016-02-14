#include "cd_user_md.hpp"
#include "play_md.hpp"
#include <thread>
#include <chrono>

#define WAIT_SEC 30

cd_user_md::cd_user_md(): is_on_(true) {

}

cd_user_md::~cd_user_md() {

}

void cd_user_md::start_check_alive() {

  while(is_on_) {
    auto n = time(NULL); 

    {
      std::lock_guard<std::mutex> lock(m);
      for (auto& kv : users_) {
        auto user = kv.second;
        //std::cout << kv.first << " has value " << kv.second << std::endl;
        if(static_cast<time_t>(n - kv.second->get_alive_t()) > WAIT_SEC+10) {

	  if(server_) {
	    std::cout << "[noti] kick user uid: " << user->get_uid() << std::endl;
	    server_->send_close(user->connection_, 2);
            user->destory();
            kick_user_without_lock(user->get_uid());
	  } else {
	    std::cout << "[error] server is nullptr" << std::endl;
	  }
         
        } else {
          std::cout << "[debug-check_alive] 연결중 uid: " << user->get_uid() << std::endl;
	 json11::Json res = json11::Json::object {
            { "type", "update_alive_noti" },
	 };
	 user->send2(res);
        }
      }
    }

    std::cout << "[debug] wait for 30 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(WAIT_SEC));
  }
}

void cd_user_md::stop_check_alive() {
  //is_on_ = false;
}

bool cd_user_md::add_user(long long uid, user_ptr user) {
  std::lock_guard<std::mutex> lock(m);
  auto it = users_.find(uid);
  if(it != users_.end()) {
    return false;
  }
  
  users_[uid] = user;
  return true;
}

bool cd_user_md::remove_user(long long uid) {
  std::lock_guard<std::mutex> lock(m);
  auto it = users_.find(uid);
  if(it == users_.end()) {
    return false;
  }
  users_.erase (it);   
  
  return true;
}

bool cd_user_md::get_users_size() {
  return users_.size();
}

user_ptr cd_user_md::get_user(long long uid) {
  auto it = users_.find(uid);
  if(it != users_.end()) {
    return it->second;
  }
  return nullptr;
}

void cd_user_md::kick_user(long long uid) {
  std::lock_guard<std::mutex> lock(m);
  auto user_ptr = get_user(uid);
  if(user_ptr) {
    server_->send_close(user_ptr->connection_, 2);
    user_ptr->destory();
  }
}

void cd_user_md::kick_user_without_lock(long long uid) {
  auto user_ptr = get_user(uid);
  if(user_ptr) {
    user_ptr->destory();
  }
}
