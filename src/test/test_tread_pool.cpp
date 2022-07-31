#include <cmath>
#include <functional>
#include "../thread_pool/thread_pool.hpp"


class Test {
 public:
  void operator()() { std::cout << "Working with functors!\n"; }
};

void computing(int a, int b) {
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(500ms);
  std::thread::id this_id = std::this_thread::get_id();
  std::size_t result =
      static_cast<uint64_t>(std::exp2(static_cast<uint64_t>(a))) +
      static_cast<uint64_t>(pow(a, b));
  std::cout << "result: " << result << std::endl;
  std::cout << "this tread id : " << this_id << std::endl;
}

int main() {
  // run_test();

  Test test;

  thread_pool t(3);

  for (int i = 0, j = 0; i < 15 && j < 15; ++i, ++j) {
    auto res = std::bind(computing, i, j);
    t.add_task(res);
  }

  t.add_task(test);

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1500ms);
  std::cout << "wait all !" << std::endl;
  t.wait_all();

  return 0;
}