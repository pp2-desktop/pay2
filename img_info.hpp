#ifndef IMG_INFO_HPP
#define IMG_INFO_HPP

#include <vector>

class img_info {

  int img_count_;

public:
  img_info();
  ~img_info();

  int get_img_count();
  std::vector<int> generate_ids(int count=5);
  
};

#endif
