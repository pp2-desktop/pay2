#include "ConnectionPool.hpp"
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>

class MySQLConnection : public Connection {

 public:


  ~MySQLConnection() {

    if(this->sql_connection) {

      //_DEBUG("MYSQL Destruct");

      this->sql_connection->close();
      this->sql_connection.reset();     // Release and destruct

    }

  };

  std::shared_ptr<sql::Connection> sql_connection;
};


class MySQLConnectionFactory : public ConnectionFactory {

 public:
  MySQLConnectionFactory(std::string server, std::string username, std::string password) {

    this->server=server;
    this->username=username;
    this->password=password;

  };

  // Any exceptions thrown here should be caught elsewhere
  std::shared_ptr<Connection> create() {

    // Get the driver
    sql::Driver *driver;
    driver=get_driver_instance();

    // Create the connection
    std::shared_ptr<MySQLConnection>conn(new MySQLConnection());

    // Connect
    conn->sql_connection = std::shared_ptr<sql::Connection>(driver->connect(this->server,this->username,this->password));
    conn->sql_connection->setSchema("pay2");
    //conn->setAutoCommit(0);

    return std::static_pointer_cast<Connection>(conn);
  };

 private:
  std::string server;
  std::string username;
  std::string password;
};
/*
clientMultiStatements |
   clientMultiResults |
*/
