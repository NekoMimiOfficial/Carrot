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
          coro->setReturn(result);
        } catch (...) {
          coro->setReturn(std::monostate{});
        }
        coro->state = NinCoroutine::State::DONE;
      }));

  coro->platformHandle = future;
}

void coroutineJoin(std::shared_ptr<NinCoroutine> coro) {
  if (coro->platformHandle) {
    auto future =
        std::static_pointer_cast<std::future<void>>(coro->platformHandle);
    if (future->valid())
      future->get();
    coro->platformHandle = nullptr;
  }
}
