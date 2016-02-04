#include "play_md.hpp"
#include "lobby_md.hpp"

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
  std::lock_guard<std::mutex> lock(m);
  if(is_ready_) return;

  is_ready_ = true;
  auto user = master_;
  json11::Json noti = json11::Json::object {
    { "type", "ready_game_noti" }
  };
  user->send2(noti);
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
    // 방장과 상대측 둘에게 각자의 정보를 보내줌 - 해야할것
    json11::Json noti = json11::Json::object {
      { "type", "join_opponent_noti" }
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
      if(room->master_->get_uid() == user->get_uid()) {
	room->leave_master();
	destroy_room(room->id_);
      
      } else if(room->opponent_->get_uid() == user->get_uid()) {
	room->leave_opponent();
      
      } else {
	std::cout << "[error] 유저가 방에 있지 않음" << std::endl;
      }
    } else {

    }
  }

  //user->room_ptr.reset();

  //if(user->room_ptr) user->room_ptr = nullptr;
}
