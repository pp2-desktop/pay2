#ifndef PLAY_MD_HPP
#define PLAY_MD_HPP

#include <functional>
#include <map>
#include "json11.hpp"
#include "cd_user.hpp"
#include "game_info.hpp"

class room;
typedef std::shared_ptr<cd_user> user_ptr;
typedef std::shared_ptr<room> room_ptr;

using namespace json11;

class room {

public:

  enum room_status { lobby, playing };

  room(int id, user_ptr user, std::string title, std::string password);
  ~room();

  void leave_master();
  void leave_opponent();

  void ready_game();
  bool is_ready_game();
  void start_game();
  void ready_stage(user_ptr user);
  void check_point(user_ptr user, int user_stage_count, vec2 point);

  std::mutex m;
  user_ptr master_;
  user_ptr opponent_;

  std::atomic<bool> is_ready_;
  room_status status_;
  int id_;
  std::string title_;
  std::string password_;

  std::atomic<bool> is_playing_;
  std::unique_ptr<game_info> game_info_ptr;
};

class play_md {

public:  

  play_md();
  ~play_md();
  
  bool init();

  int create_room(user_ptr user, std::string title, std::string password);
  void destroy_room(int rid);
  void destroy_playing_room(int rid, long long winner_uid, int winner_score, long long loser_uid, int loser_score);
  int find_quick_join_room();

  bool join_user(int rid, user_ptr user);
  void leave_user(user_ptr user);
  void update_room_info(int rid, std::string type);

  void send_room_list(user_ptr user);

  float get_score_percentage(float score);
  std::tuple<int, int> earn_score(int my_score, int opponent_score);
  void update_score(long long winner_uid, int winner_score, long long loser_uid, int loser_score);

  static play_md& get() {
    static play_md obj;
    return obj;
  }

  std::mutex m;
  //std::map<size_t, user_ptr> users_;
  std::map<int, room_ptr> rooms_;

  int rid_;
};

#endif
