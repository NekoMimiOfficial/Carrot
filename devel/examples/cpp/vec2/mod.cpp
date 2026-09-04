#include "carrot_module.h"
#include <cmath>
#include <stdexcept>

struct Vector2 {
  double x = 0;
  double y = 0;
};

static Value wrapVector2(std::shared_ptr<Vector2> v) {
  auto native = std::make_shared<NinNative>();
  native->typeName = "Vector2";
  native->data = v;

  native->getField = [v](const std::string &f) -> Value {
    if (f == "x") return v->x;
    if (f == "y") return v->y;
    if (f == "length")
      return std::sqrt(v->x * v->x + v->y * v->y);
    throw std::runtime_error("Vector2 has no field '" + f + "'.");
  };

  native->setField = [v](const std::string &f, Value val) {
    if (!std::holds_alternative<double>(val))
      throw std::runtime_error("Vector2." + f + " must be a number.");
    double d = std::get<double>(val);
    if (f == "x") v->x = d;
    else if (f == "y") v->y = d;
    else throw std::runtime_error("Vector2 has no settable field '" + f + "'.");
  };

    return native;
}

struct Vector2NewFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "vec2"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]) ||
      !std::holds_alternative<double>(args[1]))
      throw std::runtime_error("vec2(x, y): both arguments must be numbers.");

    auto v = std::make_shared<Vector2>();
    v->x = std::get<double>(args[0]);
    v->y = std::get<double>(args[1]);
    return wrapVector2(v);
  }
};

struct Vector2AddFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "vec2add"; }
  Value call(std::vector<Value> args) override {
    for (auto &a : args) {
      if (!std::holds_alternative<std::shared_ptr<NinNative>>(a) ||
        std::get<std::shared_ptr<NinNative>>(a)->typeName != "Vector2")
        throw std::runtime_error("vec2add(): both arguments must be Vector2.");
    }

    auto aData = std::static_pointer_cast<Vector2>(
      std::get<std::shared_ptr<NinNative>>(args[0])->data);
    auto bData = std::static_pointer_cast<Vector2>(
      std::get<std::shared_ptr<NinNative>>(args[1])->data);

    auto result = std::make_shared<Vector2>();
    result->x = aData->x + bData->x;
    result->y = aData->y + bData->y;
    return wrapVector2(result);
  }
};

extern "C" void carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["vec2"]    = std::make_shared<Vector2NewFn>();
  (*out)["vec2add"] = std::make_shared<Vector2AddFn>();
}
