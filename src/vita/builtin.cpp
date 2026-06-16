#include "builtin.h"
#include "value.h"
#include <string>
#include <vector>

using InitFn = void (*)(std::unordered_map<std::string, Value> *);

Value LoadModuleFn::call(std::vector<Value> args) {
    auto mod = std::shared_ptr<NinModule>();

    return mod; // we're stubbing it cause the vita really doesnt like dynmod...
}
