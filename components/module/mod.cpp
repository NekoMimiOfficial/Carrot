#include "carrot_module.h"
#include <string>

struct PlaceholderFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "pholder"; }
  Value call(std::vector<Value>) override {
    return "hello, module!";
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["pholder"] = std::make_shared<PlaceholderFn>();
}
