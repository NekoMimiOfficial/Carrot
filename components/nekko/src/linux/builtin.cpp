#include "builtin.h"
#include "asset_store.h"
#include "fs.h"
#include "utils/methods.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <variant>

static std::vector<std::vector<StmtPtr>> &importedAsts() {
  static std::vector<std::vector<StmtPtr>> registry;
  return registry;
}

Value ImportFn::call(std::vector<Value> args) {
  if (!std::holds_alternative<std::string>(args[0]))
    throw std::runtime_error("import(): argument must be a string path.");

  std::filesystem::path path;
  std::string rel = std::get<std::string>(args[0]);
  if (!rel.empty() and rel.front() == '@') {
    auto pget = getAsset("modules/" + (rel.substr(1) + ".nin"));
    if (std::holds_alternative<std::string>(pget)) {
      path = std::get<std::string>(pget);
    } else {
      throw std::runtime_error("import(): cannot open '" + path.string() +
                               "'.");
    }
  } else {
    path = std::filesystem::path(callerDir) / rel;
  }

  std::string cacheKey =
      std::filesystem::absolute(path).lexically_normal().string();
  Value cached = interp->getCachedModule(cacheKey);
  if (!std::holds_alternative<std::monostate>(cached))
    return cached;

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

  importedAsts().push_back(std::move(stmts));
  auto &keptStmts = importedAsts().back();

  auto modEnv = std::make_shared<Environment>(interp->globals);
  interp->executeBlock(keptStmts, modEnv);
  mod->members = modEnv->exportAll();
  interp->cacheModule(cacheKey, mod);

  return mod;
}

// TODO: fix exit only looking for kwarg and not arg too
Value ExitFn::callWithKwargs(std::vector<Value> args,
                             std::unordered_map<std::string, Value> kwargs) {

  double ecode = 0;
  auto it = kwargs.find("exit_code");
  if (it != kwargs.end()) {
    if (!std::holds_alternative<double>(it->second))
      throw std::runtime_error("exit(): 'exit_code' must be an integer.");
    ecode = std::get<double>(it->second);
    kwargs.erase(it);
  }
  if (!kwargs.empty())
    throw std::runtime_error("exit(): unknown keyword argument '" +
                             kwargs.begin()->first + "'.");

  if (!(isInt(ecode))) {
    throw std::runtime_error("exit(): argument must be an integer");
  }
  int ret_code = static_cast<int>(ecode);
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
