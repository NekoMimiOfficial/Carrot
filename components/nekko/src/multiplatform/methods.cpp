#include "utils/methods.h"
#include "meta.h"
#include "value.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

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

std::string getVerString() {
  std::ostringstream ver_string;
  ver_string << c_assemble_appver.maj << "." << c_assemble_appver.min << "."
             << c_assemble_appver.fix;

  return ver_string.str();
}

void strReplace(std::string &base, const std::string &old,
                const std::string &new_w) {
  if (old.empty())
    return;

  size_t start_pos = 0;
  while ((start_pos = base.find(old, start_pos)) != std::string::npos) {
    base.replace(start_pos, old.length(), new_w);

    start_pos += new_w.length();
  }
}

std::string strJoin(std::shared_ptr<NinArray> items, std::string delimiter) {
  if (items->elements.empty())
    return "";

  size_t tsize = 0;
  for (const auto &s : items->elements)
    tsize += valueToString(s).size();
  tsize += delimiter.size() * (items->elements.size() - 1);

  std::string res;
  res.reserve(tsize);

  res += valueToString(items->elements[0]);
  for (size_t i = 1; i < items->elements.size(); ++i) {
    res += delimiter;
    res += valueToString(items->elements[i]);
  }

  return res;
}

std::string valueToString(const Value &val) {
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

  if (std::holds_alternative<uint8_t>(val)) {
    uint8_t byte = std::get<uint8_t>(val);
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<uint32_t>(byte);
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

bool isTruthy(const Value &val) {
  if (std::holds_alternative<std::monostate>(val))
    return false;
  if (std::holds_alternative<double>(val))
    return (std::get<double>(val) == 0) ? false : true;
  if (std::holds_alternative<bool>(val))
    return std::get<bool>(val);
  return true;
}

bool isEqual(const Value &a, const Value &b) {
  if (std::holds_alternative<std::shared_ptr<NinArray>>(a) &&
      std::holds_alternative<std::shared_ptr<NinArray>>(b)) {
    return std::get<std::shared_ptr<NinArray>>(a).get() ==
           std::get<std::shared_ptr<NinArray>>(b).get();
  }
  return a == b;
}

bool isInt(double d) {
  uint64_t bits;
  std::memcpy(&bits, &d, sizeof(bits));

  int32_t exponent = ((bits >> 52) & 0x7FF) - 1023;

  if (exponent >= 52) {
    return exponent == 1024 ? false : true;
  }
  if (exponent < 0) {
    return d == 0.0;
  }

  uint64_t fractional_mask = (1ULL << (52 - exponent)) - 1;
  return (bits & fractional_mask) == 0;
}
