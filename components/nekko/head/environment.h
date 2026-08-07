#pragma once
#include "value.h"
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Environment {
public:
  std::shared_ptr<Environment> parent;

  explicit Environment(std::shared_ptr<Environment> parent = nullptr)
      : parent(std::move(parent)) {}

  void define(const std::string &name, Value value) {
    std::lock_guard<std::mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), false, false};
  }

  void defineConst(const std::string &name, Value value) {
    std::lock_guard<std::mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), true, false};
  }

  void defineGlobal(const std::string &name, Value value) {
    std::lock_guard<std::mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), false, true};
  }

  void defineMutex(const std::string &name, Value value) {
    defineGlobal(name, std::move(value));
  }

  Value get(const std::string &name) {
    std::lock_guard<std::mutex> lock(lockMutex());
    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end())
        return it->second.value;
    }
    throw std::runtime_error("Undefined variable '" + name + "'.");
  }

  void assign(const std::string &name, Value value) {
    std::lock_guard<std::mutex> lock(lockMutex());

    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end() && it->second.isConst)
        throw std::runtime_error("Cannot assign to const '" + name + "'.");
    }

    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end() && it->second.isGlobal) {
        it->second.value = std::move(value);
        return;
      }
    }

    auto local = slots.find(name);
    if (local != slots.end()) {
      local->second.value = std::move(value);
      return;
    }

    for (Environment *e = parent.get(); e; e = e->parent.get()) {
      if (e->slots.count(name)) {
        slots[name] = Slot{std::move(value), false, false};
        return;
      }
    }

    throw std::runtime_error("Cannot assign to undefined variable '" + name +
                             "'.");
  }

  void free(const std::string &name) {
    std::lock_guard<std::mutex> lock(lockMutex());
    for (Environment *e = this; e; e = e->parent.get()) {
      if (e->slots.erase(name))
        return;
    }
    throw std::runtime_error("Cannot free undefined variable '" + name + "'.");
  }

  bool hasLocal(const std::string &name) const {
    std::lock_guard<std::mutex> lock(lockMutex());
    return slots.count(name) > 0;
  }

  bool isConst(const std::string &name) const {
    std::lock_guard<std::mutex> lock(lockMutex());
    for (const Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end())
        return it->second.isConst;
    }
    return false;
  }

  std::unordered_map<std::string, Value> exportAll() const {
    std::lock_guard<std::mutex> lock(lockMutex());
    std::unordered_map<std::string, Value> out;
    for (auto &[k, slot] : slots)
      out[k] = slot.value;
    return out;
  }

private:
  struct Slot {
    Value value;
    bool isConst = false;
    bool isGlobal = false;
  };

  static std::mutex &lockMutex() {
    static std::mutex m;
    return m;
  }

  void ensureUndefinedLocked(const std::string &name) const {
    for (const Environment *e = this; e; e = e->parent.get()) {
      if (e->slots.count(name))
        throw std::runtime_error("'" + name + "' is already defined.");
    }
  }

  std::unordered_map<std::string, Slot> slots;
};
