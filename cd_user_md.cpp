#include "cd_user_md.hpp"
#include "play_md.hpp"
#include <thread>
#include <chrono>
#include "db_md.hpp"

#define WAIT_SEC 5

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
        if(static_cast<time_t>(n - kv.second->get_alive_t()) > WAIT_SEC) {

	  if(server_) {
	    std::cout << "[noti] 킥 당한 유저아이디: " << user->get_uid() << std::endl;
            std::cout << "킥 유저 시간 간격" << n - kv.second->get_alive_t() << std::endl;
	    server_->send_close(user->connection_, 2);
            user->destory();
            kick_user_without_lock(user->get_uid());
	  } else {
	    std::cout << "[error] server is nullptr" << std::endl;
	  }
         
        } else {
          std::cout << "[debug] 연결중 uid: " << user->get_uid() << std::endl;
          std::cout << "커넥션 유저 시간 간격" << n - kv.second->get_alive_t() << std::endl;
	 json11::Json res = json11::Json::object {
            { "type", "update_alive_noti" },
	 };
	 user->send2(res);
        }
      }
    }

    //std::cout << "[debug] wait for 30 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
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

void cd_user_md::update_game_info(long long uid) {
  std::lock_guard<std::mutex> lock(m);
  auto user_ptr = get_user(uid);
  if(user_ptr) {
    std::string query = "call get_game_info(" + std::to_string(uid) + ")";
    auto rs = db_md::get().execute_query(query);
    while(rs->next()) {
      auto score = rs->getInt("score");
      auto win_count = rs->getInt("win_count");
      auto lose_count = rs->getInt("lose_count");
      auto ranking = rs->getInt("ranking");

      user_ptr->set_score(score);
      user_ptr->set_win_count(win_count);
      user_ptr->set_lose_count(lose_count);
      user_ptr->set_ranking(ranking);

      json11::Json noti = json11::Json::object {
	{ "type", "update_game_info_noti" },
	{ "result", true },
	{ "score", score },
	{ "win_count", win_count },
	{ "lose_count", lose_count },
	{ "ranking", ranking }
      };
      user_ptr->send2(noti);
    }
  }

}

void cd_user_md::update_game_info_without_lock(long long uid) {
  auto user_ptr = get_user(uid);
  if(user_ptr) {
    std::string query = "call get_game_info(" + std::to_string(uid) + ")";
    auto rs = db_md::get().execute_query(query);
    while(rs->next()) {
      auto score = rs->getInt("score");
      auto win_count = rs->getInt("win_count");
      auto lose_count = rs->getInt("lose_count");
      auto ranking = rs->getInt("ranking");

      user_ptr->set_score(score);
      user_ptr->set_win_count(win_count);
      user_ptr->set_lose_count(lose_count);
      user_ptr->set_ranking(ranking);

      json11::Json noti = json11::Json::object {
	{ "type", "update_game_info_noti" },
	{ "result", true },
	{ "score", score },
	{ "win_count", win_count },
	{ "lose_count", lose_count },
	{ "ranking", ranking }
      };
      user_ptr->send2(noti);
    }
  }

}
