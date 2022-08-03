#include <signal.h>
#include <sstream>
#include <string>
#include <functional>

#include "../server/tcp_server.hpp"
#include "../server/client_handler.hpp"

void signal_handler(int signum) {
  TCP_server::terminate = true;
  /* Close all open file descriptors */
  int x;
  for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
    close(x);
  }
}

int main(int argc, char* argv[]) {
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);

  client_handler handler;
  auto callback = std::bind(&client_handler::service, &handler, std::placeholders::_1); 

  TCP_server server(65009, callback);
  server.run();
}