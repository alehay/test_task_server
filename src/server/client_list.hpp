#ifndef _CLIENT_LIST_HPP_
#define _CLIENT_LIST_HPP_

#include <unistd.h>

#include <string>
#include <cstdint>
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <atomic>

/**
 * @brief stores sequence setting: start, step and counter
 * @details for increment of the sequence you may call operator++ 
 * @tparam T is integer type only
 */
template <typename T>
class sequence
{
public:
  sequence() = default;
  sequence(sequence &&oth) = default;
  sequence &operator=(sequence &&oth) = default;

  sequence(sequence &oth) = delete;
  sequence(const sequence &oth) = delete;
  sequence &operator=(const sequence &oth) = delete;

  /**
   * @brief Construct a new sequence object
   *
   * @param _start start value
   * @param _step step value
   * @details counter - current value = start at the constructor stage
   */
  sequence(T _start, T _step)
      : start(_start),
        step(_step),
        counter(_start){

        };

  void set(T _start, T _step)
  {
    start = _start;
    step = _step;
    counter = _start;
  }

  bool is_valid()
  {
    return (start > 0ul) && (step > 0ul);
  }

  sequence &operator++()
  {
    auto tmp_counter = counter;
    if (std::numeric_limits<T>::max() - step < tmp_counter)
    {
      // Edge case: there is counter overflow, have to return to begin value
      counter = start;
    }
    else
    {
      counter += step;
    }
    return *this;
  }

  T get_counter() { return counter; }

  std::string get_param_str() const
  {
    return (std::string(" strt > ") + std::to_string(start) + "; step" + std::to_string(step) + "; counter " + std::to_string(counter));
  }

private:
  T start;
  T step;
  T counter;
};

/**
 * @brief stores user info: id, socket descriptor, vector of sequences and process flag
 * 
 */
struct client_settings
{
  std::uint64_t client_id;
  std::unique_ptr<int> socket{nullptr};
  std::vector<sequence<std::uint64_t>> seq;
  bool in_process;
};

/**
 * @brief thread-safe singleton which contains the list of clients
 * 
 */
class client_list
{
public:
  client_list(client_list &oth) = delete;
  client_list(const client_list &oth) = delete;
  client_list(client_list &&oth) = delete;
  client_list &operator=(const client_list &oth) = delete;
  client_list &operator=(client_list &&oth) = delete;

  static client_list *get_instance()
  {
    std::lock_guard<std::mutex> lock(get_client_list_mutex());
    if (not instance_client_list)
    {
      instance_client_list = new client_list;
    }
    return instance_client_list;
  }

  void emplace_back(client_settings &&client)
  {
    std::lock_guard<std::mutex> guard(internal_mutex);

    client_l.emplace_back(std::move(client));
    client_l.back().client_id = last_client_id;
    ++last_client_id;
  }

  std::size_t size()
  {
    return client_l.size();
  }

  auto begin()
  {
    return client_l.begin();
  }

  auto end()
  {
    return client_l.end();
  }

  void close_all_socket()
  {
    for (auto &it : client_l)
    {
      close(*it.socket);
    }
    return;
  }

// TODO: write erase_of_client() method

private:
  client_list() = default;

  static std::mutex &get_client_list_mutex()
  {
    static std::mutex mutex;
    return mutex;
  }

private:
  static client_list *instance_client_list;
  std::mutex internal_mutex;

  std::list<client_settings> client_l;

  uint64_t last_client_id{0};
};

client_list *client_list::instance_client_list{nullptr};

#endif // _CLIENT_LIST_HPP_