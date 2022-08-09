#ifndef _CLIENT_HANDLER_HPP_
#define _CLIENT_HANDLER_HPP_

#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <memory>
#include <regex>

#include <iostream>

#include "client_list.hpp"

/**
 * @brief communicates with users
 * @details it receives commands from client, parses them 
 * and adds new client with setttings to client list
 */
class client_handler
{
public:

  void service(std::unique_ptr<int> &&socket_client)
  {
    ssize_t msg_size{0};
    client_settings new_client;
    new_client.seq.resize(3);
    new_client.socket = (std::move(socket_client));

    while (not terminate)
    {
      bzero(msg_buf, len);
      msg_size = recv(*new_client.socket, (void *) msg_buf, len, 0);
      if (msg_size < 0)
      {
        // TODO handle error client;
        printf("error client ");
        return;
      }

      std::string msg_str(msg_buf, msg_size);

      std::sregex_iterator iter(msg_str.begin(), msg_str.end(), regexp_seq);
      std::sregex_iterator end;
      if (iter->size() > 0)
      {
        try
        {
          std::uint64_t seq_iter = std::stoull((*iter)[1]);
          std::uint64_t start = std::stoull((*iter)[2]);
          std::uint64_t step = std::stoull((*iter)[3]);

          new_client.seq.at(seq_iter - 1).set(start, step);

        #ifdef DEBUG_PRINT
          std::cout << "seq_iter " << seq_iter << " ; start " << start << "; step " << step << std::endl;
        #endif // DEBUG_PRINT

        }
        catch (const std::out_of_range &e)
        {
          std::string error {"Is too long for unsigned long long (uint64_t)\n"};
          send(*new_client.socket, error.c_str(), error.size() + 1, 0);
          continue;
        }

        continue;
      }

      if (msg_str.find(command_start_job) != std::string::npos)
      {

#ifdef DEBUG_PRINT
        std::cout << "client seq is " << new_client.seq.size() << std::endl;
        for (auto &it : new_client.seq)
        {
          std::cout << it.get_counter() << " ";
        }
        std::cout << std::endl;
#endif

        // remove invalid sequence
        auto it = std::remove_if(new_client.seq.begin(), new_client.seq.end(), [](sequence<uint64_t> &seq_)
                                 { return not seq_.is_valid(); });
        new_client.seq.erase(it, new_client.seq.end());

        if(not new_client.seq.empty()){
          client_list::get_instance()->emplace_back(std::move(new_client));
          break;
        }
        new_client.seq.resize(3);
      }

      send(*new_client.socket, unrecognized_command_msg.c_str(), unrecognized_command_msg.size() + 1, NULL);
    }
  }

private:
  static constexpr size_t len{256};
  char msg_buf[len];

  std::string command_start_job = "export seq";
  std::string unrecognized_command_msg = "the wrong command could not be recognized, please repeat the input \n";
  std::regex regexp_seq{"^seq([1-3]) ([0-9]+) ([0-9]+)"};

public:
  static bool terminate;
};

bool client_handler::terminate = false;

#endif //_CLIENT_HANDLER_HPP_
