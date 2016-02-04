//#include "server_ws.hpp"
#include "log4cxx_md.hpp"
#include "cd_handler_md.hpp"
#include "cd_user_md.hpp"
#include "json11.hpp"
#include <time.h>
using namespace std;
using namespace json11;

typedef SocketServer<WS> WsServer;
//typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;

int main() {

  log4cxx_md::get().init();
  _logt("log test");
  _plogt("packet log test");

  /*
  std::thread t( [] {
      //vs_room_md::get().run(8);
    });
  */
  
  std::thread t2( [] {
      cd_user_md::get().start_check_alive();
    });

  bool r = cd_handler_md::get().init();
  if(!r) {
    std::cout << "[error] fail cd_handler_md init" << std::endl;
    return 1;
  }
    //WebSocket (WS)-server at port 8080 using 4 threads
    WsServer server(8080, 8);
    
    //Example 1: echo WebSocket endpoint
    //  Added debug messages for example use of the callbacks
    //  Test with the following JavaScript:
    //    var ws=new WebSocket("ws://localhost:8080/echo");
    //    ws.onmessage=function(evt){console.log(evt.data);};
    //    ws.send("test");
    auto& echo=server.endpoint["^/echo/?$"];
    
    echo.onmessage=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {

      //std::cout << connection->cd_user_ptr->name << std::endl;

      auto message_str=message->string();
      std::cout << "[payload]: " << message_str << std::endl;
      string err;
      auto payload = Json::parse(message_str, err);

      if (!err.empty()) {
	std::cout<< "[error] fail to parse json " << err.c_str() << std::endl;
      } else {

	std::string h = payload["type"].string_value();
	
	if ( cd_handler_md::get().m.find(h) == cd_handler_md::get().m.end() ) {
	  std::cout << "[error] 핸들러 없음" << std::endl;
	  server.send_close(connection, 2);
	} else {

	  bool r = cd_handler_md::get().m[h](connection->cd_user_ptr, payload);
	  if(!r) {
	    std::cout << "[error] 핸들러 콜백 처리중" << std::endl;
	  }

	}
      }



     
      /* 
      for(auto a_connection: server.get_connections()) {
	std::cout << "size: " << server.get_connections().size() << std::endl;
	auto send_stream=make_shared<WsServer::SendStream>();
	*send_stream << message_str;

	server.send(a_
connection, send_stream);
      }
*/

        //WsServer::Message::string() is a convenience function for:
        //stringstream data_ss;
        //data_ss << message->rdbuf();
        //auto message_str = data_ss.str();
        /*
        cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;
                
        cout << "Server: Sending message \"" << message_str <<  "\" to " << (size_t)connection.get() << endl;
        auto send_stream=make_shared<WsServer::SendStream>();
        *send_stream << message_str;
     
      // 비동기로 커넥션 한테 다시 보내주기
        server.send(connection, send_stream, [](const boost::system::error_code& ec) {
            if(ec) {
                cout << "Server: Error sending message. " <<
                //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                        "Error: " << ec << ", error message: " << ec.message() << endl;
            }
        });
	*/
    };
    
    echo.onopen=[&](shared_ptr<WsServer::Connection> connection) {


        cout << "Server: Opened connection " << (size_t)connection.get() << endl;

      	
	std::shared_ptr<cd_user> user = std::make_shared<cd_user>(server, connection);
	connection->cd_user_ptr = user;
	// 잠시 고유 아이디로 설정
	user->set_uid((size_t)connection.get());
	if(cd_user_md::get().add_user(user->get_uid(), user)) {
	  std::cout << "[debug] 유저매니져 유저 추가 성공" << std::endl;
	  std::cout << "[debug] 유저매니져 사이즈: " << cd_user_md::get().get_users_size() << std::endl;
	} else {
	  std::cout << "[error] 유저매니져 유저 추가 실패" << std::endl;
	}

	//vs_room_md::get().users.push_back(user);
	//connection->cd_user_ptr = std::unique_ptr<cd_user>(new cd_user(server, connection));

    };
    
    //See RFC 6455 7.4.1. for status codes
    echo.onclose=[](shared_ptr<WsServer::Connection> connection, int status, const string& reason) {
        cout << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status << endl;
	if(cd_user_md::get().remove_user(connection->cd_user_ptr->get_uid())) {

	std::cout << "[debug] 유저매니져 유저 삭제 성공" << std::endl;
	std::cout << "[debug] 유저매니져 사이즈: " << cd_user_md::get().get_users_size() << std::endl;
	} else {
	  std::cout << "[error] 유저매니져 유저 삭제 실패" << std::endl;
	}
	connection->cd_user_ptr->destory();
	connection->cd_user_ptr = nullptr;
    };
    
    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    echo.onerror=[](shared_ptr<WsServer::Connection> connection, const boost::system::error_code& ec) {
        cout << "Server: Error in connection " << (size_t)connection.get() << ". " << 
                "Error: " << ec << ", error message: " << ec.message() << endl;

	if(cd_user_md::get().remove_user(connection->cd_user_ptr->get_uid())) {
	std::cout << "[debug] 유저매니져 유저 삭제 성공" << std::endl;
	std::cout << "[debug] 유저매니져 사이즈: " << cd_user_md::get().get_users_size() << std::endl;	
	} else {
	  std::cout << "[error] 유저매니져 유저 삭제 실패" << std::endl;	  
	}
	connection->cd_user_ptr->destory();
	connection->cd_user_ptr = nullptr;
    };
    

    /*
    auto& echo_all=server.endpoint["^/echo_all/?$"];
    echo_all.onmessage=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
        //To receive message from client as string (data_ss.str())
        auto message_str=message->string();
        //WsServer::Message::string() is a convenience function for:
        //stringstream data_ss;
        //data_ss << message->rdbuf();
        //auto message_str = data_ss.str();
        
        //echo_all.get_connections() can also be used to solely receive connections on this endpoint
        for(auto a_connection: server.get_connections()) {
            auto send_stream=make_shared<WsServer::SendStream>();
            *send_stream << message_str;
            
            //server.send is an asynchronous function
            server.send(a_connection, send_stream);
        }
    };
    */
    
    thread server_thread([&server](){
        server.start();
    });
    
    //Wait for server to start so that the client can connect
    this_thread::sleep_for(chrono::seconds(1));
    

    
    server_thread.join();
    cd_user_md::get().stop_check_alive();
    t2.join();
    //t.join();
    
    return 0;
}
