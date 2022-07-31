#ifndef _TCP_SERVER_HPP_
#define _TCP_SERVER_HPP_

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

class TCPserver {
  TCPserver() = delete;
  TCPserver(TCPserver& oth) = delete;
  TCPserver(const TCPserver& oth) = delete;
  TCPserver(TCPserver&& oth) = delete;
  TCPserver operator=(const TCPserver oth) = delete;
  TCPserver operator=(TCPserver&& oth) = delete;

  TCPserver(const uint16_t _port) : port(_port) {
    
  };

  int run() {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
      perror("socket");
      exit(1);
    }
    // reset to zero
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    // we bind to all interfaces
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, &servaddr, sizeof(servaddr)) < 0) {
        printf ("error bind ");
        exit (1);
    }

    listen(listener, 0);



  }

 private:
  int sock, listener;
  struct sockaddr_in servaddr;
  uint16_t port;
};

#endif  //_TCP_SERVER_HPP_