#include "coroutine.h"
#include <future>
#include <thread>

thread_local bool insideCoroutine = false;

void coroutineRun(std::shared_ptr<NinCoroutine> coro) {
  if (coro->state != NinCoroutine::State::CREATED)
    throw std::runtime_error("coroutine is already running or finished.");

  coro->state = NinCoroutine::State::RUNNING;

  auto future = std::make_shared<std::future<void>>(
      std::async(std::launch::async, [coro]() {
        insideCoroutine = true;
        try {
          Value result = coro->task();
          coro->returnValue = result;
        } catch (...) {
          coro->returnValue = std::monostate{};
        }
        coro->state = NinCoroutine::State::DONE;
      }));

  coro->platformHandle = future;
}
