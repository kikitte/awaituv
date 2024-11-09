// Test1.cpp : Defines the entry point for the console application.
//

#include "utils.h"
#include <awaituv.h>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <iostream>

using namespace awaituv;
using namespace std;

void write_thread_id()
{
  static std::mutex mutex;
  std::lock_guard   lock(mutex);
  std::cout << std::this_thread::get_id() << std::endl;
}

awaitable_t<void> test_loop_data()
{
  write_thread_id();
  co_await switch_to_work_queue_t{ uv_default_loop() };
  write_thread_id();
  co_await switch_to_loop_thread_t{ uv_default_loop() };
  write_thread_id();
}

awaitable_t<void> test_local_thread_switcher()
{
  local_thread_switcher_t switcher{ uv_default_loop() };
  write_thread_id();
  co_await switcher.switch_to_worker_thread();
  write_thread_id();
  co_await switcher.switch_to_loop_thread();
  write_thread_id();
}

int main(int argc, char* argv[])
{
  // Process command line
  if (argc != 1)
  {
    return -1;
  }

  // Create the shared loop_data_t object.
  {
    loop_data_t loop_data{ uv_default_loop() };
    std::cout << "test shared thread switcher mechanism" << std::endl;
    // The loop_data object will be shared by multiple coroutines/threads.
    test_loop_data();
    test_loop_data();
    test_loop_data();
    test_loop_data();
    test_loop_data();
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  }

  std::cout << "test local thread switcher mechanism" << std::endl;
  test_local_thread_switcher();
  test_local_thread_switcher();
  test_local_thread_switcher();
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  auto ret = uv_loop_close(uv_default_loop());
  assert(ret != UV_EBUSY);

  return 0;
}
