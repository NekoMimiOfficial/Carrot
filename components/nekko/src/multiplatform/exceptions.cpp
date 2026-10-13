#include "exceptions.h"
#include "lang.h"
#include "nin_types.h"
#include "utils/methods.h"

struct ExceptionInit : NinNativeMethod {
  int arity() override { return 2; }
  std::string name() override { return "init"; }

  Value callBound(std::shared_ptr<NinInstance> self, std::vector<Value> args,
                  std::unordered_map<std::string, Value> kwargs) override {
    if (args.size() > 2)
      throw std::runtime_error(
          LOC(FN_TOO_MANY_ARGS, "init", "2", std::to_string(args.size())));

    Value message = std::string();
    Value ret = 1.0;
    if (args.size() >= 1)
      message = std::move(args[0]);
    if (args.size() >= 2)
      ret = std::move(args[1]);

    auto m = kwargs.find("message");
    if (m != kwargs.end()) {
      message = std::move(m->second);
      kwargs.erase(m);
    }
    auto r = kwargs.find("ret");
    if (r != kwargs.end()) {
      ret = std::move(r->second);
      kwargs.erase(r);
    }

    if (!kwargs.empty())
      throw std::runtime_error(
          LOC(FN_UNEXPECTED_KWARG, "init", kwargs.begin()->first));
    if (!std::holds_alternative<double>(ret))
      throw std::runtime_error(LOC(EXCEPTION_RET_NOT_NUMBER));

    self->fields["message"] = std::move(message);
    self->fields["ret"] = std::move(ret);
    self->fields["type"] = self->klass->className;
    return std::monostate{};
  }
};

void registerExceptionTypes(Interpreter *interp) {
  auto init = std::make_shared<ExceptionInit>();

  auto base = std::make_shared<NinClass>("exception");
  base->methods["init"] = init;
  interp->registerBuiltinClass(base);

  for (const char *name : {"runtime_error", "type_error", "value_error",
                           "index_error", "name_error", "io_error"}) {
    auto klass = std::make_shared<NinClass>(name, base);
    klass->methods["init"] = init;
    interp->registerBuiltinClass(klass);
  }
}

bool isInstanceOf(const Value &v, const std::shared_ptr<NinClass> &klass) {
  auto inst = std::get_if<std::shared_ptr<NinInstance>>(&v);
  if (!inst || !*inst)
    return false;
  for (auto k = (*inst)->klass; k; k = k->superclass)
    if (k == klass)
      return true;
  return false;
}

Value makeRuntimeError(Interpreter &interp, const std::string &message) {
  std::shared_ptr<NinClass> klass;
  try {
    Value v = interp.globals->get("runtime_error");
    if (auto k = std::get_if<std::shared_ptr<NinClass>>(&v))
      klass = *k;
  } catch (const std::exception &) {
  }
  if (!klass)
    klass = std::make_shared<NinClass>("runtime_error");

  auto inst = std::make_shared<NinInstance>(klass);
  inst->fields["message"] = message;
  inst->fields["ret"] = 1.0;
  inst->fields["type"] = klass->className;
  return inst;
}

std::string exceptionText(const Value &v) {
  if (auto inst = std::get_if<std::shared_ptr<NinInstance>>(&v)) {
    if (*inst) {
      auto &fields = (*inst)->fields;
      auto m = fields.find("message");
      std::string msg = (m != fields.end()) ? valueToString(m->second) : "";
      return LOC(UNCAUGHT_EXCEPTION, (*inst)->klass->className, msg);
    }
  }
  return LOC(UNCAUGHT_VALUE, valueToString(v));
}

int exceptionRet(const Value &v) {
  if (auto inst = std::get_if<std::shared_ptr<NinInstance>>(&v)) {
    if (*inst) {
      auto &fields = (*inst)->fields;
      auto r = fields.find("ret");
      if (r != fields.end())
        if (auto d = std::get_if<double>(&r->second))
          return (int)*d;
    }
  }
  return 1;
}
