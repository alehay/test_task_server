#include <signal.h>
#include <functional>
#include <sstream>
#include <string>

#include "../server/client_handler.hpp"
#include "../server/tcp_server.hpp"
#include "../thread_pool/thread_pool.hpp"

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
  
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGTERM, signal_handler);

  auto server_run = []() {
    client_handler handler;
    auto callback =
        std::bind(&client_handler::service, &handler, std::placeholders::_1);
    TCP_server server(65009, callback);
    server.run();
  };

  auto task = []( client_settings& client)  {
    //if (client.in_processing) {return;}
    // ethernet patket data size
    std::string message;
    message.reserve(1522);
    for (int i = 0; i < 100; ++i) {
      for (auto& seq : client.seq) {
        message += std::to_string(seq.get_counter()) + " ";
        ++seq;
      }
      message += '\n';
    }
    send(*client.socket, message.c_str(), message.size(), NULL );
    using namespace std::chrono_literals;
   // client.in_processing = false;
    std::this_thread::sleep_for(500ms);
    return;
  };

  std::thread(server_run).detach();
  thread_pool thr_pool(3);
  //thr_pool.run();

  client_list* cli_list = client_list::GetInstanse();
  while (TCP_server::terminate)   {
    for (auto & client:  *cli_list) {
     // if( !client.in_processing)
      thr_pool.add_task(task, client);
    }
  }


}