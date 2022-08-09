#include "server/client_handler.hpp"
#include "server/tcp_server.hpp"
#include "thread_pool/thread_pool.hpp"

#include <signal.h>
#include <functional>
#include <sstream>
#include <string>

/**
 * @brief Handle signal from OS
 * 
 * @param signum 
 * @details close all sockets and file descriptors
 */
void signal_handler(int)
{
  TCP_server::terminate = true;
  client_handler::terminate = true;
  client_list::get_instance()->close_all_socket();

  for (auto x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
  {
    close(x);
  }
  exit(1);
}

/**
 * @brief Send piece of data to client socket
 * 
 * @param client client setting contains port, socket and sequence of sequences, process flag
 */
void print_chunk_data(client_settings &client)
{
  std::string message;
  message.reserve(1522);
  for (int i = 0; i < 100; ++i)
  {
    for (auto &seq : client.seq)
    {
      message += std::to_string(seq.get_counter()) + " ";
      ++seq;
    }
    message += '\n';
  }

  // TODO: check socket connection, if it closes then delete client or set flag or them

  send(*client.socket, message.c_str(), message.size(), 0);
#ifdef DEBUG
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(500ms);
#endif
  client.in_process = false;
  return;
}

int main(int argc, char *argv[])
{
  // Handle signals from OS
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGTERM, signal_handler);

  if (argc < 2)
  {
    std::cout << "plesse set port " << std::endl;
    return 0;
  }

  long port = std::atoi(argv[1]);


  // Lambda for server starting with handler (client_handler::service)
  auto server_run = [](uint16_t port)
  {
    client_handler handler;
    auto callback =
        std::bind(&client_handler::service, &handler, std::placeholders::_1);
    TCP_server server(port, callback);
    server.run();
  };

  std::thread(server_run, port).detach();
  thread_pool thr_pool(2);

  // Get pointer to client list singleton, which contains info about clients
  client_list *cli_list = client_list::get_instance();
  while (not TCP_server::terminate)
  {
    for (auto &client : *cli_list)
    {
      if (not client.in_process)
      {
        client.in_process = true;
        // Add task to print piece of sequences to the thread pool for the client 
        thr_pool.add_task(print_chunk_data, std::ref(client));
      }
    }
    std::this_thread::yield();
  }
}
