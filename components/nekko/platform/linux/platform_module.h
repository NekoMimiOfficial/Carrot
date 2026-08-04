#pragma once
#include "fs.h"
#include "value.h"
#include <cstdlib>
#include <dlfcn.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

using InitFn = void (*)(std::unordered_map<std::string, Value> *);

struct LoadModuleFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "loadmodule"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("loadmodule(): argument must be a string path.");

    std::string get_path = std::get<std::string>(args[0]);
    std::string path;
    path = get_path;
    if (!get_path.empty() and get_path.front() == '@')
      path = expand_user_path("~/.local/lib/carrot/CAPI/").string() + get_path.substr(1);

    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
      throw std::runtime_error("loadmodule(): cannot open '" + path +
                               "': " + dlerror());

    auto init = (InitFn)dlsym(handle, "carrot_module_init");
    if (!init) {
      dlclose(handle);
      throw std::runtime_error("loadmodule(): '" + path +
                               "' has no carrot_module_init symbol.");
    }

    auto mod = std::shared_ptr<NinModule>(new NinModule(), [](NinModule *m) {
      if (m->handle)
        dlclose(m->handle);
      delete m;
    });

    mod->sourcePath = path;
    mod->handle = handle;
    init(&mod->members);

    return mod;
  }
};
