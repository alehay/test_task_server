#ifndef _CLIENT_HANDLER_
#define _CLENT_HANDLER_

#include <stdio.h>

#include <memory>
#include <regex>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

class client_handler {
 public:
  void service(std::unique_ptr<int>&& socket_cleint) {
    //std::regex regexp_seq("^seq([1-3]) ([0-9]+) ([0-9]+)");

    int msg_size{0};
    bool term = false;
    while ( !term ) {
      bzero(msg_buf, 255);
      msg_size = recv(*socket_cleint, (void *)msg_buf, len, NULL);     
      if (msg_size < 0 ) {
        
        // TODO handle error client;
          printf("error client ");
          return ; 
      }
      
      std::string msg_str (msg_buf,msg_size );
      if (msg_str.find(comand_start_job) != std::string::npos) {
        send(*socket_cleint, comand_start_job.c_str(), comand_start_job.size() , NULL);
        close(*socket_cleint);
        break;  
      }  

      std::sregex_iterator iter(msg_str.begin(), msg_str.end(), regexp_seq);
      std::sregex_iterator end;

      std::cout << " first [" << (*iter)[1]
                << "] second [" << (*iter)[2] << "]" 
                << " third [" << (*iter)[3] << "]" << std::endl;
 
      //printf("msg %s", msg_buf);
      //send(*socket_cleint, msg_buf_str.c_str(), msg_buf_str.size() , NULL);

    
    }

  }

 private:


  private:
  char msg_buf [255];
  size_t len = 255;
  std::string comand_start_job ="export seq";
  std::regex regexp_seq {"^seq([1-3]) ([0-9]+) ([0-9]+)"};

};
#endif  //_CLENT_HANDLER__