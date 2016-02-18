#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "play_req.hpp"
#include "../cd_user.hpp"
#include "../lobby_md.hpp"
#include "../play_md.hpp"
#include "../game_info.hpp"

bool create_room_req(std::shared_ptr<cd_user> user, Json payload) {

  std::string title = payload["title"].string_value();
  std::string password = payload["password"].string_value();

  auto rid = play_md::get().create_room(user, title, password);
  
  json11::Json res = json11::Json::object {
    { "type", "create_room_res" },
    { "rid", rid }
  };
  user->send2(res);

  //lobby_md::get().leave_user(user);
  lobby_md::get().create_room_noti(rid, title, password);
  /*
  std::string uid = payload["uid"].string_value();
  auto r = lobby_md::get().join_user(user_ptr);
  json11::Json res = json11::Json::object {
    { "type", "join_lobby_res" },
    { "result", r }
  };
  user_ptr->send2(res);
  */
  return true;
}

bool quick_join_req(std::shared_ptr<cd_user> user, Json payload) {

  auto is_create = true;
  auto rid = play_md::get().find_quick_join_room();

  if(rid >= 0) {
    is_create = false;
  }

  json11::Json res = json11::Json::object {
    { "type", "quick_join_res" },
    { "is_create", is_create },
    { "rid", rid }
  };
  user->send2(res);

  return true;
}

bool join_room_req(std::shared_ptr<cd_user> user, Json payload) {

  auto rid = payload["rid"].int_value();
  auto result = play_md::get().join_user(rid, user);

  json11::Json res = json11::Json::object {
    { "type", "join_room_res" },
    { "result", result }
  };
  user->send2(res);

  if(result) lobby_md::get().full_room_noti(rid);
  // 전체에게 유저가 들어와서 꽛 찼다고 알려줌

  return true;
}

bool leave_room_req(std::shared_ptr<cd_user> user, Json payload) {
  play_md::get().leave_user(user);

  return true;
}

bool start_game_req(std::shared_ptr<cd_user> user, Json payload) {
  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->start_game();
  } else {
    json11::Json res = json11::Json::object {
      { "type", "start_game_res" },
      { "result", false }
    };
    user->send2(res);
  }

  return true;
}

bool ready_game_noti(std::shared_ptr<cd_user> user, Json payload) {
  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->ready_game();
  }
  return true;
}

bool ready_stage_noti(std::shared_ptr<cd_user> user, Json payload) {
 if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->ready_stage(user);
  }
  return true;
}

bool check_point_req(std::shared_ptr<cd_user> user, Json payload) {
  auto x = static_cast<float>(payload["x"].int_value());
  auto y = static_cast<float>(payload["y"].int_value());
  int stage_count = payload["stage_count"].int_value();

  if(auto room_ptr = user->room_ptr.lock()) {
    room_ptr->check_point(user, stage_count, vec2(x, y));
  }

  return true;
}

bool master_info_req(std::shared_ptr<cd_user> user, Json payload) {
  auto result = true;

  auto earn_score = 0;
  auto lose_score = 0;

  std::string name = "";
  auto score = 0;
  auto win_count = 0;
  auto lose_count = 0;
  auto ranking = 0;
  auto is_facebook_login = false;
  std::string facebookid = "";

  if(auto room_ptr = user->room_ptr.lock()) {
    auto master = room_ptr->master_;
    if(!master) {
      result = false;
    }

   std::tuple<int, int> scores = play_md::get().earn_score(user->get_score(), master->get_score());
   earn_score = std::get<0>(scores);
   lose_score = std::get<1>(scores);
   name = master->get_name();
   score = master->get_score();
   win_count = master->get_win_count();
   lose_count = master->get_lose_count();
   ranking = master->get_ranking();
   is_facebook_login = master->get_is_facebook_login();
   if(is_facebook_login) facebookid = master->get_facebookid();

  } else {
    result = false;
  }

  json11::Json res = json11::Json::object {
    { "type", "master_info_res" },
    { "result", result },
    { "earn_score", earn_score },
    { "lose_score", lose_score },
    { "name", name },
    { "score", score },
    { "win_count", win_count },
    { "lose_count", lose_count },
    { "facebookid", facebookid },
    { "ranking", ranking }
  };
  user->send2(res);

  return true;
}

bool opponent_info_req(std::shared_ptr<cd_user> user, Json payload) {
  auto earn_score = 0;
  auto lose_score = 0;

  std::string name = "";
  auto score = 0;
  auto win_count = 0;
  auto lose_count = 0;
  auto ranking = 0;
  auto is_facebook_login = false;
  std::string facebookid = "";

  if(auto room_ptr = user->room_ptr.lock()) {
    auto opponent = room_ptr->opponent_;

    if(!opponent) {
      json11::Json res = json11::Json::object {
        { "type", "opponent_info_res" },
        { "result", false }
      };
      user->send2(res);
      return true;
    }

    std::tuple<int, int> scores = play_md::get().earn_score(user->get_score(), opponent->get_score());
    earn_score = std::get<0>(scores);
    lose_score = std::get<1>(scores);
    name = opponent->get_name();
    score = opponent->get_score();
    win_count = opponent->get_win_count();
    lose_count = opponent->get_lose_count();
    ranking = opponent->get_ranking();
    is_facebook_login = opponent->get_is_facebook_login();
    if(is_facebook_login) facebookid = opponent->get_facebookid();

 
    json11::Json res = json11::Json::object {
      { "type", "opponent_info_res" },
      { "result", true },
      { "earn_score", earn_score },
      { "lose_score", lose_score },
      { "name", name },
      { "score", score },
      { "win_count", win_count },
      { "lose_count", lose_count },
      { "facebookid", facebookid },
      { "ranking", ranking }
    };
    user->send2(res);
  }

  return true;
}

bool kick_opponent_noti(std::shared_ptr<cd_user> user, Json payload) {
 if(auto room_ptr = user->room_ptr.lock()) {
   auto opponent = room_ptr->opponent_;
   if(opponent) {
     json11::Json noti = json11::Json::object {
       { "type", "kick_opponent_noti" }
     };
     opponent->send2(noti);
   }
 }

 return true;
}

bool check_ready_opponent_req(std::shared_ptr<cd_user> user, Json payload) {
  
  auto is_ready = false;
  if(auto room_ptr = user->room_ptr.lock()) {
    is_ready = room_ptr->is_ready_game();
  }
  
  json11::Json res = json11::Json::object {
    { "type", "check_ready_opponent_req" },
    { "is_ready", is_ready },
  };
  user->send2(res);
  
  return true;
}
