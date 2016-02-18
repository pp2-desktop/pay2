#include "game_info.hpp"
#include "play_md.hpp"
#include "db_md.hpp"
#include <iostream>

vec2::vec2() {
  x = 0;
  y = 0;
}

vec2::vec2(int x0, int y0) {
  x = x0;
  y = y0;
}

vec2::~vec2() {

}

// ``````````````````````````````````
// stage
// ..................................

stage::stage() {
  master_find_count = 0;
  opponent_find_count = 0;
  is_master_ready = false;
  is_opponent_ready = false;
}

stage::~stage() {

}

void stage::set_stage_info(std::string img, std::vector<vec2> hidden_points) {
  // db에서 정보 가져오기
  //hidden_points = hidden_points;
  img = img;

  for(auto point : hidden_points) {
    auto info = std::tuple<user_type, vec2>(user_type::none, point);
    hidden_point_infos.push_back(info);
  }
}

bool stage::check_point(user_type type, vec2 point) {

  auto index = 0;
  for(auto& point_info : hidden_point_infos) {
    if(std::get<0>(point_info) == user_type::none && std::get<1>(point_info) == point) {
      //point_info = std::tuple<user_type, vec2>(type, point);

      std::cout <<"index: " << index << std::endl;
      std::get<0>(hidden_point_infos[index]) = type;

      if(type == user_type::master) {
        master_find_count++; 
      }
      else {
        opponent_find_count++;
      }

      return true;
    }
    index++;
  }
  return false;
}

bool stage::is_all_ready() {
  if(is_master_ready && is_opponent_ready) return true;

  return false;
}

user_type stage::check_stage_end() {
  
  if(master_find_count + opponent_find_count == hidden_point_infos.size()) {
    if(master_find_count > opponent_find_count) {
      return user_type::master;
    } else {
      return user_type::opponent;
    }
  }
  /*
  if(master_find_count >= 3) return user_type::master;
  
  if(opponent_find_count >=3) return user_type::opponent;
  */
  return user_type::none;
}

// ``````````````````````````````````
// game_info
// ..................................

game_info::game_info() : max_stage_count(5) {
  stage_count = 0;
  master_win_count = 0;
  opponent_win_count = 0;
}

game_info::~game_info() {

}

std::vector<stage> game_info::query_game_infos() {
  // 경기 시작전 모든 정보들을 가져온다.
  stages.clear();

  std::cout << "stages.size: " << stages.size() << std::endl;

  // db에서 정보 가져오기
  auto ids = play_md::get().img_info_.generate_ids(max_stage_count);
  std::string ids_query = "";

  for(unsigned i=0; i<ids.size(); i++) {
    if(i == ids.size()-1) {
      ids_query += std::to_string(ids[i]);
    } else {
      ids_query += std::to_string(ids[i]) + ",";
    }
  }

  std::string query = "call get_img_info("+ ids_query + ")";

  std::cout << "query: " << query << std::endl;

  auto rs = db_md::get().execute_query(query);

  while(rs->next()) {
    stage game_stage;

    std::cout << "@@ id: " << rs->getInt("id") << "@@" <<  std::endl;
    game_stage.img = rs->getString("name");

    auto point_count = rs->getInt("point_count");
    for(auto i=0; i<point_count; i++) {
      std::string point_x = "point" + std::to_string(i) + "_x";
      std::string point_y = "point" + std::to_string(i) + "_y";
      game_stage.hidden_point_infos.push_back(std::tuple<user_type, vec2>(none, vec2(rs->getInt(point_x), rs->getInt(point_y))));
    }
    stages.push_back(game_stage);
  }

  return stages;
}

bool game_info::check_game_end() {  
  // 마스터 승리 - 수정하기
  if(master_win_count > 2) return true;
  // 상대측 승리
  if(opponent_win_count > 2) return true;

  return false;
}

user_type game_info::get_game_winner() {
  if(master_win_count > opponent_win_count) {
    return user_type::master;
  }
  return user_type::opponent;
}

int game_info::ready_stage(user_type type) {
  if(type == user_type::master) {
    stages[stage_count].is_master_ready = true;
  } else {
    stages[stage_count].is_opponent_ready = true;
  }

  if(stages[stage_count].is_all_ready()) {
    std::cout << "둘다 스테이지 준비 완료" << std::endl;
    return stage_count;
  }

  std::cout << "스테이지 준비안돔" << std::endl;
  return -1;
}

// 스테이지 끝난거 알려줌, 승자 유저타이브,  포인트 찾았는지, 유저타입
check_point_rtn game_info::check_point(user_type type, vec2 v) {

  check_point_rtn rtn;

  // 포인트 찾았는지 먼저 검사
  if(stages[stage_count].check_point(type, v)) {
    rtn.is_find_point = true;
    rtn.find_point_user_type = type;

    auto r = stages[stage_count].check_stage_end();

    if(r == user_type::master) {
      rtn.is_stage_end = true;
      rtn.stage_user_type = user_type::master;
      master_win_count++;
    } else if(r == user_type::opponent) {
      rtn.is_stage_end = true;
      rtn.stage_user_type = user_type::opponent;
      opponent_win_count++;
    }
    return rtn;
  }
  
  // 없는 결과값
  return rtn;
}
