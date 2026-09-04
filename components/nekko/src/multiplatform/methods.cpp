#include "methods.h"
#include "meta.h"
#include "value.h"
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

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

std::string getVerString() {
  std::ostringstream ver_string;
  ver_string << c_assemble_appver.maj << "." << c_assemble_appver.min << "."
             << c_assemble_appver.fix;

  return ver_string.str();
}

std::shared_ptr<NinArray> strSplit(std::string base, std::string delimiter) {
  std::vector<Value> segments;

  size_t start = 0;
  size_t end = base.find(delimiter);

  while (end != std::string::npos) {
    segments.push_back(base.substr(start, end - start));

    start = end + delimiter.length();
    end = base.find(delimiter, start);
  }

  segments.push_back(base.substr(start));

  return std::make_shared<NinArray>(std::move(segments));
}
