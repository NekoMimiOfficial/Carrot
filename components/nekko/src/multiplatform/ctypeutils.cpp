#include "utils/ctypeutils.h"
#include "value.h"
#include <cstdint>
#include <string>
#include <variant>

std::string getType(Value arg) {
  const Value &v = arg;
  if (std::holds_alternative<std::monostate>(v))
    return std::string("nil");
  if (std::holds_alternative<double>(v))
    return std::string("number");
  if (std::holds_alternative<uint8_t>(v))
    return std::string("byte");
  if (std::holds_alternative<std::string>(v))
    return std::string("string");
  if (std::holds_alternative<bool>(v))
    return std::string("bool");
  if (std::holds_alternative<std::shared_ptr<NinCallable>>(v))
    return std::string("function");
  if (std::holds_alternative<std::shared_ptr<NinArray>>(v))
    return std::string("array");
  if (std::holds_alternative<std::shared_ptr<NinModule>>(v))
    return std::string("module");
  if (std::holds_alternative<std::shared_ptr<NinClass>>(v))
    return std::string("class");
  if (std::holds_alternative<std::shared_ptr<NinInstance>>(v))
    return std::string("instance");
  if (std::holds_alternative<std::shared_ptr<NinCoroutine>>(v))
    return std::string("coroutine");
  if (std::holds_alternative<std::shared_ptr<NinNative>>(v))
    return std::get<std::shared_ptr<NinNative>>(v)->typeName;
  return std::string("unknown");
}

bool checkArgs(Value arg, std::string type) { return (type == getType(arg)); }

