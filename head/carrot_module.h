#pragma once
#include <atomic>
#include <cmath>
#include <functional>
#include <iomanip>
#include <memory>
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
    std::variant<std::monostate, double, std::string, bool,
                 std::shared_ptr<NinCallable>, std::shared_ptr<NinArray>,
                 std::shared_ptr<NinClass>, std::shared_ptr<NinInstance>,
                 std::shared_ptr<NinModule>, std::shared_ptr<NinCoroutine>,
                 std::shared_ptr<NinNative>>;

struct NinArray {
  std::vector<Value> elements;

  NinArray() = default;
  explicit NinArray(std::vector<Value> elems) : elements(std::move(elems)) {}
};

struct NinCallable {
  virtual ~NinCallable() = default;
  virtual int arity() = 0;
  virtual Value call(std::vector<Value> args) = 0;
  virtual std::string name() = 0;
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

  explicit NinClass(std::string name) : className(std::move(name)) {}
};

struct NinInstance {
  std::shared_ptr<NinClass> klass;
  std::unordered_map<std::string, Value> fields;

  explicit NinInstance(std::shared_ptr<NinClass> k) : klass(std::move(k)) {}
};

struct NinCoroutine {
  enum class State { CREATED, RUNNING, DONE };

  std::atomic<State> state{State::CREATED};
  Value returnValue;
  std::function<Value()> task;
  std::shared_ptr<void> platformHandle;

  explicit NinCoroutine(std::function<Value()> t) : task(std::move(t)) {}
};

struct NinNative {
  std::string typeName;
  std::shared_ptr<void> data;
  std::function<Value(const std::string &field)> getField;
  std::function<void(const std::string &field, Value v)> setField;
};

// Helpers go below, stop looking where the divider is girl...

inline std::string valueToString(const Value &val) {
  if (std::holds_alternative<std::monostate>(val))
    return "nil";

  if (std::holds_alternative<double>(val)) {
    double d = std::get<double>(val);
    if (std::isnan(d))
      return "nan";
    if (std::isinf(d))
      return d > 0 ? "inf" : "-inf";

    if (d == std::floor(d) && std::abs(d) < 1e15) {
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(0) << d;
      return oss.str();
    }

    std::ostringstream oss;
    oss << d;
    return oss.str();
  }

  if (std::holds_alternative<std::string>(val))
    return std::get<std::string>(val);

  if (std::holds_alternative<bool>(val))
    return std::get<bool>(val) ? "true" : "false";

  if (std::holds_alternative<std::shared_ptr<NinCallable>>(val)) {
    auto fn = std::get<std::shared_ptr<NinCallable>>(val);
    return "<fun " + fn->name() + ">";
  }

  if (std::holds_alternative<std::shared_ptr<NinArray>>(val)) {
    auto arr = std::get<std::shared_ptr<NinArray>>(val);
    std::string s = "[";
    for (size_t i = 0; i < arr->elements.size(); i++) {
      if (i > 0)
        s += ", ";

      if (std::holds_alternative<std::string>(arr->elements[i]))
        s += "\"" + std::get<std::string>(arr->elements[i]) + "\"";
      else
        s += valueToString(arr->elements[i]);
    }
    s += "]";
    return s;
  }

  if (std::holds_alternative<std::shared_ptr<NinModule>>(val)) {
    auto mod = std::get<std::shared_ptr<NinModule>>(val);
    return "<module \"" + mod->sourcePath + "\">";
  }

  if (std::holds_alternative<std::shared_ptr<NinClass>>(val)) {
    return "<class " + std::get<std::shared_ptr<NinClass>>(val)->className +
           ">";
  }

  if (std::holds_alternative<std::shared_ptr<NinInstance>>(val)) {
    return "<instance of " +
           std::get<std::shared_ptr<NinInstance>>(val)->klass->className + ">";
  }

  if (std::holds_alternative<std::shared_ptr<NinCoroutine>>(val))
    return "<coroutine>";

  if (std::holds_alternative<std::shared_ptr<NinNative>>(val))
    return "<native " + std::get<std::shared_ptr<NinNative>>(val)->typeName +
           ">";

  return "<unknown>";
}

inline bool isTruthy(const Value &val) {
  if (std::holds_alternative<std::monostate>(val))
    return false;
  if (std::holds_alternative<bool>(val))
    return std::get<bool>(val);
  return true;
}

extern "C" {
void carrot_module_init(std::unordered_map<std::string, Value> *out);
}
