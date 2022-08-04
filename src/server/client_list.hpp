#ifndef _CLIENT_LIST_HPP_
#define _CLIENT_LIST_HPP_

#include <unistd.h>


#include <string>
#include <cstdint>
#include <vector>
#include <list>
#include <memory>
#include <mutex>

template <typename T>
class sequence {
 public:
  sequence() = default;
  sequence(sequence&& oth) = default;

  sequence(sequence& oth) = delete;
  sequence(const sequence& oth) = delete;
  sequence& operator=(const sequence& oth) = delete;
  sequence& operator=(sequence&& oth) = delete;

  /**
   * @brief Construct a new sequence object
   *
   * @param _start start value
   * @param _step step value
   * @details counter - carrent value = start at the constructor stage
   */
  sequence(T _start, T _step)
      : start(_start),
        step(_step),
        counter(_start){

        };

  void set(T _start, T _step) {
    start = _start;
    counter = start;
    step = _step;

  }

  bool is_init () {
    return (start > 0) and (step > 0); 
  }
  // prefix operator
  sequence& operator++() {
    auto tmp_counter = counter;
    if (std::numeric_limits<T>::max() - step < tmp_counter) {
      // Edge case: there is counter overflow, have to return to begin value
      counter = start;
    } else {
      counter += step;
    }
    return *this;
  }

  T get() { return counter; }

  std::string get_param_str() const {
    return (std::to_string(start) + " " +std::to_string(step) + " " + std::to_string(counter));
  }

 private:
  T start;
  T step;
  T counter;
};

class client_settings {
 public:
  uint64_t client_id;
  std::unique_ptr<int> socket = nullptr;
  std::vector<sequence<uint64_t>> seq;
  
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

  void emplace_back(client_settings&& client) { 
    std::lock_guard<std::mutex>  guard (internal_mutex);
    client_l.emplace_back(std::move(client)); 
    client_l.back().client_id = last_client_id;
    ++last_client_id;
  }

  client_settings& get() { 
    std::lock_guard<std::mutex>  guard (internal_mutex);
    return client_l.back(); 
  }

  std::size_t size() {
    return client_l.size();
  }

  auto begin () {
    return client_l.cbegin();
  }

  auto end() {
    return client_l.cend();
  }

  void close_all_socoekt() {
      for(auto &it : client_l) {
      close(*it.socket);
    }
    return;
  }

 private:
  client_list() = default;
  static client_list* instance_client_list;
  static std::mutex mutex_client_list;
  std::mutex internal_mutex;

  std::list<client_settings> client_l;
  uint64_t last_client_id {0};
};
client_list* client_list::instance_client_list{nullptr};
std::mutex client_list::mutex_client_list;

#endif  // _CLIENT_LIST_HPP_