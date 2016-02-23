#ifndef GAME_INFO_HPP
#define GAME_INFO_HPP

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "json11.hpp"
#include <atomic>

//typedef std::shared_ptr<cd_user> user_ptr;

using namespace json11;

struct vec2 {
  vec2();
  vec2(int x0, int y0);
  ~vec2();
  int x;
  int y;
  bool operator==(vec2 v) {
    if(x == v.x && y == v.y) return true;

    return false;
  }
};

enum user_type { none=0, master, opponent };

struct check_point_rtn {
  bool is_stage_end;
  user_type stage_user_type;
  bool is_find_point;
  user_type find_point_user_type;
  check_point_rtn() {
    is_stage_end = false;
    stage_user_type = none;
    is_find_point = false;
    find_point_user_type = none;
  }
  ~check_point_rtn() {}
};

class stage {
public:
  stage();
  ~stage();

  void set_stage_info(std::string img, std::vector<vec2> hidden_points);
  bool check_point(user_type type, vec2 point);
  user_type check_stage_end();
  bool is_all_ready();

  unsigned int master_find_count;
  unsigned int opponent_find_count;
  bool is_master_ready;
  bool is_opponent_ready;

  std::vector<std::tuple<user_type, vec2>> hidden_point_infos;
  std::string img;
};

class game_info {

public:

  game_info();
  ~game_info();

  std::vector<stage> query_game_infos();
  bool check_game_end();
  user_type get_game_winner();
  check_point_rtn check_point(user_type type, vec2 v);

  int ready_stage(user_type type);
  int get_perfect_stage_count(user_type type);

  void dummy_stages();

  std::vector<stage> stages;

  const int max_stage_count;
  int stage_count;
  int master_win_count;
  int opponent_win_count;
  //std::mutex m;
};

#endif
