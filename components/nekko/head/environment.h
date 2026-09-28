#pragma once
#include "lang.h"
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
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), false, false};
  }

  void defineConst(const std::string &name, Value value) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), true, false};
  }

  void defineGlobal(const std::string &name, Value value) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    slots[name] = Slot{std::move(value), false, true};
  }

  void defineMutex(const std::string &name, Value value) {
    defineGlobal(name, std::move(value));
  }

  void defineAlias(const std::string &name, std::shared_ptr<Environment> target,
                   const std::string &targetName) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    ensureUndefinedLocked(name);
    Slot slot;
    slot.aliasTarget = std::move(target);
    slot.aliasName = targetName;
    slots[name] = std::move(slot);
  }

  Value get(const std::string &name) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end()) {
        if (it->second.aliasTarget)
          return it->second.aliasTarget->get(it->second.aliasName);
        return it->second.value;
      }
    }
    throw std::runtime_error(LOC(UDEF_VAR, name));
  }

  void assign(const std::string &name, Value value) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());

    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end() && it->second.isConst)
        throw std::runtime_error(LOC(ASSIGN_TO_CONST_VAR, name));
    }

    for (Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it == e->slots.end())
        continue;
      if (it->second.aliasTarget) {
        it->second.aliasTarget->assign(it->second.aliasName, std::move(value));
        return;
      }
      if (it->second.isGlobal) {
        it->second.value = std::move(value);
        return;
      }
      break;
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

    throw std::runtime_error(LOC(ASSIGN_TO_UDEF_VAR, name));
  }

  void free(const std::string &name) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    for (Environment *e = this; e; e = e->parent.get()) {
      if (e->slots.erase(name))
        return;
    }
    throw std::runtime_error(LOC(FREE_UDEF_VAR, name));
  }

  bool hasLocal(const std::string &name) const {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    return slots.count(name) > 0;
  }

  bool isConst(const std::string &name) const {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    for (const Environment *e = this; e; e = e->parent.get()) {
      auto it = e->slots.find(name);
      if (it != e->slots.end())
        return it->second.isConst;
    }
    return false;
  }

  void defineCapture(const std::string &name, Value value) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    if (slots.count(name))
      throw std::runtime_error(LOC(PREDEFINED_VAR, name));
    slots[name] = Slot{std::move(value), false, false};
  }

  void defineAliasCapture(const std::string &name,
                          std::shared_ptr<Environment> target,
                          const std::string &targetName) {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
    if (slots.count(name))
      throw std::runtime_error(LOC(PREDEFINED_VAR, name));
    Slot slot;
    slot.aliasTarget = std::move(target);
    slot.aliasName = targetName;
    slots[name] = std::move(slot);
  }

  std::unordered_map<std::string, Value> exportAll() const {
    std::lock_guard<std::recursive_mutex> lock(lockMutex());
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
    std::shared_ptr<Environment> aliasTarget;
    std::string aliasName;
  };

  static std::recursive_mutex &lockMutex() {
    static std::recursive_mutex m;
    return m;
  }

  void ensureUndefinedLocked(const std::string &name) const {
    for (const Environment *e = this; e; e = e->parent.get()) {
      if (e->slots.count(name))
        throw std::runtime_error(LOC(PREDEFINED_VAR, name));
    }
  }

  std::unordered_map<std::string, Slot> slots;
};
