#include "carrot_module.h"
#include <psp2/kernel/modulemgr.h>
#include <string>

struct TestModFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "init"; }
  Value call(std::vector<Value>) override { return "hello, carrot!"; }
};

// PS: This will likely never work since i never saw PRX work on C++ qwq
// I will sadly be stubbing loadmodule() from the Vita build, instead it'll heavily rely on builtins...
// speaking of which... oh frick...
// it's leaking out... aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

extern "C" {
void carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["test"] = std::make_shared<TestModFn>();
}

void _start() __attribute__((weak, alias("module_start")));
int module_start(SceSize argc, void *args) { return SCE_KERNEL_START_SUCCESS; }
int module_stop(SceSize argc, const void *args) {
  return SCE_KERNEL_STOP_SUCCESS;
}
int module_exit() { return SCE_KERNEL_STOP_SUCCESS; }
}
