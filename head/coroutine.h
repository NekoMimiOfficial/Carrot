#pragma once
#include "value.h"

void coroutineRun(std::shared_ptr<NinCoroutine> coro);
void coroutineJoin(std::shared_ptr<NinCoroutine> coro);
extern thread_local bool insideCoroutine;
