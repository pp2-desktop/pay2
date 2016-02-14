#include <deque>
#include <set>
#include <exception>
#include <string>
#include <memory>
#include <mutex>

struct ConnectionUnavailable : std::exception {

  char const* what() const throw() {
    return "Unable to allocate connection";
  };
};


class Connection {

public:
  Connection(){};
  virtual ~Connection(){};

};

class ConnectionFactory {

public:
  virtual std::shared_ptr<Connection> create()=0;
};

struct ConnectionPoolStats {

  size_t pool_size;
  size_t borrowed_size;

};

template<class T>
class ConnectionPool {

public:

  ConnectionPoolStats get_stats() {

    // Lock
    std::lock_guard<std::mutex> lock(this->io_mutex);

    // Get stats
    ConnectionPoolStats stats;
    stats.pool_size=this->pool.size();
    stats.borrowed_size=this->borrowed.size();
    return stats;
  };

  ConnectionPool(size_t pool_size, std::shared_ptr<ConnectionFactory> factory){

    // Setup
    this->pool_size=pool_size;
    this->factory=factory;

    // Fill the pool
    while(this->pool.size() < this->pool_size){

      this->pool.push_back(this->factory->create());
    }


  };

  ~ConnectionPool() {
  };

  std::shared_ptr<T> borrow(){

    // Lock
    std::lock_guard<std::mutex> lock(this->io_mutex);

    // Check for a free connection
    if(this->pool.size()==0){

      // Are there any crashed connections listed as "borrowed"?
      for(std::set<std::shared_ptr<Connection> >::iterator it=this->borrowed.begin(); it!=this->borrowed.end(); ++it){

        if((*it).unique()) {

          // This connection has been abandoned! Destroy it and create a new connection
          try {

            // If we are able to create a new connection, return it
            //_DEBUG("Creating new connection to replace discarded connection");
            std::shared_ptr<Connection> conn=this->factory->create();
            this->borrowed.erase(it);
            this->borrowed.insert(conn);
            return std::static_pointer_cast<T>(conn);

          } catch(std::exception& e) {

            // Error creating a replacement connection
            throw ConnectionUnavailable();
          }
        }
      }

      // Nothing available
      throw ConnectionUnavailable();
    }

    // Take one off the front
    std::shared_ptr<Connection>conn=this->pool.front();
    this->pool.pop_front();

    // Add it to the borrowed list
    this->borrowed.insert(conn);

    return std::static_pointer_cast<T>(conn);
  };
  void unborrow(std::shared_ptr<T> conn) {

    // Lock
    std::lock_guard<std::mutex> lock(this->io_mutex);

    // Push onto the pool
    this->pool.push_back(std::static_pointer_cast<Connection>(conn));

    // Unborrow
    this->borrowed.erase(conn);
  };

protected:
  std::shared_ptr<ConnectionFactory> factory;
  size_t pool_size;
  std::deque<std::shared_ptr<Connection> > pool;
  std::set<std::shared_ptr<Connection> > borrowed;
  std::mutex io_mutex;

};
