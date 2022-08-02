#include <signal.h>
#include <sstream>
#include <string>
#include "../server/tcp_server.hpp"

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

  std::function<void(std::unique_ptr<int>&&)> worker =
      [](std::unique_ptr<int> && socket) {
        printf("tread start \n");
        std::ostringstream ss;
        ss << "tread info : " << std::this_thread::get_id()
           << " client id : " << "\n";

        while (TCP_server::terminate != true) {
          using namespace std::chrono_literals;
          std::this_thread::sleep_for(1s);
          write(*socket, ss.str().data(), ss.str().size() + 1);
        }
      };

  TCP_server server(65004, worker);
  server.run();
}