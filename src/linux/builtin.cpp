#include "builtin.h"
#include "asset_store.h"
#include "fs.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <variant>

Value ImportFn::call(std::vector<Value> args) {
  if (!std::holds_alternative<std::string>(args[0]))
    throw std::runtime_error("import(): argument must be a string path.");

  std::filesystem::path path;
  std::string rel = std::get<std::string>(args[0]);
  if (!rel.empty() and rel.front() == '@') {
    auto pget = getAsset("modules/" + rel.substr(1));
    if (std::holds_alternative<std::string>(pget)) {
      path = std::get<std::string>(pget);
    } else {
      throw std::runtime_error("import(): cannot open '" + path.string() +
                               "'.");
    }
  } else {
    path = std::filesystem::path(callerDir) / rel;
  }

  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("import(): cannot open '" + path.string() + "'.");

  std::ostringstream ss;
  ss << file.rdbuf();
  std::string source = ss.str();

  Lexer modLex(source);
  auto tokens = modLex.tokenize();
  Parser modParser(std::move(tokens));
  auto stmts = modParser.parse();

  auto mod = std::make_shared<NinModule>();
  mod->sourcePath = path.string();
  mod->ast = std::move(stmts);

  auto modEnv = std::make_shared<Environment>(interp->globals);
  interp->executeBlock(mod->ast, modEnv);
  mod->members = modEnv->exportAll();
  return mod;
}

Value ExitFn::call(std::vector<Value> args) {
  if (!(std::holds_alternative<double>(args[0]))) {
    throw std::runtime_error("exit(): argument must be an integer");
  }
  double get_arg = std::get<double>(args[0]);
  if (!(isInt(get_arg))) {
    throw std::runtime_error("exit(): argument must be an integer");
  }
  int ret_code = static_cast<int>(get_arg);
  exit(ret_code);
}

Value SleepFn::call(std::vector<Value> args) {
  if (!(std::holds_alternative<double>(args[0]))) {
    throw std::runtime_error("sleep(): argument must be an integer");
  }
  double get_arg = std::get<double>(args[0]);
  if (!(isInt(get_arg))) {
    throw std::runtime_error("sleep(): argument must be an integer");
  }

  int sleep_ms = static_cast<int>(get_arg);
  usleep(sleep_ms * 1000);

  return std::monostate();
}
