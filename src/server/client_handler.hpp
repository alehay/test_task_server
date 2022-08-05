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
    client_settings new_client;
    new_client.seq.resize(3);
    new_client.socket = (std::move(socket_cleint));

    while (!terminate) {
      bzero(msg_buf, 255);
      msg_size = recv(*new_client.socket, (void*)msg_buf, len, NULL);
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

        #ifdef DEBUG_PRINT
        std::cout << "seq_iter " << seq_iter << " ; start " << start << "; step " << step << std::endl;
        #endif // DEBUG_PRINT 
        continue;
      }

      if (msg_str.find(comand_start_job) != std::string::npos) {
        
        #ifdef DEBUG_PRINT
          std::cout << "client seq is " << new_client.seq.size() << std::endl;
          for (auto &it : new_client.seq) {
            std::cout << it.get_counter() << " ";
          }
          std::cout << std::endl;
        #endif     
       
        //  auto it = std::remove_if( new_client.seq.begin(), new_client.seq.end(),[](sequence <typename std::decay<decltype(*new_client.seq.begin())>::type> & _seq) {
   
        // remove invalid sequence
        auto it = std::remove_if( new_client.seq.begin(), new_client.seq.end(),[](sequence <uint64_t> & _seq) {
          return not _seq.is_valid();
        });
        new_client.seq.erase(it,  new_client.seq.end());

        client_list::GetInstanse()->emplace_back(std::move(new_client));
        break;
      }
      
      send(*new_client.socket, unrecognized_comadn_msg.c_str(), unrecognized_comadn_msg.size() +1, NULL ); 
    }
  }


 private:
  char msg_buf[255];
  size_t len = 255;
  std::string comand_start_job = "export seq";
  std::string unrecognized_comadn_msg = "the command could not be recognized, repeat the input \n";
  std::regex regexp_seq{"^seq([1-3]) ([0-9]+) ([0-9]+)"};
public:
  static bool terminate ;  
};

bool client_handler::terminate = false;
#endif  //_CLENT_HANDLER__s