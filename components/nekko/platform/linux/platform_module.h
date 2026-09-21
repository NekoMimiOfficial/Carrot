#pragma once
#include "asset_store.h"
#include "fs.h"
#include "value.h"
#include "interpreter.h"
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <variant>
#include <vector>

using InitFn = void (*)(std::unordered_map<std::string, Value> *);

struct ModuleKeepAliveFn : NinCallable {
  std::shared_ptr<NinCallable> inner;
  std::shared_ptr<void> keepAlive;

  ModuleKeepAliveFn(std::shared_ptr<NinCallable> inner,
                    std::shared_ptr<void> keepAlive)
      : inner(std::move(inner)), keepAlive(std::move(keepAlive)) {}

  int arity() override { return inner->arity(); }
  std::string name() override { return inner->name(); }
  Value call(std::vector<Value> args) override {
    return inner->call(std::move(args));
  }
};

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
      auto pget = getAsset("CAPI/" + ("libcarrot_" + rel.substr(1) + ".so"));
      if (std::holds_alternative<std::string>(pget)) {
        path = std::get<std::string>(pget);
      } else {
        throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                                 "'.");
      }
    } else {
      path = std::filesystem::path(callerDir) / ("libcarrot_" + rel + ".so");
    }

    std::string cacheKey =
        std::filesystem::absolute(path).lexically_normal().string();
    Value cached = interp->getCachedModule(cacheKey);
    if (!std::holds_alternative<std::monostate>(cached))
      return cached;

    std::ifstream file(path);
    if (!file.is_open())
      throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                               "'.");

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    void *rawHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!rawHandle)
      throw std::runtime_error("loadmodule(): cannot open '" + path.string() +
                               "': " + dlerror());

    auto init = (InitFn)dlsym(rawHandle, "carrot_module_init");
    if (!init) {
      dlclose(rawHandle);
      throw std::runtime_error("loadmodule(): '" + path.string() +
                               "' has no carrot_module_init symbol.");
    }

    auto libHandle = std::shared_ptr<void>(rawHandle, [](void *h) {
      if (h)
        dlclose(h);
    });

    auto mod = std::make_shared<NinModule>();
    mod->sourcePath = path.string();
    mod->handle = rawHandle;

    std::unordered_map<std::string, Value> rawMembers;
    init(&rawMembers);

    for (auto &[memberName, val] : rawMembers) {
      if (std::holds_alternative<std::shared_ptr<NinCallable>>(val)) {
        auto orig = std::get<std::shared_ptr<NinCallable>>(val);
        mod->members[memberName] =
            std::make_shared<ModuleKeepAliveFn>(orig, libHandle);
      } else {
        mod->members[memberName] = val;
      }
    }

    interp->cacheModule(cacheKey, mod);
    return mod;
  }
};
