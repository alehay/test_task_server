#ifndef _CLIENT_HANDLER_
#define _CLENT_HANDLER_

#include <memory>
#include <regex>

class client_handler {
 public:
  void service(std::unique_ptr<int>&& socket_cleint) {}

 private:
  struct RegExpGen {
    static std::regex get(std::int8_t number) {
      std::string expression =
          "^seq" + std::to_string(number) + " ([0-9]+) ([0-9]+)$";
      return std::regex(expression);
    }
  };

#endif  //_CLENT_HANDLER__