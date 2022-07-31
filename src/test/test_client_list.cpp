#include <iostream>
#include <thread>
#include "../server/client_list.hpp"

void simpletest() {
  client_list* cl1 = client_list::GetInstanse();
  std::cout << cl1->get().client_id << std::endl;
  client_settings b;
  b.client_id = 888;
  cl1->set(std::move(b));
}

int main(int argc, char* argv[]) {
  client_list* cl = client_list::GetInstanse();

  client_settings a;
  a.client_id = 999;

  cl->set(std::move(a));

  std::thread test_tread(simpletest);
  for (int i = 0; i < 10; i++) {
    using namespace std::chrono_literals;
    //std::this_thread::sleep_for(1ns);
    std::cout << "main tread " << cl->get().client_id << std::endl;
  }
  test_tread.join();
}