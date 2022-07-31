#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>

class thread_pool {
 public:
  // prohibition on copying and moving
  thread_pool() = delete;
  thread_pool(thread_pool& oth) = delete;
  thread_pool(const thread_pool& oth) = delete;
  thread_pool(thread_pool&& oth) = delete;
  thread_pool operator=(const thread_pool& oth) = delete;
  thread_pool operator=(thread_pool&& oth) = delete;

  thread_pool(uint32_t num_threads) {
    threads.reserve(num_threads);
    for (uint32_t i = 0; i < num_threads; ++i) {
      threads.emplace_back(&thread_pool::run, this);
    }
  }

  template <typename Func, typename... Args>
  int64_t add_task(const Func& task_func, Args&&... args) {
    int64_t task_idx = last_idx++;

    std::lock_guard<std::mutex> q_lock(queue_mtx);
    queue_task.emplace(std::async(std::launch::deferred, task_func, args...),
                       task_idx);
    queue_cv.notify_one();
    return task_idx;
  }

  void wait(int64_t task_id) {
    std::unique_lock<std::mutex> lock(completed_task_ids_mtx);
    completed_task_ids_cv.wait(lock, [this, task_id]() -> bool {
      return completed_task_ids.find(task_id) != completed_task_ids.end();
    });
  }

  void wait_all() {
    std::unique_lock<std::mutex> lock(queue_mtx);
    completed_task_ids_cv.wait(lock, [this]() -> bool {
      std::lock_guard<std::mutex> task_lock(completed_task_ids_mtx);
      return queue_task.empty() && last_idx == completed_task_ids.size();
    });
  }

  bool calculated(int64_t task_id) {
    std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
    if (completed_task_ids.find(task_id) != completed_task_ids.end()) {
      return true;
    }
    return false;
  }

  ~thread_pool() {
    quite = true;
    for (uint32_t i = 0; i < threads.size(); ++i) {
      queue_cv.notify_all();
      threads[i].join();
    }
  }

 private:
  void run() {
    while (!quite) {
      std::unique_lock<std::mutex> lock(queue_mtx);
      // we are waiting for the receipt of the task
      queue_cv.wait(lock,
                    [this]() -> bool { return !queue_task.empty() || quite; });

      if (!queue_task.empty()) {
        auto elem = std::move(queue_task.front());
        queue_task.pop();
        lock.unlock();

        elem.first.get();

        std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
        completed_task_ids.insert(elem.second);

        completed_task_ids_cv.notify_all();
      }
    }
  }

  // task queue - stores the function (task) to be executed and the number of
  // this task
  std::queue<std::pair<std::future<void>, int64_t>> queue_task;
  // queue mutex - task queue guard
  std::mutex queue_mtx;

  // waiting for an event in the presence of mutex queue_mtx
  std::condition_variable queue_cv;

  // we put the completed tasks
  std::unordered_set<int64_t> completed_task_ids;

  std::condition_variable completed_task_ids_cv;
  std::mutex completed_task_ids_mtx;

  std::vector<std::thread> threads;

  // global stop flag thread_pool
  std::atomic<bool> quite{false};

  // id counter for tasks
  std::atomic<uint64_t> last_idx = 0;
};

#endif  // _THREAD_POOL_HPP_