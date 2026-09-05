#pragma once
#include "lang.h"
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct NinCallable;
struct NinArray;
struct NinModule;
struct NinClass;
struct NinInstance;
struct NinCoroutine;
struct NinNative;

using Value =
    std::variant<std::monostate, double, std::string, bool, uint8_t,
                 std::shared_ptr<NinCallable>, std::shared_ptr<NinArray>,
                 std::shared_ptr<NinClass>, std::shared_ptr<NinInstance>,
                 std::shared_ptr<NinModule>, std::shared_ptr<NinCoroutine>,
                 std::shared_ptr<NinNative>>;

struct NinArray {
  std::vector<Value> elements;
  bool isConst = false;

  NinArray() = default;
  explicit NinArray(std::vector<Value> elems) : elements(std::move(elems)) {}
};

struct NinCallable {
  virtual ~NinCallable() = default;
  virtual int arity() = 0;
  virtual Value call(std::vector<Value> args) = 0;
  virtual std::string name() = 0;

  virtual bool isVariadic() { return false; }

  virtual Value callWithKwargs(std::vector<Value> args,
                               std::unordered_map<std::string, Value> kwargs) {
    if (!kwargs.empty())
      throw std::runtime_error(LOC(FN_NOT_VARIADIC, name()));
    return call(std::move(args));
  }
};

struct NinModule {
  std::string sourcePath;
  std::unordered_map<std::string, Value> members;
  void *handle = nullptr;

  NinModule() = default;

  NinModule(std::string path, std::unordered_map<std::string, Value> members)
      : sourcePath(std::move(path)), members(std::move(members)) {}
};

struct NinClass {
  std::string className;
  std::unordered_map<std::string, std::shared_ptr<NinCallable>> methods;
  std::shared_ptr<NinClass> superclass;

  explicit NinClass(std::string name,
                    std::shared_ptr<NinClass> superclass = nullptr)
      : className(std::move(name)), superclass(std::move(superclass)) {}
};

struct NinInstance {
  std::shared_ptr<NinClass> klass;
  std::unordered_map<std::string, Value> fields;

  explicit NinInstance(std::shared_ptr<NinClass> k) : klass(std::move(k)) {}
};

struct NinCoroutine {
  enum class State { CREATED, RUNNING, DONE, PAUSED };

  std::atomic<State> state{State::CREATED};
  Value returnValue;
  std::mutex valueMutex;
  std::function<Value()> task;
  std::shared_ptr<void> platformHandle;

  explicit NinCoroutine(std::function<Value()> t) : task(std::move(t)) {}

  void setReturn(Value v) {
    std::lock_guard<std::mutex> lock(valueMutex);
    returnValue = std::move(v);
  }

  Value getReturn() {
    std::lock_guard<std::mutex> lock(valueMutex);
    return returnValue;
  }
};

struct NinNative {
  std::string typeName;
  std::shared_ptr<void> data;
  std::function<Value(const std::string &field)> getField;
  std::function<void(const std::string &field, Value v)> setField;
};
