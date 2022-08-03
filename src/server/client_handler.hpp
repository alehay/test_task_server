#ifndef _CLIENT_HANDLER_
#define _CLENT_HANDLER_

#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <memory>
#include <regex>

#include <iostream>

#include "client_list.hpp"

class client_handler {
 public:
  void service(std::unique_ptr<int>&& socket_cleint) {
    int msg_size{0};
    bool term = false;
    client_settings new_client;
    new_client.seq.resize(3);

    while (!term) {
      bzero(msg_buf, 255);
      msg_size = recv(*socket_cleint, (void*)msg_buf, len, NULL);
      if (msg_size < 0) {
        // TODO handle error client;
        printf("error client ");
        return;
      }

      std::string msg_str(msg_buf, msg_size);

      std::sregex_iterator iter(msg_str.begin(), msg_str.end(), regexp_seq);
      std::sregex_iterator end;
      if (iter->size() > 0) {
        std::size_t seq_iter = std::stoul((*iter)[1]);
        std::size_t start = std::stoul((*iter)[2]);
        std::size_t step = std::stoul((*iter)[3]);
        
        new_client.seq.at(seq_iter - 1).set(start, step);

        std::cout << "seq_iter " << seq_iter << "start " << start << "step " << step << std::endl;
        continue;
      }
      /*
      std::cout << " first [" << (*iter)[1] << "] second [" << (*iter)[2] << "]"
                << " third [" << (*iter)[3] << "]" << std::endl;
      */

      if (msg_str.find(comand_start_job) != std::string::npos) {
        std::cout << "client seq is " << new_client.seq.size() << std::endl;
        for (auto &it : new_client.seq) {
          std::cout << it.get() << " ";
        }
        std::cout << std::endl;
        break;
      }
    }
  }

 private:
 private:
  char msg_buf[255];
  size_t len = 255;
  std::string comand_start_job = "export seq";
  std::regex regexp_seq{"^seq([1-3]) ([0-9]+) ([0-9]+)"};
};
#endif  //_CLENT_HANDLER__