#ifndef _CLIENT_LIST_HPP_
#define _CLIENT_LIST_HPP_

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

  void set(client_settings&& client) { clietn_s.emplace_back(std::move(client)); }
  client_settings& get() { return clietn_s.back(); }

 private:
  client_list() = default;
  static client_list* instance_client_list;
  static std::mutex mutex_client_list;

  std::list<client_settings> clietn_s;
  uint64_t last_client_id;
};
client_list* client_list::instance_client_list{nullptr};
std::mutex client_list::mutex_client_list;

#endif  // _CLIENT_LIST_HPP_