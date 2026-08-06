#pragma once
#include "asset_store.h"
#include "fs.h"
#include "value.h"
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <variant>
#include <vector>

using InitFn = void (*)(std::unordered_map<std::string, Value> *);

struct LoadModuleFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "loadmodule"; }

  Interpreter *interp;
  std::string callerDir;
  explicit LoadModuleFn(Interpreter *interp, std::string callerDir)
      : interp(interp), callerDir(std::move(callerDir)) {}

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("loadmodule(): argument must be a string path.");

    std::filesystem::path path;
    std::string rel = std::get<std::string>(args[0]);
    if (!rel.empty() and rel.front() == '@') {
      auto pget = getAsset("CAPI/" + rel.substr(1));
      if (std::holds_alternative<std::string>(pget)) {
        path = std::get<std::string>(pget);
      } else {
        throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                                 "'.");
      }
    } else {
      path = std::filesystem::path(callerDir) / rel;
    }

    std::ifstream file(path);
    if (!file.is_open())
      throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                               "'.");

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    void *handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE); // TODO: please fix this garbo qwq
    if (!handle)
      throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                               "': " + dlerror());

    auto init = (InitFn)dlsym(handle, "carrot_module_init");
    if (!init) {
      dlclose(handle);
      throw std::runtime_error("loadmodule(): '" + path.string() +
                               "' has no carrot_module_init symbol.");
    }

    auto mod = std::shared_ptr<NinModule>(new NinModule(), [](NinModule *m) {
      if (m->handle)
        dlclose(m->handle);
      delete m;
    });

    mod->sourcePath = path.string();
    mod->handle = handle;
    init(&mod->members);

    return mod;
  }
};
