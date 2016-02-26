#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "user_info_req.hpp"
#include "../cd_user_md.hpp"
#include "../db_md.hpp"
#include <random>
#include <vector>

bool login_req(std::shared_ptr<cd_user> user, Json payload) {
  std::cout << "login_req called" << std::endl;
  std::string uid = payload["uid"].string_value();
  std::string name = payload["name"].string_value();
  std::string password = payload["password"].string_value();

  // 시간 갱신
  user->update_alive_t();

  // 중복 로그인 체크


  //auto uid = std::stoll(uid_str);
  // query
  std::shared_ptr<MySQLConnection> conn = db_md::get().mysql_pool->borrow();

  try {
    std::unique_ptr<sql::Statement> stmt_ptr2(conn->sql_connection->createStatement());
    std::string query = "call get_game_info(" + uid + ")";

    if(stmt_ptr2) {
      auto res_ptr = std::shared_ptr<sql::ResultSet>(stmt_ptr2->executeQuery(query));
      stmt_ptr2->getMoreResults();
      //auto res_ptr = std::shared_ptr<sql::ResultSet>(stmt_ptr2->execute(query));

      while(res_ptr->next()) {
	auto score = res_ptr->getInt("score");
	auto win_count = res_ptr->getInt("win_count");
	auto lose_count = res_ptr->getInt("lose_count");
	auto ranking = res_ptr->getInt("ranking");

        user->set_score(score);
        user->set_win_count(win_count);
        user->set_lose_count(lose_count);
        user->set_ranking(ranking);

	json11::Json res = json11::Json::object {
	  { "type", "login_res" },
	  { "result", true },
	  { "score", score },
	  { "win_count", win_count },
	  { "lose_count", lose_count },
	  { "ranking", ranking }
	};
	user->send2(res);

	user->set_uid(std::stol(uid));
        user->set_name(name);
	if(cd_user_md::get().add_user(user->get_uid(), user)) {
	  std::cout << "[debug] 유저매니져 유저 추가 성공" << std::endl;
	  std::cout << "[debug] 유저매니져 사이즈: " << cd_user_md::get().get_users_size() << std::endl;
	}

        db_md::get().mysql_pool->unborrow(conn);
        return true;
      }
      // 유저가 존재하지 않음
      json11::Json res = json11::Json::object {
        { "type", "login_res" },
        { "result", false }
      };
      user->send2(res);
      db_md::get().mysql_pool->unborrow(conn);
      return false;
    }

  } catch (sql::SQLException& e) {
    std::cout << "ec: " << e.getErrorCode() << std::endl;
    std::cout << "Invalid Argument: " << e.what() << std::endl;
    std::cout << "SQLState: " << e.getSQLState() << std::endl;
    json11::Json res = json11::Json::object {
      { "type", "login_res" },
      { "result", false }
    };
    user->send2(res);
    db_md::get().mysql_pool->unborrow(conn);
    return false;
  }

  return true;
}

bool logout_req(std::shared_ptr<cd_user> user, Json payload) {

  json11::Json res = json11::Json::object {
    { "type", "logout_res" },
    { "result", true }
  };

  user->send2(res);
  return true;
}

bool update_alive_req(std::shared_ptr<cd_user> user, Json payload) {
  std::cout << "update alive req called" << std::endl;
  user->update_alive_t();
  return true;
}

bool update_alive_noti(std::shared_ptr<cd_user> user, Json payload) {
  std::cout << "update alive noti called" << std::endl;
  user->update_alive_t();
  return true;
}

bool get_user_info_req(std::shared_ptr<cd_user> user, Json payload) {
  std::string uid = payload["uid"].string_value();
  // uid 기준으로 유저 정보를 가져옴

  return true;
}

bool create_guest_account_req(std::shared_ptr<cd_user> user, Json payload) {
  //static int guest_id = 100;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(1.0, 100000.0);
  auto rand_id = static_cast<long>(dist(mt));
  std::string name = "guest" + std::to_string(rand_id);
  std::string password = name;
  auto result = true;

  // query
  std::shared_ptr<MySQLConnection> conn = db_md::get().mysql_pool->borrow();

 try {
   std::unique_ptr<sql::Statement> stmt_ptr2(conn->sql_connection->createStatement());
   std::string query = "call join_guest_user('" +  name + "','" + password + "')";

   if(stmt_ptr2) {
     auto res_ptr = std::shared_ptr<sql::ResultSet>(stmt_ptr2->executeQuery(query));
     stmt_ptr2->getMoreResults();
     //auto res_ptr = std::shared_ptr<sql::ResultSet>(stmt_ptr2->execute(query));

     while(res_ptr->next()) {
       auto uid = res_ptr->getUInt64("uid");

       if(uid < 0) result = false;

       json11::Json res = json11::Json::object {
	 { "type", "create_guest_account_res" },
	 { "result", result },
	 { "uid", std::to_string(uid) },
	 { "name", name },
	 { "password", password }
       };
       user->send2(res);
     }
   }

   db_md::get().mysql_pool->unborrow(conn);
 } catch (sql::SQLException& e) {
   std::cout << "ec: " << e.getErrorCode() << std::endl;
   std::cout << "Invalid Argument: " << e.what() << std::endl;
   std::cout << "SQLState: " << e.getSQLState() << std::endl;
   json11::Json res = json11::Json::object {
     { "type", "create_guest_account_res" },
     { "result", false }
   };
   user->send2(res);
   db_md::get().mysql_pool->unborrow(conn);
 }

  return true;
}

bool update_game_info_noti(std::shared_ptr<cd_user> user, Json payload) {
  std::string uid = payload["uid"].string_value();

  if(uid == "") {
    std::cout << "[error] uid is not exist" << std::endl;
    return false;
  }

  std::string query = "call get_game_info(" + uid + ")";
  auto rs = db_md::get().execute_query(query);
  while(rs->next()) {

    auto score = rs->getInt("score");
    auto win_count = rs->getInt("win_count");
    auto lose_count = rs->getInt("lose_count");
    auto ranking = rs->getInt("ranking");

    json11::Json noti = json11::Json::object {
      { "type", "update_game_info_noti" },
      { "result", true },
      { "score", score },
      { "win_count", win_count },
      { "lose_count", lose_count },
      { "ranking", ranking }
    };
    user->send2(noti);
  }

  //cd_user_md::get().update_game_info(std::stol(uid));

  return true;
}

bool get_ranking_req(std::shared_ptr<cd_user> user, Json payload) {

  std::vector<json11::Json> ranking_infos;
  std::string query = "call get_game_ranking()";

  auto rs = db_md::get().execute_query(query);

  while(rs->next()) {
    std::string name = rs->getString("name");
    auto score = rs->getInt("score");
    auto win_count = rs->getInt("win_count");
    auto lose_count = rs->getInt("lose_count");
    auto ranking = rs->getInt("ranking");

    Json tmp = Json::object({
  	{ "name",  name },
	{ "score", score },
        { "win_count", win_count },
        { "lose_count", lose_count },
        { "ranking", ranking }
      });
    ranking_infos.push_back(tmp);
  }

 json11::Json res = json11::Json::object {
    { "type", "get_ranking_res" },
    { "ranking_infos", ranking_infos }
  };

  user->send2(res);

  return true;
}

bool check_version_req(std::shared_ptr<cd_user> user, Json payload) {
  auto is_update = false;
  std::string platform = payload["platform"].string_value();
  auto client_version = payload["version"].int_value();

  std::cout << "[debug] platform: " << platform << std::endl;

  if(client_version < 2) {
    is_update = false;
  }

 json11::Json res = json11::Json::object {
    { "type", "check_version_res" },
    { "is_update", is_update }
  };

  user->send2(res);

  return true;
}
