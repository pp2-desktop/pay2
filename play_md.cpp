#include "play_md.hpp"
#include "lobby_md.hpp"
#include "db_md.hpp"
#include "cd_user_md.hpp"

room::room(int id, user_ptr user, std::string title, std::string password) {
  id_ = id;
  title_ = title;
  password_ = password;
  status_ = lobby;
  master_ = user;
  opponent_ = nullptr;
  is_ready_ = false;
  game_info_ptr = nullptr;
  is_playing_ = false;
}

room::~room() {

}

void room::leave_master() {
  if(!opponent_) return;

  auto user = opponent_;
  if(status_ == lobby) {
    json11::Json noti = json11::Json::object {
      { "type", "room_destroy_noti" }
    };
    user->send2(noti);
  } else {
    json11::Json noti = json11::Json::object {
      { "type", "room_destroy_noti" }
    };
    user->send2(noti);
  }
}

void room::leave_opponent() {
  if(!master_) return;

  auto user = master_;
  if(status_ == lobby) {
    json11::Json noti = json11::Json::object {
      { "type", "leave_opponent_noti" }
    };
    user->send2(noti);
    is_ready_ = false;
    opponent_ = nullptr;
  } else {
    json11::Json noti = json11::Json::object {
      { "type", "leave_playing_opponent_noti" }
    };
    user->send2(noti);

    is_playing_ = false;
    is_ready_ = false;
    status_ = lobby;
  }
}

void room::ready_game() {
  //std::lock_guard<std::mutex> lock(m);
  if(is_ready_) return;

  is_ready_ = true;
  auto user = master_;
  json11::Json noti = json11::Json::object {
    { "type", "ready_game_noti" }
  };
  user->send2(noti);
}

bool room::is_ready_game() {
  //std::lock_guard<std::mutex> lock(m);
  return is_ready_;
}

void room::start_game() {
  std::lock_guard<std::mutex> lock(m);

  is_playing_ = true;
  game_info_ptr.reset(new game_info);
  

  if(!master_) {
    json11::Json res = json11::Json::object {
      { "type", "start_game_res" },
      { "result", false }
    };
    opponent_->send2(res);
  }


  if(!opponent_) {
    json11::Json res = json11::Json::object {
      { "type", "start_game_res" },
      { "result", false }
    };
    master_->send2(res);
  }

  auto stages = game_info_ptr->query_game_infos();

  std::vector<json11::Json> stage_infos;

  for(auto stage : stages) {

    std::vector<int> points;

    for(auto point_info : stage.hidden_point_infos) {
      vec2 v = std::get<1>(point_info);
      points.push_back(v.x);
      points.push_back(v.y);
    }

    Json tmp = Json::object({
	{ "img",  stage.img },
	{ "points",  points } 
    });
    stage_infos.push_back(tmp);
  }

  //{img:"0.jpg", points:[10, 20, 30, 40, 50, 60, 70, 80, 20, 50]}
  // 여기 안에 5라운드 정보 담아서 보내주기
  json11::Json res = json11::Json::object {
    { "type", "start_game_res" },
    { "result", true },
    { "stage_infos", stage_infos }
  };

  master_->send2(res);
  opponent_->send2(res);
}

void room::ready_stage(user_ptr user) {
  std::lock_guard<std::mutex> lock(m);

  auto stage_count = -1;
  if(user->get_uid() == master_->get_uid()) {
    stage_count = game_info_ptr->ready_stage(user_type::master);
  } else {
    stage_count = game_info_ptr->ready_stage(user_type::opponent);
  }

  std::cout << "stage_count: " << stage_count << std::endl;

  if(stage_count < 0) return;
  
  // 유저 stage 시작하라고 알려줌
  json11::Json noti = json11::Json::object {
    { "type", "start_stage_noti" }, 
    { "stage_count", stage_count }
  };
  master_->send2(noti);
  opponent_->send2(noti);
  status_ = playing;
}

void room::check_point(user_ptr user, int user_stage_count, vec2 point) {
  std::lock_guard<std::mutex> lock(m);

  auto stage_count = game_info_ptr->stage_count;

  if(stage_count != user_stage_count) return;

  auto x = point.x;
  auto y = point.y;

  user_type type = user_type::master;
  std::string found_type_string = "master";

  if(user->get_uid() != master_->get_uid()) {
    type = user_type::opponent;
    found_type_string = "opponent";
  }

  auto r = game_info_ptr->check_point(type, point);

  if(!r.is_find_point) {
    std::cout << "[debug] 벌써 유저가 찾아서 중복처리해버림" << std::endl;
    return;
  }


  auto is_stage_end = r.is_stage_end;
  auto stage_winner = r.stage_user_type;

  auto is_game_end = game_info_ptr->check_game_end();

  auto stage_winner_string = "master";
  if(stage_winner != user_type::master) {
    stage_winner_string = "opponent";
  }

  // 게임이 끝난경우
  if(is_game_end) {
    std::string game_winner_string = "master";
    if(game_info_ptr->get_game_winner() != user_type::master) {
      game_winner_string = "opponent";
    }

    json11::Json res = json11::Json::object {
      { "type", "check_point_res" },
      { "is_game_end", true },
      { "game_winner", game_winner_string },
      { "is_stage_end", true },
      { "stage_winner", stage_winner_string },
      { "found_type", found_type_string },
      { "x", x },
      { "y", y },
      { "stage_count", stage_count }
    };

    master_->send2(res);
    opponent_->send2(res);
    is_playing_ = false;
    is_ready_ = false;
    status_ = lobby;

    if(game_winner_string == "master") {
      play_md::get().update_score(master_->get_uid(), master_->get_score(), opponent_->get_uid(), opponent_->get_score());
    } else {
      play_md::get().update_score(opponent_->get_uid(), opponent_->get_score(), master_->get_uid(), master_->get_score());
    }

    cd_user_md::get().update_game_info(master_->get_uid());
    cd_user_md::get().update_game_info(opponent_->get_uid());

    return;
  }

  // 스테이지만 끝난경우
  if(is_stage_end) {
    json11::Json res = json11::Json::object {
      { "type", "check_point_res" },
      { "is_game_end", false },
      { "is_stage_end", true },
      { "stage_winner", stage_winner_string },
      { "found_type", found_type_string },
      { "x", x },
      { "y", y },
      { "stage_count", stage_count }
    };

    master_->send2(res);
    opponent_->send2(res);

    if(game_info_ptr->stage_count < static_cast<int>(game_info_ptr->stages.size())) {
      game_info_ptr->stage_count++;
    }
    return;
  }



  json11::Json res = json11::Json::object {
    { "type", "check_point_res" },
    { "is_game_end", false },
    { "is_stage_end", false },
    { "found_type", found_type_string },
    { "x", x },
    { "y", y },
    { "stage_count", stage_count }
  };

  master_->send2(res);
  opponent_->send2(res);

}

//***********************************************************

play_md::play_md() {
  rid_ = 0;
}

play_md::~play_md() {

}

bool play_md::init() {

  return true;
}

int play_md::create_room(user_ptr user, std::string title, std::string password) {
  std::lock_guard<std::mutex> lock(m);
  rid_++;
  auto sp = std::make_shared<room>(rid_, user, title, password);
  rooms_[rid_] = sp;

  user->room_ptr = sp;

  //auto uid = user->get_uid();
  /*
  auto rs = db_md::get().execute_query("call get_game_info(" + std::to_string(uid) + ")");
  while(rs->next()) {
    auto score = rs->getInt("score");
    auto win_count = rs->getInt("win_count");
    auto lose_count = rs->getInt("lose_count");
  }
  */




  return rid_;
}

void play_md::destroy_room(int rid) {
  std::cout << "삭제 방 번호: " << rid << std::endl;
  std::cout << "삭제전 방 갯수: " << rooms_.size() << std::endl;

  rooms_.erase(rid);

  lobby_md::get().destroy_room_noti(rid);
  std::cout << "삭제후 방 갯수: " << rooms_.size() << std::endl;
  // 방장이니 로비유저들에게 방폭 사실을 알려줌
}

void play_md::destroy_playing_room(int rid, long long winner_uid, int winner_score, long long loser_uid, int loser_score) {
  std::cout << "destroy_playing_room called" << std::endl;
  std::cout << "삭제 방 번호: " << rid << std::endl;
  std::cout << "삭제전 방 갯수: " << rooms_.size() << std::endl;

  // 점수 처리 해줌
  auto scores = earn_score(winner_score, loser_score);
  auto add_score = std::get<0>(scores);

  scores = earn_score(loser_score, winner_score);
  auto sub_score = std::get<0>(scores);


  std::string query = "call update_users_score(" + std::to_string(winner_uid) + "," + std::to_string(add_score) + "," + std::to_string(loser_uid) + "," + std::to_string(sub_score)  +")";

  auto r = db_md::get().execute(query);

  if(!r) {
    std::cout << "[error] db 쿼리: " << query << std::endl;
  }


  rooms_.erase(rid);
  lobby_md::get().destroy_room_noti(rid);
  std::cout << "삭제후 방 갯수: " << rooms_.size() << std::endl;
}

int play_md::find_quick_join_room() {
  std::lock_guard<std::mutex> lock(m);

  for(auto& r : rooms_) {
    auto room = r.second;
   
    if(!room->is_playing_) {
      if(room->opponent_ == nullptr) {
        return room->id_;
      }
    }
  }

  return -1;
}

bool play_md::join_user(int rid, user_ptr user) {
  std::lock_guard<std::mutex> lock(m);

  auto it = rooms_.find(rid);
  if(it != rooms_.end()) {
    // 조인했는데 다른유저가 벌써 들어온 상황
    auto room = it->second;
    if(room->opponent_ != nullptr || room->status_ != room::lobby) {
      return false;
    }

    room->opponent_ = user;
    // 유저에게 방을 설정
    user->room_ptr = room;

    // 방장에거 유저가 들어온것이라 유저 정보 알려줌
    auto uid = user->get_uid();
    
    std::tuple<int, int> scores = earn_score(room->master_->get_score(), user->get_score());
    auto earn_score = std::get<0>(scores);
    auto lose_score = std::get<1>(scores);
 

    // 방장과 상대측 둘에게 각자의 정보를 보내줌 - 해야할것
    json11::Json noti = json11::Json::object {
      { "type", "join_opponent_noti" },
      { "earn_score", earn_score },
      { "lose_score", lose_score },
      { "name", user->get_name() },
      { "score", user->get_score() },
      { "win_count", user->get_win_count() },
      { "lose_count", user->get_lose_count() },
      { "facebookid", user->get_facebookid() },
      { "ranking", user->get_ranking() },
    };
    room->master_->send2(noti);
    return true;
  }

  return false;
}

void play_md::leave_user(user_ptr user) {
  std::lock_guard<std::mutex> lock(m);

  auto rid = -1;
  if(auto room_ptr = user->room_ptr.lock()) {
    rid = room_ptr->id_;
  }

  auto it = rooms_.find(rid);

  // 방이 존재함
  if(it != rooms_.end()) {
    auto room = it->second;
    if(room->status_ == room::lobby) {
      auto master = room->master_;
      auto opponent = room->opponent_;
      if(master && master->get_uid() == user->get_uid()) {
	room->leave_master();
	destroy_room(room->id_);
      
      } else if(opponent && opponent->get_uid() == user->get_uid()) {
	room->leave_opponent();
      
      } else {
	std::cout << "[error] 유저가 방에 있지 않음" << std::endl;
      }
    } else {
      std::shared_ptr<cd_user> user_ptr = nullptr;
      // 유저가 강제적으로 나갔으니 종료 처리해줌(방 삭제 등등)
      if(user->get_uid() == room->master_->get_uid()) {
        room->master_->room_ptr.reset();
        room->opponent_->room_ptr.reset();
        user_ptr = room->opponent_;
        destroy_playing_room(room->id_, room->opponent_->get_uid(), room->opponent_->get_score(), room->master_->get_uid(), room->master_->get_score());
      } else {
        user_ptr = room->master_;
        destroy_playing_room(room->id_, room->master_->get_uid(), room->master_->get_score(), room->opponent_->get_uid(), room->opponent_->get_score());
      }

      if(user_ptr) {
        json11::Json noti = json11::Json::object {
          { "type", "game_end_noti" }
        };
        user_ptr->send2(noti);
      }
    }
  }

  //user->room_ptr.reset();

  //if(user->room_ptr) user->room_ptr = nullptr;
}

void play_md::send_room_list(user_ptr user) {
  std::lock_guard<std::mutex> lock(m);

  std::vector<json11::Json> room_list;

  for(auto& room : rooms_) {

    auto room_ptr = room.second;

    if(room_ptr) {
      auto is_full = false;
      if(room_ptr->master_ && room_ptr->opponent_) {
	is_full = true;
      }

      Json tmp = Json::object({
  	{ "rid",  room_ptr->id_ },
  	{ "title", room_ptr->title_ },
	{ "password", room_ptr->password_ },
	{ "is_full", is_full }
      });
      room_list.push_back(tmp);
    }
  }

  json11::Json res = json11::Json::object {
    { "type", "room_list_res" },
    { "room_list", room_list }
  };

  user->send2(res);
}

float play_md::get_score_percentage(float score) {
  auto d = score / 400.0f;
  return 1 / (1 + std::pow(10, d));
}

std::tuple<int, int> play_md::earn_score(int my_score, int opponent_score) {
  auto score = static_cast<float>(my_score) - static_cast<float>(opponent_score);
  float r = get_score_percentage(score);
    
  float game_cnt = 30;
  auto win_score = std::ceil(game_cnt * (1.0f - r));
  auto lose_score = std::ceil(game_cnt * r);

  return std::tuple<int, int>(win_score, lose_score);
}

void play_md::update_score(long long winner_uid, int winner_score, long long loser_uid, int loser_score) {

  auto scores = earn_score(winner_score, loser_score);
  auto add_score = std::get<0>(scores);

  scores = earn_score(loser_score, winner_score);
  auto sub_score = std::get<0>(scores);

  std::string query = "call update_users_score(" + std::to_string(winner_uid) + "," + std::to_string(add_score) + "," + std::to_string(loser_uid) + "," + std::to_string(sub_score)  +")";

  auto r = db_md::get().execute(query);

  if(!r) {
    std::cout << "[error] db 쿼리: " << query << std::endl;
  }
}
