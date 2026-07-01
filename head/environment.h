#pragma once
#include "value.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Environment {
public:
  std::shared_ptr<Environment> parent;

  explicit Environment(std::shared_ptr<Environment> parent = nullptr)
      : parent(std::move(parent)) {}

  void define(const std::string &name, Value value) {
    Environment *e = this;
    while (e) {
      if (e->values.count(name))
        throw std::runtime_error("'" + name + "' is already defined.");
      e = e->parent.get();
    }
    values[name] = std::move(value);
  }

  Value &get(const std::string &name) {
    auto it = values.find(name);
    if (it != values.end())
      return it->second;

    if (parent)
      return parent->get(name);

    throw std::runtime_error("Undefined variable '" + name + "'.");
  }

  const Value &get(const std::string &name) const {
    auto it = values.find(name);
    if (it != values.end())
      return it->second;

    if (parent)
      return parent->get(name);

    throw std::runtime_error("Undefined variable '" + name + "'.");
  }

  void assign(const std::string &name, Value value) {
    auto it = values.find(name);
    if (it != values.end()) {
      it->second = std::move(value);
      return;
    }

    if (parent) {
      parent->assign(name, std::move(value));
      return;
    }

    throw std::runtime_error("Cannot assign to undefined variable '" + name +
                             "'. "
                             "Use 'let' to declare it first.");
  }

  bool hasLocal(const std::string &name) const {
    return values.count(name) > 0;
  }

  std::unordered_map<std::string, Value> exportAll() const { return values; }

private:
  std::unordered_map<std::string, Value> values;
};
