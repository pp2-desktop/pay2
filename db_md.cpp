#include "db_md.hpp"
#include <iostream>

db_md::db_md() {
  mysql_connection_factory = nullptr;
  mysql_pool = nullptr;
}

db_md::~db_md() {

}

bool db_md::init(std::string host, std::string id, std::string password, int connection_cnt) {

std::shared_ptr<MySQLConnectionFactory> mysql_connection_factory(new MySQLConnectionFactory(host, id, password));

  std::shared_ptr<ConnectionPool<MySQLConnection>> mysql_pool(new ConnectionPool<MySQLConnection>(connection_cnt, mysql_connection_factory));

  this->mysql_connection_factory = mysql_connection_factory;
  this->mysql_pool = mysql_pool;

  
  if(!this->mysql_connection_factory) return false;
  if(!this->mysql_pool) return false;

  return true;
}

std::shared_ptr<sql::ResultSet> db_md::execute_query(std::string query) {
  std::shared_ptr<sql::ResultSet> rs= nullptr;

  std::shared_ptr<MySQLConnection> conn = mysql_pool->borrow();

  try {
    std::unique_ptr<sql::Statement> stmt_ptr2(conn->sql_connection->createStatement());

    if(stmt_ptr2) {
      rs = std::shared_ptr<sql::ResultSet>(stmt_ptr2->executeQuery(query));
      stmt_ptr2->getMoreResults();
    }
  } catch (sql::SQLException& e) {
    std::cout << "ec: " << e.getErrorCode() << std::endl;
    std::cout << "Invalid Argument: " << e.what() << std::endl;
    std::cout << "SQLState: " << e.getSQLState() << std::endl;
  }

  db_md::get().mysql_pool->unborrow(conn);
  return rs;
}

bool db_md::execute(std::string query) {
  std::shared_ptr<MySQLConnection> conn = mysql_pool->borrow();
  auto rtn = false;
  try {
    std::unique_ptr<sql::Statement> stmt_ptr2(conn->sql_connection->createStatement());

    if(stmt_ptr2) {
      rtn = stmt_ptr2->execute(query);
      if(!rtn) rtn = true;
      stmt_ptr2->getMoreResults();
    }
  } catch (sql::SQLException& e) {
    std::cout << "ec: " << e.getErrorCode() << std::endl;
    std::cout << "Invalid Argument: " << e.what() << std::endl;
    std::cout << "SQLState: " << e.getSQLState() << std::endl;
  }

  db_md::get().mysql_pool->unborrow(conn);
  return rtn;
}
