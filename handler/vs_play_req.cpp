#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include "vs_play_req.hpp"
#include "../cd_user.hpp"
#include "../vs_room.hpp"

using namespace json11;

bool round_info_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  //user_ptr->get_vs_room()->change_status(vs_room::PLAYING);
  user_ptr->get_vs_room()->vs_round_info_.pre_loading_round_info();
  std::vector<Json> round_infos;
  
  // 나중에 pre_loading_round_info를 기반으로 이 패킷을 만들어야함
  for(auto i=0; i<5; i++) {
    std::string img = "test.jpg";
    
    /*
    if(i==1) {
      img = "2.jpg";
    }
    */

    std::string img0 = "left_" + img;
    std::string img1 = "right_" + img;

    std::vector<int> points;
    points.push_back(113);
    points.push_back(131);

    points.push_back(281);
    points.push_back(269);

    points.push_back(506);
    points.push_back(306);



    Json round = Json::object({
	{ "img0", img0 },
	{ "img1", img1 },
	{ "point_infos", points } 
      });

    round_infos.push_back(round);
  }

  Json tmp = round_infos;
   
  Json res = Json::object({
      { "type", "round_info_res" },
      { "round_infos", tmp }
    });
  
  user_ptr->send2(res);

  return true;
}

bool start_round_req(std::shared_ptr<cd_user> user_ptr, Json payload) {

  int round_cnt = payload["round_cnt"].int_value();
  std::cout << "유저 라운드: " << round_cnt << std::endl;
  
  if(user_ptr->get_vs_room()->vs_round_info_.get_round_ready_cnt() < 1) {
    user_ptr->get_vs_room()->vs_round_info_.inc_round_ready_cnt();
    std::cout << "[debug] 1명 대기 상태" << std::endl;
  } else {
    user_ptr->get_vs_room()->vs_round_info_.inc_round_ready_cnt();
    std::cout << "[debug] 2명 대기 완료" << std::endl;    
    bool r = user_ptr->get_vs_room()->start_round();
    if(!r) {
      std::cout << "[error] start_round_res 패킷 전송 실패" << std::endl;    
    }
  }

  return true;
}

bool find_spot_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  int round_cnt = payload["round_cnt"].int_value();
  int index = payload["index"].int_value();

  if(user_ptr->get_is_master()) {
    user_ptr->get_vs_room()->find_spot(round_cnt, index, MASTER);
  } else {
    user_ptr->get_vs_room()->find_spot(round_cnt, index, OPPONENT);
  }

  return true;
}

bool play_to_lobby_req(std::shared_ptr<cd_user> user_ptr, Json payload) {
  user_ptr->get_vs_room()->play_to_lobby();

  Json res = Json::object({
      { "type", "play_to_lobby_res" }
    });
  
  user_ptr->send2(res);

  return true;
}

/*
bool opponent_ready_notify(std::shared_ptr<cd_user> user_ptr, Json) {

  auto room_ptr = user_ptr->get_vs_room_ptr();
  
  if(room_ptr) {
    auto master_user_ptr = room_ptr->master_user_ptr;
    if(master_user_ptr) {
      master_user_ptr->send2(json11::Json::object {
	  { "type", "opponent_ready_notify" }
	});
    } 
  }
  return true;
}

bool start_vs_game_req(std::shared_ptr<cd_user> user_ptr, Json) {

  std::cout << "게임 시작 메시지 받음" << std::endl;

  auto room_ptr = user_ptr->get_vs_room_ptr();

  auto res = json11::Json::object{ 
    { "type", "start_vs_game_res" }
  };
  
  if(room_ptr) {
    auto master_user_ptr = room_ptr->master_user_ptr;
    if(master_user_ptr) {
      master_user_ptr->send2(res);
    }
    auto opponent_user_ptr = room_ptr->master_user_ptr;
    if(opponent_user_ptr) {
      opponent_user_ptr->send2(res);
    }
  }
  
  return true;
}
*/
