#pragma once
#include "value.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

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

  void defineGlobal(const std::string &name, Value value) {
    Environment *e = this->parent ? this->parent.get() : this;
    while (e) {
      if (e->globals.count(name))
        throw std::runtime_error("'" + name +
                                 "' is already defined as global.");
      e = e->parent.get();
    }
    globals.insert(name);
    values[name] = std::move(value);
  }

  void defineConst(const std::string &name, Value value) {
    Environment *e = this;
    while (e) {
      if (e->values.count(name))
        throw std::runtime_error("'" + name + "' is already defined.");
      e = e->parent.get();
    }
    consts.insert(name);
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
    Environment *e = this;
    while (e) {
      if (e->consts.count(name))
        throw std::runtime_error("Cannot assign to const '" + name + "'.");
      e = e->parent.get();
    }

    e = this;
    while (e) {
      if (e->globals.count(name)) {
        e->values[name] = std::move(value);
        return;
      }
      e = e->parent.get();
    }

    if (values.count(name)) {
      values[name] = std::move(value);
      return;
    }

    e = parent.get();
    while (e) {
      if (e->values.count(name)) {
        values[name] = std::move(value);
        return;
      }
      e = e->parent.get();
    }

    throw std::runtime_error("Cannot assign to undefined variable '" + name +
                             "'.");
  }

  bool hasLocal(const std::string &name) const {
    return values.count(name) > 0;
  }

  bool isConst(const std::string &name) const {
    const Environment *e = this;
    while (e) {
      if (e->consts.count(name))
        return true;
      e = e->parent.get();
    }
    return false;
  }

  std::unordered_map<std::string, Value> exportAll() const { return values; }

private:
  std::unordered_map<std::string, Value> values;
  std::unordered_set<std::string> globals;
  std::unordered_set<std::string> consts;
};
