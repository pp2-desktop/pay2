#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "single_req.hpp"
#include "../cd_user_md.hpp"
#include "../db_md.hpp"
#include "../game_info.hpp"
#include "../img_info.hpp"
#include "../log4cxx_md.hpp"
#include <random>
#include <vector>

bool single_img_info_req(std::shared_ptr<cd_user> user, Json payload) {

  int stage_count = payload["stage_count"].int_value();
  std::string img = "";
  std::vector<int> points;

  std::string query = "call get_single_img_info("+ std::to_string(stage_count) + ")";

  std::cout << "query: " << query << std::endl;

  auto rs = db_md::get().execute_query(query);

  while(rs->next()) {
    std::cout << "@@ id: " << rs->getInt("id") << "@@" <<  std::endl;
    img = rs->getString("name");

    auto point_count = rs->getInt("point_count");
    for(auto i=0; i<point_count; i++) {
      std::string point_x = "point" + std::to_string(i) + "_x";
      std::string point_y = "point" + std::to_string(i) + "_y";

      auto x = rs->getInt(point_x);
      auto y = rs->getInt(point_y);
      std::cout << "x: " << x << std::endl;
      std::cout << "y: " << y << std::endl;
      points.push_back(x);
      points.push_back(y);
    }
  }
  
  json11::Json res = json11::Json::object {
    { "type", "single_img_info_res" },
    { "result", true },
    { "img", img },
    { "points", points }
  };
  user->send2(res);

  _logt("[single_img_info_req] stage_count: " << stage_count << ", address: " << user->connection_->remote_endpoint_address << ", port: " << user->connection_->remote_endpoint_port);

  return true;
}

bool max_stage_req(std::shared_ptr<cd_user> user, Json payload) {

  img_info tmp;
  auto img_count = tmp.get_img_count();

  json11::Json res = json11::Json::object {
    { "type", "max_stage_res" },
    { "max_stage_count", img_count }
  };
  user->send2(res);

  return true;
}
