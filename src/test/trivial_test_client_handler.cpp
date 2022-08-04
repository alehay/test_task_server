#include <signal.h>
#include <sstream>
#include <string>
#include <functional>

#include "../server/tcp_server.hpp"
#include "../server/client_handler.hpp"


void signal_handler(int signum) {
  TCP_server::terminate = true;
  client_handler::terminate = true;
  client_list::GetInstanse()->close_all_socoekt();

    
  /* Close all open file descriptors */
  int x;
  for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
    close(x);
  }



}

int main(int argc, char* argv[]) {

  client_list * cli_list = client_list::GetInstanse();
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGTERM ,signal_handler );
  
  auto server_run = [] () {
    
    client_handler handler;
     auto callback = std::bind(&client_handler::service, &handler, std::placeholders::_1); 
    TCP_server server(65009, callback);
    server.run();
  };

  auto task = [](client_settings &client) {
      // ethernet patket data size
      std::string message;
      message.reserv(1522);
      for (int i = 0 ; i < 100 ; ++i) {
        for(auto &seq : client.seq ) {
          message += std::to_string(seq.get()) + " ";
          ++seq;
        }
        message += '\n';
      }
    
  }

  std::thread (server_run).detach();

  while(!client_handler::terminate) {
    sleep(2);
    
    for (auto &it: *cli_list){
      std::cout << "client_id "<< it.client_id << " set reg" << std::endl;
      for (int i = 0 ; i < 3 ; ++i) {
        std::cout << "seq " + std::to_string( i ) << " " << it.seq.at(i).get_param_str() << std::endl;
      }
    }
  }

}