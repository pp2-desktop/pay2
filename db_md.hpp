#ifndef DB_MD_HPP
#define DB_MD_HPP

#include <functional>
#include <map>
#include <memory>
#include "db/MySQLConnection.hpp"

class db_md {

public:  

  db_md();
  ~db_md();

  bool init(std::string host, std::string id, std::string password, int connection_cnt=10);
  std::shared_ptr<sql::ResultSet> execute_query(std::string query);
  bool execute(std::string query);

  static db_md& get() {
    static db_md obj;
    return obj;
  }

  std::mutex m;

  std::shared_ptr<MySQLConnectionFactory> mysql_connection_factory;
  std::shared_ptr<ConnectionPool<MySQLConnection>> mysql_pool;

};

#endif
