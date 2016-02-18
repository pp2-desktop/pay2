#include "img_info.hpp"
#include "db_md.hpp"
#include <random>

img_info::img_info() {

}

img_info::~img_info() {

}

int img_info::get_img_count() {

  std::string query = "call get_img_count()";

  auto rs = db_md::get().execute_query(query);

  while(rs->next()) {
    img_count_ = rs->getInt("img_count");
  }

  return img_count_;
}

std::vector<int> img_info::generate_ids(int count) {
  auto end_count = get_img_count();
  std::vector<int> ids;

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> dist(0, end_count);

  for (int i=0; i<count; ++i) {
    bool is_find = false;
    while(!is_find) {
      auto id = static_cast<int>(dist(mt));
      auto it = std::find(ids.begin(), ids.end(), id);
      //std::cout << "id: " << id << std::endl;
      if(it == ids.end()) {
	ids.push_back(id);
	is_find = true;
	//std::cout << "삽입 성공 id: " << id << std::endl;
      }
    }
  }

  return ids;
}
