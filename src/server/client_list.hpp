#ifndef _CLIENT_LIST_HPP_
#define _CLIENT_LIST_HPP_

#include <cstdint>
#include <list>
#include <mutex>
#include <memory>

class client_settings {
public:
  uint64_t client_id;
  int *socket;
  uint64_t sequence_start;
  uint64_t sequence_step;
};

class client_list {
 public:
  

  client_list(client_list& oth) = delete;
  client_list(const client_list& oth) = delete;
  client_list(client_list&& oth) = delete;
  client_list& operator=(const client_list& oth) = delete;
  client_list& operator=(client_list&& oth) = delete;

  static client_list* GetInstanse() {
    std::lock_guard<std::mutex> lcok(mutex_client_list);
    if (instance_client_list == nullptr) {
      instance_client_list = new client_list;
    }
    return instance_client_list;
  }

  void set(client_settings&& client) { clietn_s.emplace_back(client); }
  client_settings& get() { return clietn_s.back(); }

 private:
  client_list() = default;
  static client_list* instance_client_list;
  static std::mutex mutex_client_list;

  std::list<client_settings> clietn_s;
  uint64_t last_client_id;
};
client_list* client_list::instance_client_list {nullptr};
std::mutex client_list::mutex_client_list;


#endif  // _CLIENT_LIST_HPP_