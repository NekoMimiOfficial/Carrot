#include "carrot_module.h"
#include <string>

struct TestModFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "init"; }
  Value call(std::vector<Value>) override {
    return "hello, module!";
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["test"] = std::make_shared<TestModFn>();
}
